#pragma once

#include <map>
#include <raylib.h>
#include <string>

struct Pose {
    std::map<std::string, float> rotationMap{};
    std::map<std::string, Vector2> positionMap{};
    std::map<std::string, bool> mirrorMap{};
};
