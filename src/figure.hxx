#pragma once

#include "common.hxx"
#include "part.hxx"
#include "pose.hxx"

struct Figure : std::vector<Part> {
    void update() {
        for (int i = 0; i < size(); i++) {
            Part& part = (*this)[i];
            // Set world rotation and position for each part
            if (part.parent != nullptr) {
                part.worldRotation = part.localRotation + part.parent->worldRotation;
                if (part.connectedNotch == -1) part.position = part.parent->position;
                else part.position = part.parent->worldNotches[part.connectedNotch];
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

    void draw(Texture texture, Color color) {
        for (Part& part : (*this))
            part.draw(texture, color);
    }

    Pose getPose() {
        Pose result{};
        for (Part& part : (*this)) {
            if (part.parent == nullptr)
                result.positionMap[part.name] = part.position;
            result.rotationMap[part.name] = part.localRotation;
        }
        return result;
    }

    void setPose(Pose& newPose) {
        for (Part& part : (*this)) {
            if (newPose.positionMap.find(part.name) != newPose.positionMap.end())
                part.position = newPose.positionMap[part.name];
            if (newPose.rotationMap.find(part.name) != newPose.rotationMap.end())
                part.localRotation = newPose.rotationMap[part.name];
        }
    }

    Part* getPartByName(const std::string name) {
        for (int i = 0; i < size(); i++)
            if ((*this)[i].name == name)
                return &(*this)[i];
        return nullptr;
    }
};
