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
    typedef std::function<void(grid &, quadrant_t)> callback_t;

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
    
    void setQuadrant(quadrant_t q, identifier_t instance) {
        if(data.size() <= q.second) {
            data.resize(q.second + 1);
        }
        if(data[q.second].size() == 0) {
            data[q.second].resize(width + 1);
        }
        data[q.second][q.first] = instance;
        types[instance].factory(*this, q);
    }
    
    identifier_t inQuadrant(quadrant_t q) {
        if(data.size() > q.second && data[q.second].size() > q.first) {
            return data[q.second][q.first];
        }
        return 0;
    }
    
    identifier_t peekLeft(quadrant_t q) {
        if(q.second < data.size() && q.first > 0) {
            return data[q.second][q.first - 1];
        }
        return 0;
    }

    identifier_t peekRight(quadrant_t q) {
        if(q.second < data.size() && q.first < (width-1)) {
            return data[q.second][q.first + 1];
        }
        return 0;
    }

    identifier_t peekBehind(quadrant_t q) {
        if(q.second <= data.size() && q.second > 0) {
            return data[q.second - 1][q.first];
        }
        return 0;
    }
    
    identifier_t peekForward(quadrant_t q) {
        if(data.size() > (q.second + 1)) {
            return data[q.second +1][q.first];
        }
        return 0;
    }

    identifier_t addType(callback_t c, float weight = 1.0f) {
        if(types.size() == 0) {
            types.resize(1);
        }
        types.push_back({types.size(), weight, c});
        return types.size()-1;
    }
    
    void addLeftConstraint(identifier_t adding, identifier_t required) {
        if(left.size() < types.size()) {
            left.resize(types.size());
        }
        left[required].push_back(adding);
        if(right.size() < types.size()) {
            right.resize(types.size());
        }
        right[adding].push_back(required);
    }
    void addRightConstraint(identifier_t adding, identifier_t required) {
        if(right.size() < types.size()) {
            right.resize(types.size());
        }
        right[required].push_back(adding);
        if(left.size() < types.size()) {
            left.resize(types.size());
        }
        left[adding].push_back(required);
    }
    void addBehindConstraint(identifier_t adding, identifier_t required) {
        if(behind.size() < types.size()) {
            behind.resize(types.size());
        }
        behind[required].push_back(adding);
    }
    
    void generateRow(int y) {
        for(; watermark<=y; watermark++) {
            for(int x=0; x<width; x++) {
                quadrant_t q({x, watermark});
                auto i = peekBehind(q);
                if(i && behind.size() && behind[i].size()) {
                    generateQuadrant(q);
                }
            }
        }
    }
    
    bool generateQuadrant(quadrant_t q) {
    
        auto bi = peekBehind(q);
        auto li = peekLeft(q);
        auto ri = peekRight(q);

        std::vector<identifier_t> candidates;
        for(identifier_t i=0; i<types.size(); i++) {
            if(bi > 0 && behind[bi].size() && std::find(behind[bi].begin(), behind[bi].end(), i) == behind[bi].end()) {
                continue;
            }
            if(li > 0 && left[li].size() && std::find(left[li].begin(), left[li].end(), i) == left[li].end()) {
                continue;
            }
            if(ri > 0 && right[ri].size() && std::find(right[ri].begin(), right[ri].end(), i) == right[ri].end()) {
                continue;
            }
            candidates.push_back(i);
        }
        if(candidates.size() == 0) {
            return false;
        }
        
        setQuadrant(q, *candidates.begin());
        
        return true;
    }
    
protected:
    struct type {
        identifier_t id;
        float weight;
        callback_t factory;
    };
    std::vector<type> types;
    
    std::vector<std::vector<identifier_t>> data;

    std::vector<std::vector<identifier_t>> left;
    std::vector<std::vector<identifier_t>> right;
    std::vector<std::vector<identifier_t>> behind;
    
    int width = 0;
    
    int x_offset = 0;
    int y_offset = 0;
    
    int watermark = 0;
};

