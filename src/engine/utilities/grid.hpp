#pragma once

class grid {
public:
    
    void define(int w, int x, int y) {
        width = w;
        x_offset = x;
        y_offset = y;
    }
    
    typedef std::pair<int, int> quadrant;
    
    quadrant getQuadrant(float x, float z) {
        int nx = floor(x + x_offset);
        int nz = floor(z + y_offset);
        
        float tx = (nx % 2 ? nx + 1: nx + 2) / 2;
        float tz = (nz % 2 ? nz + 1: nz + 2) / 2;
        
        return { tx, tz * -1 };
    }
    
    spatial::position getQuadrantPosition(quadrant q, float y=0.0) {
        float nx = (q.first * 2) - 1 - x_offset;
        float nz = (q.second * -2) - 1 - y_offset;
        return spatial::position({ nx, y, nz });
    }
    
    void addToQuadrant(quadrant q, int instance) {
        if(data.size() <= q.second) {
            data.resize(q.second + 1);
        }
        if(data[q.second].size() == 0) {
            data[q.second].resize(width + 1);
        }
        data[q.second][q.first].push_back(instance);
    }
    
    int inQuadrant(quadrant q) {
        if(data.size() > q.second && data[q.second].size() > q.first) {
            return data[q.second][q.first].size();
        }
        return 0;
    }
    
protected:
    std::vector<std::vector<std::vector<int>>> data;

    int width;
    
    int x_offset;
    int y_offset;
};

