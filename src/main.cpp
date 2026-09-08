#include "ecs.h"
#include "components.h"
#include "systems.h"
#include "raylib.h"
#include <random>

int main() 
{
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
        collisionSystem(world);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        renderSystem(world);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}