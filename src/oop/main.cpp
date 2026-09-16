#include "gameobject.h"
#include "raylib.h"
#include <chrono>
#include <random>

void collisionSystemOOP(std::vector<GameObject*>& objects) 
{
    for (size_t i = 0; i < objects.size(); ++i) 
    {
        for (size_t j = i + 1; j < objects.size(); ++j) 
        {
            GameObject* a = objects[i];
            GameObject* b = objects[j];

            float dx = b->x - a->x;
            float dy = b->y - a->y;
            float distSq = dx * dx + dy * dy;
            float minDist = a->radius + b->radius;

            if (distSq < minDist * minDist && distSq > 0.0001f) 
            {
                float dist = std::sqrt(distSq);
                float overlap = minDist - dist;
                float nx = dx / dist;
                float ny = dy / dist;

                a->x -= nx * overlap * 0.5f;
                a->y -= ny * overlap * 0.5f;
                b->x += nx * overlap * 0.5f;
                b->y += ny * overlap * 0.5f;

                std::swap(a->dx, b->dx);
                std::swap(a->dy, b->dy);
            }
        }
    }
}

int main() {
    const int screenW = 1000, screenH = 700;
    InitWindow(screenW, screenH, "oop demo");
    SetTargetFPS(0);

    std::vector<GameObject*> objects;
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> posX(0, screenW);
    std::uniform_real_distribution<float> posY(0, screenH);
    std::uniform_real_distribution<float> vel(-150.f, 150.f);

    const int entityCount = 10000; 
    for (int i = 0; i < entityCount; ++i)
     {
        objects.push_back(new GameObject(posX(rng), posY(rng), vel(rng), vel(rng), 4.0f));
    }

    // timing setup
    double totalUpdateMs = 0.0;
    int frameCount = 0;
    const int framesToMeasure = 300;

    while (!WindowShouldClose() && frameCount < framesToMeasure) 
    {
        float dt = GetFrameTime();

        auto t0 = std::chrono::high_resolution_clock::now();

        for (auto* obj : objects) 
        {
            obj->update(dt);
            obj->checkWallBounce(screenW, screenH);
        }
        collisionSystemOOP(objects);

        auto t1 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        totalUpdateMs += ms;
        frameCount++;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (auto* obj : objects) 
        {
            DrawCircle((int)obj->x, (int)obj->y, obj->radius, SKYBLUE);
        }
        DrawFPS(10, 10);
        EndDrawing();
    }

    double avgMs = totalUpdateMs / frameCount;
    printf("\n=== OOP Baseline Results ===\n");
    printf("Entity count: %d\n", entityCount);
    printf("Frames measured: %d\n", frameCount);
    printf("Average update time: %.4f ms\n", avgMs);
    printf("============================\n");

    for (auto* obj : objects) delete obj;
    CloseWindow();
    return 0;
}