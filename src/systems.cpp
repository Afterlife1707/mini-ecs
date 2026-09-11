#include "systems.h"
#include "components.h"
#include "raylib.h"
#include "spatial_hash_grid.h"
#include <cmath>
#include <vector>

void movementSystem(World& world, float dt) 
{
    auto& positions = world.storage<Position>();
    auto& velocities = world.storage<Velocity>();

    auto& posArray = positions.denseArray();
    auto& posEntities = positions.entities();
    
    for (size_t i = 0; i< posArray.size(); i++) 
    {
        Entity e = posEntities[i];
        if (Velocity* vel = velocities.get(e)) 
        {
            posArray[i].x += vel->dx * dt;
            posArray[i].y += vel->dy * dt;
        }
    }
}

void wallBounceSystem(World& world, int screenW, int screenH) 
{
    auto& positions = world.storage<Position>();
    auto& velocities = world.storage<Velocity>();
    auto& circles = world.storage<Circle>();

    auto& posArray = positions.denseArray();
    auto& posEntities = positions.entities();

    for (size_t i = 0; i<posArray.size();i++) 
    {
        Entity e = posEntities[i];
        Velocity* vel = velocities.get(e);
        Circle* circ = circles.get(e);

        if (!vel || !circ) continue;

        if (posArray[i].x - circ->radius < 0 || 
            posArray[i].x + circ->radius > screenW)
            vel->dx *= -1;

        if (posArray[i].y - circ->radius < 0 || 
            posArray[i].y + circ->radius > screenH)
            vel->dy *= -1;
    }
}

void renderSystem(World& world) 
{
    auto& positions = world.storage<Position>();
    auto& circles = world.storage<Circle>();

    auto& posArray = positions.denseArray();
    auto& posEntities = positions.entities();

    for (size_t i = 0; i<posArray.size();i++) 
    {
        Entity e = posEntities[i];
        if (Circle* circ = circles.get(e))
            DrawCircle((int)posArray[i].x, (int)posArray[i].y, circ->radius, SKYBLUE);
    }
}

void collisionSystem(World& world, SpatialHashGrid& grid) 
{
    auto& positions = world.storage<Position>();
    auto& circles = world.storage<Circle>();
    auto& velocities = world.storage<Velocity>();

    auto& posArray = positions.denseArray();
    auto& posEntities = positions.entities();

    // Rebuild the grid fresh each frame — entities moved since last frame
    grid.clear();
    for (size_t i = 0; i < posArray.size(); ++i) 
    {
        Entity e = posEntities[i];
        if (circles.get(e)) 
        {
            grid.insert(e, posArray[i].x, posArray[i].y);
        }
    }

    // Check each entity only against nearby entities, not all of them
    std::vector<Entity> nearby;
    for (size_t i = 0; i < posArray.size(); ++i) 
    {
        Entity a = posEntities[i];
        Circle* circA = circles.get(a);
        if (!circA) continue;

        Position* posA = positions.get(a);

        nearby.clear();
        grid.queryNearby(posA->x, posA->y, nearby);

        for (Entity b : nearby)
        {
            if (b <= a) continue;  // avoid double-checking pairs and self-checks

            Position* posB = positions.get(b);
            Circle* circB = circles.get(b);
            if (!posB || !circB) continue;

            float dx = posB->x - posA->x;
            float dy = posB->y - posA->y;
            float distSq = dx * dx + dy * dy;
            float minDist = circA->radius + circB->radius;

            if (distSq < minDist * minDist && distSq > 0.0001f) 
            {
                float dist = std::sqrt(distSq);
                float overlap = minDist - dist;
                float nx = dx / dist;
                float ny = dy / dist;

                posA->x -= nx * overlap * 0.5f;
                posA->y -= ny * overlap * 0.5f;
                posB->x += nx * overlap * 0.5f;
                posB->y += ny * overlap * 0.5f;

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