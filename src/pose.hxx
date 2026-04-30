#pragma once

#include "common.hxx"
#include "part.hxx"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define TEST_POSE_FILE "pose.txt"

std::string poseToString(std::vector<Part>& list) {
    std::string result{};
    // result.append(">>>POSE\n");
    for (Part& part : list) {
        if (part.parent == nullptr) {
            // We only need to add position data if the part is root, else it's 0,0 anyways
            result.append("#\n");
            result.append(std::to_string(part.position.x));
            result.append("\n");
            result.append(std::to_string(part.position.y));
            result.append("\n");
        }
        // Add rotation data
        result.append(std::to_string(part.localRotation));
        result.append("\n");
    }
    // result.append(">>>\n");
    return result;
}

bool setPoseFromString(std::vector<Part>& list, std::string string) {
    std::istringstream stream{string}; // Is this what they call Modern C++?
    int currentPart{0};
    for (std::string line; std::getline(stream, line); ) {
         try {
            Part& part = list[currentPart];
            if (line == "#") {
                // Special case; the two lines after a `#` specify
                // the position for this part, read it and move on
                getline(stream, line);
                part.position.x = std::stoi(line);
                getline(stream, line);
                part.position.y = std::stoi(line);
                getline(stream, line);
            }
            // Read each line as the rotation for the current part
            part.localRotation = std::stof(line);
            // Success, onto the next
            currentPart++;
       }
        catch (...) {
            std::cout << "Error: Failed to read pose from string; it may not be formatted correctly or it's for a different \"character\"." << std::endl;
            return false;
        }
    }
    std::cout << "Success: Set pose from string." << std::endl;
    return true;
}

void writePoseToFile(std::vector<Part>& list) {
    std::ofstream file(TEST_POSE_FILE);
    file << poseToString(list);
    file.close();
    std::cout << "Success: Wrote pose to file." << std::endl;
}

bool setPoseFromFile(std::vector<Part>& list) {
    std::ifstream file(TEST_POSE_FILE);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `" << TEST_POSE_FILE << "`!" << std::endl;
        std::exit(1);
    }

    std::string contents{};
    std::string s{};
    // Read file into a string
    while (getline(file, s)) {
        contents.append(s);
        contents.append("\n");
    }
    setPoseFromString(list, contents);

    file.close();
    std::cout << "Success: Set pose from file." << std::endl;
    return true;
}

void randomizePose(std::vector<Part>& list) {
    for (Part& part : list)
        part.localRotation = GetRandomValue(0, 360);
}
