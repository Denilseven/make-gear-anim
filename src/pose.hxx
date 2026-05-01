#pragma once

#include "common.hxx"
#include "part.hxx"
#include <map>

struct Pose {
    std::map<std::string, float> rotationMap{};
    std::map<std::string, Vector2> positionMap{};

    void readPose(std::vector<Part>& list) {
        for (Part& part : list) {
            if (part.parent == nullptr)
                positionMap[part.name] = part.position;
            rotationMap[part.name] = part.localRotation;
        }
    }

    void setPose(std::vector<Part>& list) {
        for (Part& part : list) {
            if (positionMap.find(part.name) != positionMap.end())
                part.position = positionMap[part.name];
            if (rotationMap.find(part.name) != rotationMap.end())
                part.localRotation = rotationMap[part.name];
        }
    }
};
