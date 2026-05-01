#pragma once

#include "common.hxx"
#include "part.hxx"
#include <map>

struct Pose {
    std::map<std::string, float> rotationMap{};
    std::map<std::string, Vector2> positionMap{};
};
