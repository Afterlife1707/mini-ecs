#pragma once
#include <cstdint>

class GameObject {
public:
    float x, y;
    float dx, dy;
    float radius;

    GameObject(float x_, float y_, float dx_, float dy_, float radius_)
        : x(x_), y(y_), dx(dx_), dy(dy_), radius(radius_) {}

    virtual ~GameObject() = default;

    virtual void update(float dt) 
    {
        x += dx * dt;
        y += dy * dt;
    }

    virtual void checkWallBounce(int screenW, int screenH) 
    {
        if (x - radius < 0 || x + radius > screenW) dx *= -1;
        if (y - radius < 0 || y + radius > screenH) dy *= -1;
    }
};