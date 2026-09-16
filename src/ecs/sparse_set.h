#pragma once
#include <cstdint>
#include <vector>
#include <cassert>

using Entity = uint32_t;
constexpr Entity NULL_INDEX = UINT32_MAX;

template <typename T>
class SparseSet{
public:
    void add(Entity e, T component)
    {
        if(has(e))
        {
            dense[sparse[e]] = component;
            return;
        }
        if(e >= sparse.size())
            sparse.resize(e + 1, NULL_INDEX);
        
        sparse[e] = static_cast<Entity>(dense.size());
        dense.push_back(component);
        denseToEntity.push_back(e);
    }

    void remove(Entity e)
    {
        if(!has(e)) return;

        uint32_t indexToRemove = sparse[e];
        uint32_t lastIndex = static_cast<uint32_t>(dense.size() - 1);
        Entity lastEntity = denseToEntity[lastIndex];

        dense[indexToRemove] = dense[lastIndex];
        denseToEntity[indexToRemove] = lastEntity;
        sparse[lastEntity] = indexToRemove;

        dense.pop_back();
        denseToEntity.pop_back();
        sparse[e] = NULL_INDEX;
    }
    
    bool has(Entity e) const
    {
        return e < sparse.size() && sparse[e] != NULL_INDEX;
    }

    T* get(Entity e)
    {
        if(!has(e)) return nullptr;
        return &dense[sparse[e]];
    }

    std::vector<T>& denseArray() { return dense; }
    std::vector<Entity>& entities() { return denseToEntity; }

private:
    std::vector<Entity> sparse;
    std::vector<T> dense;
    std::vector<Entity> denseToEntity;
};