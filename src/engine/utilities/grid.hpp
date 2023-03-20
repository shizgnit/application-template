/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

class grid {
public:
    void define(int w, int x, int y, int l, int m) {
        width = w;
        x_offset = x;
        y_offset = y;
        watermark = l;
        margin = m;
    }
    
    typedef unsigned long identifier_t;
    typedef std::pair<int, int> quadrant_t;
    typedef std::function<bool(quadrant_t &)> callback_t;

    struct type_t {
        identifier_t id = 0;
        int group = -1;
        callback_t factory = NULL;
    } type_empty_t;

    struct context_t {
        grid::type_t behind;
        grid::type_t left;
        grid::type_t right;
    };
   
    struct {
        std::map<int, int> created_groups;
        std::map<identifier_t, int> last_seen;
        std::map<identifier_t, int> last_seen_group;
    } stats;
    
    typedef std::function<bool(quadrant_t &, grid::type_t &, context_t &)> limit_t;
 
    type_t getGenericType() {
        static type_t noop = addType(0, [](grid::quadrant_t &q) -> bool { return true; },
            [](grid::quadrant_t q, grid::type_t& i, grid::context_t& c) -> bool { return false; });
        return noop;
    }
   
    int getWidth() {
        return width;
    }

    quadrant_t getQuadrant(spatial::vector::type_t x, spatial::vector::type_t z) {
        int nx = floor(x + x_offset);
        int nz = floor(z + y_offset);
        
        double tx = (nx % 2 ? nx + 1: nx + 2) / 2.0;
        double tz = (nz % 2 ? nz + 1: nz + 2) / 2.0;
        
        return { tx, tz * -1 };
    }
    
    spatial::position getQuadrantPosition(quadrant_t q, double y=0.0) {
        double nx = (q.first * 2.0) - 1 - x_offset;
        double nz = (q.second * -2.0) - 1 - y_offset;
        return spatial::position({ (float)nx, (float)y, (float)nz });
    }
    
    void setQuadrant(quadrant_t q, grid::type_t instance) {
        if (q.first < 0 || q.second < 0) {
            return;
        }
        if(data.size() <= q.second) {
            data.resize(q.second + 1);
        }
        if(data[q.second].size() == 0) {
            data[q.second].resize(width + 1);
        }
        data[q.second][q.first] = instance;
        types[instance.id].factory(q);
    }
    
    grid::type_t &inQuadrant(quadrant_t q) {
        if(data.size() > q.second && data[q.second].size() > q.first && q.second && q.first) {
            return data[q.second][q.first];
        }
        return type_empty_t;
    }
    
    grid::type_t &peekLeft(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first - 1, q.second });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
    }

    grid::type_t &peekRight(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first + 1, q.second });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
   }

    grid::type_t &peekBehind(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first, q.second - 1 });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
    }
    
    grid::type_t &peekForward(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first, q.second + 1 });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
    }

    grid::type_t &addType(int g, callback_t c) {
        if(types.size() == 0) {
            types.resize(1);
        }
        types.push_back({(identifier_t)types.size(), g, c});
        identifier_t added = types.size()-1;
        groups[g].push_back(types[added]);
        return types[added];
    }

    grid::type_t &addType(int g, callback_t c, limit_t limit) {
        auto &added = addType(g, c);
        limits[added.id] = limit;
        return added;
    }

    void addLeftConstraint(grid::type_t adding, grid::type_t required) {
        left[required.id].push_back(adding.id);
        right[adding.id].push_back(required.id);
    }
    void addRightConstraint(grid::type_t adding, grid::type_t required) {
        right[required.id].push_back(adding.id);
        left[adding.id].push_back(required.id);
    }
    void addBehindConstraint(grid::type_t adding, grid::type_t required) {
        behind[required.id].push_back(adding.id);
        ahead[adding.id].push_back(required.id);
    }
   
    bool hasLeftConstraint(identifier_t l, identifier_t r) {
        return(left.find(l) != left.end() && std::find(left[l].begin(), left[l].end(), r) != left[l].end());
    }
    
    bool hasRightConstraint(identifier_t r, identifier_t l) {
        return(right.find(r) != right.end() && std::find(right[r].begin(), right[r].end(), l) != right[r].end());
    }
    
    bool hasBehindConstraint(identifier_t b, identifier_t a) {
        return(behind.find(b) != behind.end() && std::find(behind[b].begin(), behind[b].end(), a) != behind[b].end());
    }
    
    bool hasAheadConstraint(identifier_t a, identifier_t b) {
        return(ahead.find(a) != ahead.end() && std::find(ahead[a].begin(), ahead[a].end(), b) != ahead[a].end());
    }
  
    bool hasAheadConstraint(identifier_t a) {
        return(ahead.find(a) != ahead.end() && ahead[a].size());
    }
    
    void generateRow(int y) {
        for(; watermark<=y; watermark++) {
            stats.created_groups.clear();
            for(auto group: groups) {
                if (group.first == 0) {
                    continue;
                }
                for(int x=margin; x<(width - margin); x++) {
                    quadrant_t q({x, watermark});
                    if(inQuadrant(q).id) {
                        continue;
                    }
                    if(generateQuadrant(q, group.first)) {
                        stats.created_groups[group.first] += 1;
                    }
                }
            }
        }
    }
    
    bool generateQuadrant(quadrant_t q, int group) {
        if(q.first < 0 || q.first >= width) {
            return false;
        }
    
        std::vector<identifier_t> candidates;
        
        context_t context = {
            peekBehind(q),
            peekLeft(q),
            peekRight(q)
        };
        
        for(grid::type_t i: groups[group]) {
            if(limits.find(i.id) != limits.end() && limits[i.id](q, i, context) == false) {
                continue;
            }
            candidates.push_back(i.id);
        }
        if(candidates.size() == 0) {
            return false;
        }
        
        //int selection = candidates.size() * ((utilities::perlin(q.first, q.second) * 0.5) + 0.5);
        int selection = rand() % candidates.size();
        
        auto added = types[candidates[selection]];
        setQuadrant(q, added);
        
        stats.last_seen[added.id] = q.second;
        stats.last_seen_group[group] = q.second;
        
        if(peekLeft(q).id == 0 && right.find(added.id) != right.end()) {
            generateQuadrant({ q.first - 1, q.second }, group);
        }
        if(peekRight(q).id == 0 && left.find(added.id) != left.end()) {
            generateQuadrant({ q.first + 1, q.second }, group);
        }

        return true;
    }
    
protected:
    std::map<identifier_t, limit_t> limits;

    std::vector<grid::type_t> types;
    
    std::map<identifier_t, std::vector<grid::type_t>> groups;
    
    std::vector<std::vector<grid::type_t>> data;

    std::map<identifier_t, std::vector<identifier_t>> left;
    std::map<identifier_t, std::vector<identifier_t>> right;
    std::map<identifier_t, std::vector<identifier_t>> behind;
    std::map<identifier_t, std::vector<identifier_t>> ahead;
    
    int width = 0;
    
    int x_offset = 0;
    int y_offset = 0;
    
    int watermark = 0;
    int margin = 0;
};

