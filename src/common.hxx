#pragma once

#include <raylib.h>
#include <raymath.h>

const char* textureFilename{"texture.png"};
const char* partsFilename{"character.parts"};
const char* sequenceFilename{"default.sequence"};

#define WIN_WIDTH 1000
#define WIN_HEIGHT 1000

#define TRANSPARENT (Color){255, 255, 255, 0}
#define SPECTRE (Color){255, 0, 0, 100}
#define PHANTOM (Color){0, 255, 0, 100}
#define MELLOWS (Color){0, 0, 255, 100}

Vector2 rotateAround(const Vector2& point, const Vector2& center, float degrees) {
    float radians = degrees * (M_PI / 180);
    Vector2 result = (Vector2){
        result.x = (point.x-center.x)*std::cos(radians) - (point.y-center.y)*std::sin(radians),
        result.y = (point.x-center.x)*std::sin(radians) + (point.y-center.y)*std::cos(radians)
    };
    return result + center;
}
