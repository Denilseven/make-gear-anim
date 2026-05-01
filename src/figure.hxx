#pragma once

#include "common.hxx"
#include "part.hxx"
#include "pose.hxx"

struct Figure {
    std::vector<Part> parts;

    void update() {
        for (int i = 0; i < parts.size(); i++) {
            Part& part = parts[i];
            // Set world position for each part
            if (part.parent != nullptr) {
                if (part.connectedNotch == -1)
                    part.position = part.parent->position;
                else
                    part.position = part.parent->worldNotches[part.connectedNotch];
                part.worldRotation = part.localRotation + part.parent->worldRotation;
            }
            else {
                part.worldRotation = part.localRotation;
            }
            // Set world position for each notch
            for (int i = 0; i < part.localNotches.size(); i++) {
                part.worldNotches[i] = rotateAround(
                    part.localNotches[i] + part.position - part.pivot,
                    part.position,
                    part.worldRotation
                );
            }
        }
    }

    Pose getPose() {
        Pose result{};
        for (Part& part : parts) {
            if (part.parent == nullptr)
                result.positionMap[part.name] = part.position;
            result.rotationMap[part.name] = part.localRotation;
        }
        return result;
    }

    void setPose(Pose& newPose) {
        for (Part& part : parts) {
            if (newPose.positionMap.find(part.name) != newPose.positionMap.end())
                part.position = newPose.positionMap[part.name];
            if (newPose.rotationMap.find(part.name) != newPose.rotationMap.end())
                part.localRotation = newPose.rotationMap[part.name];
        }
    }

    int size() { return parts.size(); }
};
