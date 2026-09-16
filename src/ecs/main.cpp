#include "ecs.h"
#include "components.h"
#include "systems.h"
#include "raylib.h"
#include "spatial_hash_grid.h"
#include <random>
#include <chrono>
#include <cstdio>

enum class CollisionMode { Naive, SpatialGrid };

int main() 
{
    const int screenW = 1000, screenH = 700;
    InitWindow(screenW, screenH, "mini-ecs demo");
    SetTargetFPS(0);

    const CollisionMode mode = CollisionMode::Naive; // change this to switch which collision system runs

    World world;
    SpatialHashGrid grid(50.0f);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> posX(0, screenW);
    std::uniform_real_distribution<float> posY(0, screenH);
    std::uniform_real_distribution<float> vel(-150.f, 150.f);

    const int entityCount = 10000;
    for (int i = 0; i < entityCount; ++i) 
    {
        Entity e = world.createEntity();
        world.storage<Position>().add(e, {posX(rng), posY(rng)});
        world.storage<Velocity>().add(e, {vel(rng), vel(rng)});
        world.storage<Circle>().add(e, {4.0f});
    }

    //timing setup
    double totalUpdateMs = 0.0;
    int frameCount = 0;
    const int framesToMeasure = 300;

    while (!WindowShouldClose() && frameCount < framesToMeasure) 
    {
        float dt = GetFrameTime();

        auto t0 = std::chrono::high_resolution_clock::now();

        movementSystem(world, dt);
        wallBounceSystem(world, screenW, screenH);

        if (mode == CollisionMode::Naive)
            collisionSystem(world);
        else
            collisionSystemSpatialGrid(world, grid);

        auto t1 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        totalUpdateMs += ms;
        frameCount++;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        renderSystem(world);
        DrawFPS(10, 10);
        EndDrawing();
    }

    double avgMs = totalUpdateMs / frameCount;
    printf("\n=== ECS Results ===\n");
    printf("Collision mode: %s\n", mode == CollisionMode::Naive ? "Naive O(n^2)" : "Spatial Grid");
    printf("Entity count: %d\n", entityCount);
    printf("Frames measured: %d\n", frameCount);
    printf("Average update time: %.4f ms\n", avgMs);
    printf("====================\n");

    CloseWindow();
    return 0;
}