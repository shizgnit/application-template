#pragma once

class grid {
public:
    void define(int w, int x, int y, int l) {
        width = w;
        x_offset = x;
        y_offset = y;
        watermark = l;
    }
    
    typedef unsigned long identifier_t;
    typedef std::pair<int, int> quadrant_t;
    typedef std::function<bool(grid &, quadrant_t &)> callback_t;

    struct type_t {
        identifier_t id = 0;
        int group = -1;
        callback_t factory = NULL;
    } type_empty_t;

    struct context_t {
        type_t behind;
        type_t left;
        type_t right;
    };
   
    struct {
        std::map<int, int> created_groups;
        std::map<identifier_t, int> last_seen;
    } stats;
    
    typedef std::function<bool(grid &, quadrant_t &, type_t &, context_t &)> limit_t;
    
    quadrant_t getQuadrant(float x, float z) {
        int nx = floor(x + x_offset);
        int nz = floor(z + y_offset);
        
        float tx = (nx % 2 ? nx + 1: nx + 2) / 2;
        float tz = (nz % 2 ? nz + 1: nz + 2) / 2;
        
        return { tx, tz * -1 };
    }
    
    spatial::position getQuadrantPosition(quadrant_t q, float y=0.0) {
        float nx = (q.first * 2) - 1 - x_offset;
        float nz = (q.second * -2) - 1 - y_offset;
        return spatial::position({ nx, y, nz });
    }
    
    void setQuadrant(quadrant_t q, type_t instance) {
        if(data.size() <= q.second) {
            data.resize(q.second + 1);
        }
        if(data[q.second].size() == 0) {
            data[q.second].resize(width + 1);
        }
        data[q.second][q.first] = instance;
        types[instance.id].factory(*this, q);
    }
    
    type_t &inQuadrant(quadrant_t q) {
        if(data.size() > q.second && data[q.second].size() > q.first && q.second && q.first) {
            return data[q.second][q.first];
        }
        return type_empty_t;
    }
    
    type_t &peekLeft(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first - 1, q.second });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
    }

    type_t &peekRight(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first + 1, q.second });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
   }

    type_t &peekBehind(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first, q.second - 1 });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
    }
    
    type_t &peekForward(quadrant_t q, int group = -1) {
        auto &result = inQuadrant({ q.first, q.second + 1 });
        if(group != -1 && result.group != group) {
            return type_empty_t;
        }
        return result;
    }

    type_t &addType(int g, callback_t c) {
        if(types.size() == 0) {
            types.resize(1);
        }
        types.push_back({types.size(), g, c});
        identifier_t added = types.size()-1;
        groups[g].push_back(types[added]);
        return types[added];
    }

    type_t &addType(int g, callback_t c, limit_t limit) {
        auto &added = addType(g, c);
        limits[added.id] = limit;
        return added;
    }

    void addLeftConstraint(type_t adding, type_t required) {
        left[required.id].push_back(adding.id);
        right[adding.id].push_back(required.id);
    }
    void addRightConstraint(type_t adding, type_t required) {
        right[required.id].push_back(adding.id);
        left[adding.id].push_back(required.id);
    }
    void addBehindConstraint(type_t adding, type_t required) {
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
                for(int x=0; x<width; x++) {
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
        
        for(type_t i: groups[group]) {
            if(limits.find(i.id) != limits.end() && limits[i.id](*this, q, i, context) == false) {
                continue;
            }
            candidates.push_back(i.id);
        }
        if(candidates.size() == 0) {
            return false;
        }
        
        int selection = candidates.size() * ((utilities::perlin(q.first, q.second) * 0.5) + 0.5);
        //int selection = rand() % candidates.size();
        
        auto added = types[candidates[selection]];
        setQuadrant(q, added);
        
        stats.last_seen[added.id] = q.second;
        
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

    std::vector<type_t> types;
    
    std::map<int, std::vector<type_t>> groups;
    
    std::vector<std::vector<type_t>> data;

    std::map<identifier_t, std::vector<identifier_t>> left;
    std::map<identifier_t, std::vector<identifier_t>> right;
    std::map<identifier_t, std::vector<identifier_t>> behind;
    std::map<identifier_t, std::vector<identifier_t>> ahead;
    
    int width = 0;
    
    int x_offset = 0;
    int y_offset = 0;
    
    int watermark = 0;
};

