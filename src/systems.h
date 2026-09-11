#pragma once

#include "ecs.h"

void movementSystem(World& world, float dt);
void wallBounceSystem(World& world, int screenW, int screenH);
void renderSystem(World& world);
void collisionSystem(World& world, class SpatialHashGrid& grid);