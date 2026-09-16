#pragma once
#include <cstdint>
#include <unordered_map>
#include "sparse_set.h"

using Entity = uint32_t;   // an entity IS just a number, nothing more

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