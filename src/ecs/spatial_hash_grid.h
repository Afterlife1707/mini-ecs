#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cmath>

using Entity = uint32_t;

class SpatialHashGrid {
public:
    explicit SpatialHashGrid(float cellSize) : cellSize_(cellSize) {}

    void clear() 
    {
        cells_.clear();
    }

    void insert(Entity e, float x, float y) 
    {
        int64_t key = cellKey(x, y);
        cells_[key].push_back(e);
    }

    void queryNearby(float x, float y, std::vector<Entity>& out) const 
    {
        int cx = static_cast<int>(std::floor(x / cellSize_));
        int cy = static_cast<int>(std::floor(y / cellSize_));

        for (int dx = -1; dx <= 1; ++dx) 
        {
            for (int dy = -1; dy <= 1; ++dy) 
            {
                int64_t key = packKey(cx + dx, cy + dy);
                auto it = cells_.find(key);
                if (it != cells_.end()) 
                {
                    out.insert(out.end(), it->second.begin(), it->second.end());
                }
            }
        }
    }

private:
    int64_t cellKey(float x, float y) const 
    {
        int cx = static_cast<int>(std::floor(x / cellSize_));
        int cy = static_cast<int>(std::floor(y / cellSize_));
        return packKey(cx, cy);
    }

    static int64_t packKey(int cx, int cy) 
    {
        return (static_cast<int64_t>(cx) << 32) | (static_cast<uint32_t>(cy));
    }

    float cellSize_;
    std::unordered_map<int64_t, std::vector<Entity>> cells_;
};