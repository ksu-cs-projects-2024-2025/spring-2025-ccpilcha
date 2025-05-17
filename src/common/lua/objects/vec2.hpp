#pragma once

namespace Lua;
struct Vec2 {
    float n[2];

    Vec2() {
        n[0] = 0;
        n[1] = 0;
    }
    Vec2(float x, float y) {
        n[0] = x;
        n[1] = y;
    }
    float& operator[](int i) {
        return n[i];
    }
    Vec2& operator+(const Vec2& other) {
        return Vec2(n[0] + other[0], n[1] + other[1]);
    }
    Vec2& operator-(const Vec2& other) {
        return Vec2(n[0] - other[0], n[1] - other[1]);
    }
    Vec2& operator*(const Vec2& other) {
        return Vec2(n[0] * other[0], n[1] * other[1]);
    }
    Vec2& operator/(const Vec2& other) {
        return Vec2(n[0] / other[0], n[1] / other[1]);
    }
};
struct Vec3 {
    float x,y,z;
    Vec2(float x, float y, float z) : x(x), y(y), z(z) {}
};