#pragma once

#include "common.hxx"
#include "part.hxx"
#include "pose.hxx"

struct Figure {
    std::vector<Part> parts;

    int size() { return parts.size(); }

    void update() {
        for (int i = 0; i < parts.size(); i++) {
            Part& part = parts[i];
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
        for (Part& part : parts)
            part.draw(texture, color);
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

    bool readFromFile(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open `" << PARTS_DESCRIPTION_FILE << "`!" << std::endl;
            std::exit(1);
        }

        parts.clear();

        std::vector<std::string> parents{};
        std::string s{};
        while (getline(file, s)) {
            if (s == "+") {
                Part newPart{};

                try {
                    getline(file, s); newPart.name = s;
                    getline(file, s); newPart.bounds.x = std::stoi(s);
                    getline(file, s); newPart.bounds.y = std::stoi(s);
                    getline(file, s); newPart.bounds.width = std::stoi(s);
                    getline(file, s); newPart.bounds.height = std::stoi(s);
                    getline(file, s); newPart.pivot.x = std::stoi(s);
                    getline(file, s); newPart.pivot.y = std::stoi(s);
                    getline(file, s); parents.push_back(s);
                    getline(file, s); newPart.connectedNotch = std::stoi(s);
                
                    getline(file, s);
                    while (s == ".") {
                        getline(file, s); int x = std::stoi(s);
                        getline(file, s); int y = std::stoi(s);
                        newPart.localNotches.push_back((Vector2){(float)x, (float)y});
                        newPart.worldNotches.push_back((Vector2){(float)x, (float)y});
                        getline(file, s);
                    }
                }
                catch (...) {
                    return false;
                }
                parts.push_back(newPart);
            }
        }
        for (int i = 0; i < parts.size(); i++) {
            parts[i].parent = getPartByName(parts, parents[i]);
            // We set the parent pointers after the list is complete because setting
            // them while it is being built and resized seems to mess up everything.
            if (parts[i].parent == nullptr) {
                parts[i].position.x = WIN_WIDTH / 2;
                parts[i].position.y = WIN_HEIGHT / 2;
            }
        }
        file.close();
        return true;
    }
};
