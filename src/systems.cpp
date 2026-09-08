#include "systems.h"
#include "components.h"
#include "raylib.h"
#include <cmath>
#include <vector>

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

void collisionSystem(World& world) 
{
    auto& positions = world.storage<Position>().all();
    auto& circles = world.storage<Circle>();
    auto& velocities = world.storage<Velocity>();

    std::vector<Entity> collidable;
    for (auto& [entity, pos] : positions) 
        if (circles.get(entity)) collidable.push_back(entity);

    for (size_t i = 0; i < collidable.size(); ++i) 
    {
        for (size_t j = i + 1; j < collidable.size(); ++j) 
        {
            Entity a = collidable[i];
            Entity b = collidable[j];

            Position& posA = positions[a];
            Position& posB = positions[b];
            Circle* circA = circles.get(a);
            Circle* circB = circles.get(b);

            float dx = posB.x - posA.x;
            float dy = posB.y - posA.y;
            float distSq = dx * dx + dy * dy;
            float minDist = circA->radius + circB->radius;

            if (distSq < minDist * minDist && distSq > 0.0001f) 
            {
                float dist = std::sqrt(distSq);
                float overlap = minDist - dist;
                float nx = dx / dist;
                float ny = dy / dist;

                posA.x -= nx * overlap * 0.5f;
                posA.y -= ny * overlap * 0.5f;
                posB.x += nx * overlap * 0.5f;
                posB.y += ny * overlap * 0.5f;

                Velocity* velA = velocities.get(a);
                Velocity* velB = velocities.get(b);
                if (velA && velB) 
                {
                    std::swap(velA->dx, velB->dx);
                    std::swap(velA->dy, velB->dy);
                }
            }
        }
    }
}