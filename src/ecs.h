#pragma once
#include <cstdint>
#include <unordered_map>
#include "sparse_set.h"

using Entity = uint32_t;   // an entity IS just a number, nothing more

template <typename T>
class ComponentStorage {
public:
    void add(Entity e, T component) { data_[e] = component; }
    void remove(Entity e) { data_.erase(e); }
    T* get(Entity e) 
    {
        auto it = data_.find(e);
        return it != data_.end() ? &it->second : nullptr;
    }
    std::unordered_map<Entity, T>& all() { return data_; }

private:
    std::unordered_map<Entity, T> data_;
};

class World {
public:
    Entity createEntity() { return nextId_++; }

    template <typename T>
    SparseSet<T>& storage() 
    {
        static SparseSet<T> instance;  // one storage per component type
        return instance;
    }

private:
    Entity nextId_ = 0;
};