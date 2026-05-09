#pragma once

#include "common.hxx"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>

struct Part {
    std::string name{};
    Rectangle bounds{};
    Vector2 pivot{};

    Part* parent{nullptr};
    int connectedNotch{-1};

    Vector2 position{};
    float localRotation{0.0f};
    float worldRotation{0.0f};
    std::vector<Vector2> localNotches{};
    std::vector<Vector2> worldNotches{};
    bool mirror{false};

    void draw(Texture texture, Color color) {
        DrawTexturePro(
            texture,
            (Rectangle){
                bounds.x,
                bounds.y,
                bounds.width * (mirror ? -1.0f : 1.0f),
                bounds.height
            },
            (Rectangle){position.x, position.y, bounds.width, bounds.height},
            (Vector2){pivot.x, pivot.y},
            worldRotation,
            color
        );
    }
};
