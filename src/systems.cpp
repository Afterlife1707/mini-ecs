#include "systems.h"
#include "components.h"
#include "raylib.h"

void movementSystem(World& world, float dt) 
{
    auto& positions = world.storage<Position>().all();
    auto& velocities = world.storage<Velocity>();

    for (auto& [entity, pos] : positions) 
    {
        if (Velocity* vel = velocities.get(entity)) 
        {
            pos.x += vel->dx * dt;
            pos.y += vel->dy * dt;
        }
    }
}

void wallBounceSystem(World& world, int screenW, int screenH) 
{
    auto& positions = world.storage<Position>().all();
    auto& velocities = world.storage<Velocity>();
    auto& circles = world.storage<Circle>();

    for (auto& [entity, pos] : positions) 
    {
        Velocity* vel = velocities.get(entity);
        Circle* circ = circles.get(entity);

        if (!vel || !circ) continue;

        if (pos.x - circ->radius < 0 || 
            pos.x + circ->radius > screenW)
            vel->dx *= -1;

        if (pos.y - circ->radius < 0 || 
            pos.y + circ->radius > screenH)
            vel->dy *= -1;
    }
}

void renderSystem(World& world) 
{
    auto& positions = world.storage<Position>().all();
    auto& circles = world.storage<Circle>();

    for (auto& [entity, pos] : positions) 
    {
        if (Circle* circ = circles.get(entity))
            DrawCircle((int)pos.x, (int)pos.y, circ->radius, SKYBLUE);
    }
}