#pragma once

#include "sequence.hxx"
#include <fstream>
#include <sstream>
#include <string>

struct PersistenceManager {
    static bool save(Sequence& sequence, std::string filename) {
        std::stringstream stream{};

        for (int i = 0; i < sequence.size(); i++) {
            Pose& pose = sequence.poses[i];
            stream << "POSE\n" << i << "\n";
            for (const auto& pair : pose.positionMap) {
                stream << "POSITION\n" << pair.first << "\n";
                stream << pair.second.x << "\n" << pair.second.y << "\n";
            }
            for (const auto& pair : pose.rotationMap) {
                stream << pair.first << "\n" << pair.second << "\n";
            }
            stream << "!\n";
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to stream `" << filename << "` to save sequence!" << std::endl;
            return false;
        }
        file << stream.rdbuf();

        return true;
    }

    static bool setSequenceFromFile(Sequence& sequence, std::string filename) {
        sequence.poses.clear();

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open `" << filename << "` to read pose!" << std::endl;
            return false;
        }

        Pose newPose{};
        std::string name{};

        std::string s{};
        while (getline(file, s)) {
            if (s == "POSE") {
                getline(file, s);
                newPose = {};
            }
            else if (s == "!") {
                sequence.addAt(newPose);
            }
            else if (s == "POSITION") {
                try {
                    getline(file, s); name = s;
                    getline(file, s); newPose.positionMap[name].x = std::stof(s);
                    getline(file, s); newPose.positionMap[name].y = std::stof(s);
                }
                catch (...) { return false; }
            }
            else {
                name = s;
                try {
                    getline(file, s); newPose.rotationMap[name] = std::stof(s);
                }
                catch (...) { return false; }
            }
        }
        return true;
    }
};
