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
};

Part* getPartByName(std::vector<Part>& list, const std::string name) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i].name == name) {
            return &list[i];
        }
    }
    return nullptr;
}

bool readPartsListFromFile(std::vector<Part>& list, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `" << PARTS_DESCRIPTION_FILE << "`!" << std::endl;
        std::exit(1);
    }

    list.clear();

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
            list.push_back(newPart);
        }
    }
    for (int i = 0; i < list.size(); i++) {
        list[i].parent = getPartByName(list, parents[i]);
        // We set the parent pointers after the list is complete because setting
        // them while it is being built and resized seems to mess up everything.
        if (list[i].parent == nullptr) {
            list[i].position.x = WIN_WIDTH / 2;
            list[i].position.y = WIN_HEIGHT / 2;
        }
    }
    file.close();
    return true;
}
