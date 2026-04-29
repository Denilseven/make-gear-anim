#pragma once

#include "common.hxx"
#include "part.hxx"
#include <fstream>
#include <iostream>
#include <vector>

#define TEST_POSE_FILE "pose.txt"

void writePoseToFile(std::vector<Part>& list) {
    std::ofstream file(TEST_POSE_FILE);

    for (Part& part : list) {
        if (part.parent == nullptr) // We only need to save position data if the part is root, else it's 0,0 anyways
            file << "#" << std::endl << part.position.x << std::endl << part.position.y << std::endl;
        file << part.localRotation << std::endl;
    }
    file.close();
    std::cout << "Success: Wrote pose to file." << std::endl;
}

bool setPoseFromFile(std::vector<Part>& list) {
    std::ifstream file(TEST_POSE_FILE);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `" << TEST_POSE_FILE << "`!" << std::endl;
        std::exit(1);
    }

    int currentPart{0};
    std::string s{};
    while (getline(file, s)) {
        try {
            Part& part = list[currentPart];
            if (s == "#") {
                // Special case; the two lines after a `#` specify
                // the position for this part, read it and move on
                getline(file, s);
                part.position.x = std::stoi(s);
                getline(file, s);
                part.position.y = std::stoi(s);
                getline(file, s);
            }
            // Read each line as the rotation for the current part
            part.localRotation = std::stof(s);
            // Success, onto the next
            currentPart++;
        }
        catch (...) {
            std::cout << "Error: Failed to read pose from file; the file may not be formatted correctly or it's for a different \"character\"." << std::endl;
            return false;
        }
    }
    file.close();
    std::cout << "Success: Set pose from file." << std::endl;
    return true;
}
