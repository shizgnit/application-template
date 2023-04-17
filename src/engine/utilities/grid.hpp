/*
================================================================================
  Copyright (c) 2023, Pandemos
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

namespace stage {

    class grid {
    public:
        void define(int w, int x, int y, int l, int m) {
            width = w;
            x_offset = x;
            y_offset = y;
            watermark = l;
            height = l;
            margin = m;
        }

        typedef type::entity::instance_t identifier_t;
        typedef std::pair<int, int> quadrant_t;
        typedef std::function<bool(quadrant_t&)> callback_t;

        struct type_t {
            identifier_t id = 0;
            int priority = -1;
            callback_t factory = NULL; // TODO: get rid of this eventually
            int probability = 0;
            std::vector<std::pair<spatial::position, type::entity::instance_t>> member;
            operator bool() {
                return id != 0;
            }
            bool operator == (const type_t& r) {
                return id == r.id;
            }
            bool operator < (const type_t& r) const {
                return id < r.id;
            }
       } type_empty_t;

        struct context_t {
            grid::type_t behind;
            grid::type_t left;
            grid::type_t right;
        };

        struct {
            std::map<int, int> created_priorities;
            std::map<identifier_t, quadrant_t> first_seen;
            std::map<identifier_t, quadrant_t> last_seen;
            std::map<identifier_t, quadrant_t> last_seen_priority;
        } stats;

        typedef std::function<bool(quadrant_t&, grid::type_t&, context_t&)> limit_t;

        type_t getGenericType() {
            static type_t noop = addType(0, [](grid::quadrant_t& q) -> bool { return true; },
                { [](grid::quadrant_t q, grid::type_t& i, grid::context_t& c) -> bool { return false; } });
            return noop;
        }

        int getWidth() {
            return width;
        }
        int getHeight() {
            return height;
        }

        quadrant_t getQuadrant(spatial::vector::type_t x, spatial::vector::type_t z) {
            int nx = floor(x + x_offset);
            int nz = floor(z + y_offset);

            double tx = (nx % 2 ? nx + 1 : nx + 2) / 2.0;
            double tz = (nz % 2 ? nz + 1 : nz + 2) / 2.0;

            return { tx, tz * -1 };
        }

        spatial::position getQuadrantPosition(quadrant_t q, double y = 0.0) {
            double nx = (q.first * 2.0) - 1 - x_offset;
            double nz = (q.second * -2.0) - 1 - y_offset;
            return spatial::position({ (float)nx, (float)y, (float)nz });
        }

        void setQuadrant(quadrant_t q, grid::type_t instance) {
            if (q.first < 0 || q.second < 0) {
                return;
            }
            if (data.size() <= q.second) {
                data.resize(q.second + 1);
            }
            if (data[q.second].size() == 0) {
                data[q.second].resize(width + 1);
            }
            data[q.second][q.first] = instance;
            types[instance.id].factory(q);
        }
        void setQuadrant(quadrant_t q, grid::identifier_t instance) {
            if (q.first < 0 || q.second < 0) {
                return;
            }
            if (instances.size() <= q.second) {
                instances.resize(q.second + 1);
            }
            if (instances[q.second].size() == 0) {
                instances[q.second].resize(width + 1);
            }
            instances[q.second][q.first].push_back(instance);
        }

        grid::type_t& getQuadrantType(quadrant_t q) {
            if (data.size() > q.second && data[q.second].size() > q.first && q.second && q.first) {
                return data[q.second][q.first];
            }
            return type_empty_t;
        }
        std::vector<grid::identifier_t>& getQuadrantInstances(quadrant_t q) {
            static std::vector<grid::identifier_t> empty;
            if (instances.size() > q.second && instances[q.second].size() > q.first && q.second && q.first) {
                return instances[q.second][q.first];
            }
            return empty;
        }

        grid::type_t& peekLeft(quadrant_t q, int priority = -1) {
            auto& result = getQuadrantType({ q.first - 1, q.second });
            if (priority != -1 && result.priority != priority) {
                return type_empty_t;
            }
            return result;
        }

        grid::type_t& peekRight(quadrant_t q, int priority = -1) {
            auto& result = getQuadrantType({ q.first + 1, q.second });
            if (priority != -1 && result.priority != priority) {
                return type_empty_t;
            }
            return result;
        }

        grid::type_t& peekAhead(quadrant_t q, int priority = -1) {
            auto& result = getQuadrantType({ q.first, q.second + 1 });
            if (priority != -1 && result.priority != priority) {
                return type_empty_t;
            }
            return result;
        }

        grid::type_t& peekBehind(quadrant_t q, int priority = -1) {
            auto& result = getQuadrantType({ q.first, q.second - 1 });
            if (priority != -1 && result.priority != priority) {
                return type_empty_t;
            }
            return result;
        }

        grid::type_t& addType(int g, callback_t c) {
            if (types.size() == 0) {
                types.resize(1);
            }
            types.push_back({ (identifier_t)types.size(), g, c });
            identifier_t added = types.size() - 1;
            priorities[g].push_back(types[added]);
            return types[added];
        }

        grid::type_t& addType(int g, callback_t c, std::vector<limit_t> limit) {
            auto& added = addType(g, c);
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
        bool hasLeftConstraint(identifier_t l) {
            return(left.find(l) != left.end() && left[l].size());
        }

        bool hasRightConstraint(identifier_t r, identifier_t l) {
            return(right.find(r) != right.end() && std::find(right[r].begin(), right[r].end(), l) != right[r].end());
        }
        bool hasRightConstraint(identifier_t r) {
            return(right.find(r) != right.end() && right[r].size());
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
            for (; watermark <= y; watermark++) {
                stats.created_priorities.clear();
                stats.first_seen.clear();
                stats.last_seen.clear();
                for (auto priority : priorities) {
                    if (priority.first == 0) {
                        continue;
                    }
                    for (int x = margin; x < (width - margin); x++) {
                        quadrant_t q({ x, watermark });
                        if (getQuadrantType(q).id) {
                            continue;
                        }
                        if (generateQuadrant(q, priority.first)) {
                            stats.created_priorities[priority.first] += 1;
                        }
                    }
                }
            }
        }

        bool generateQuadrant(quadrant_t q, int priority) {
            if (q.first < margin || q.first >= (width - margin)) {
                return false;
            }

            std::vector<identifier_t> candidates;

            context_t context = {
                peekBehind(q),
                peekLeft(q),
                peekRight(q)
            };

            for (grid::type_t i : priorities[priority]) {
                auto limiters = limits.find(i.id);
                bool include = true;
                if (limiters != limits.end()) {
                    for (auto limit : limiters->second) {
                        if (limit(q, i, context) == false) {
                            include = false;
                            break;
                        }
                    }
                }
                if (include) {
                    candidates.push_back(i.id);
                }
            }
            if (candidates.size() == 0) {
                return false;
            }

            //int selection = candidates.size() * ((utilities::perlin(q.first, q.second) * 0.5) + 0.5);
            int selection = rand() % candidates.size();

            auto added = types[candidates[selection]];
            setQuadrant(q, added);

            if (stats.first_seen.find(added.id) == stats.first_seen.end()) {
                stats.first_seen[added.id] = q;
            }
            stats.last_seen[added.id] = q;
            stats.last_seen_priority[priority] = q;

            if (peekLeft(q).id == 0 && right.find(added.id) != right.end()) {
                generateQuadrant({ q.first - 1, q.second }, priority);
            }
            if (peekRight(q).id == 0 && left.find(added.id) != left.end()) {
                generateQuadrant({ q.first + 1, q.second }, priority);
            }

            return true;
        }

        void hide(grid::quadrant_t q) {
            auto& instances = getQuadrantInstances(q);
            for (auto instance : instances) {
                auto& e = type::entity::find(instance);
                if (e.object && e.object->height() < 0.4) {
                    return;
                }
                auto& i = e.getInstance(instance);
                i.flags |= type::entity::VIRTUAL;
                i.update();
            }
            hidden.push_back(q);
        }
        void unhide() {
            for (auto q : hidden) {
                auto& instances = getQuadrantInstances(q);
                for (auto instance : instances) {
                    auto& e = type::entity::find(instance);
                    auto& i = e.getInstance(instance);
                    i.flags &= ~type::entity::VIRTUAL;
                    i.update();
                }
            }
            hidden.clear();
        }

        void addRule(type_t& t, std::vector<std::pair<spatial::vector, type::entity::instance_t>>& p) {
            rules[t] = p;
        }

    protected:
        std::map<identifier_t, std::vector<limit_t>> limits;

        std::vector<grid::type_t> types;

        std::map<identifier_t, std::vector<grid::type_t>> priorities;

        std::vector<std::vector<grid::type_t>> data;
        std::vector<std::vector<std::vector<grid::identifier_t>>> instances;

        std::map<identifier_t, std::vector<identifier_t>> left;
        std::map<identifier_t, std::vector<identifier_t>> right;
        std::map<identifier_t, std::vector<identifier_t>> behind;
        std::map<identifier_t, std::vector<identifier_t>> ahead;

        std::map<type_t, std::vector<std::pair<spatial::vector, type::entity::instance_t>>> rules;

        std::vector<grid::quadrant_t> hidden;

        int width = 0;
        int height = 0;

        int x_offset = 0;
        int y_offset = 0;

        int watermark = 0;
        int margin = 0;
    };

}
