#include "ecs.h"
#include "components.h"
#include "raylib.h"
#include <random>

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
        if (pos.x - circ->radius < 0 || pos.x + circ->radius > screenW) vel->dx *= -1;
        if (pos.y - circ->radius < 0 || pos.y + circ->radius > screenH) vel->dy *= -1;
    }
}

void renderSystem(World& world) {
    auto& positions = world.storage<Position>().all();
    auto& circles = world.storage<Circle>();
    for (auto& [entity, pos] : positions) 
    {
        if (Circle* circ = circles.get(entity)) 
            DrawCircle((int)pos.x, (int)pos.y, circ->radius, SKYBLUE);
    }
}

int main() {
    const int screenW = 1000, screenH = 700;
    InitWindow(screenW, screenH, "mini-ecs demo");
    SetTargetFPS(60);

    World world;
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> posX(0, screenW);
    std::uniform_real_distribution<float> posY(0, screenH);
    std::uniform_real_distribution<float> vel(-150.f, 150.f);

    const int entityCount = 500;
    for (int i = 0; i < entityCount; ++i) 
    {
        Entity e = world.createEntity();
        world.storage<Position>().add(e, {posX(rng), posY(rng)});
        world.storage<Velocity>().add(e, {vel(rng), vel(rng)});
        world.storage<Circle>().add(e, {4.0f});
    }

    while (!WindowShouldClose()) 
    {
        float dt = GetFrameTime();
        movementSystem(world, dt);
        wallBounceSystem(world, screenW, screenH);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        renderSystem(world);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}