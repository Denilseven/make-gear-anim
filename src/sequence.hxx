#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Sequence {
    std::vector<std::string> poses{};

    void addPose(const std::string& pose, int position=-1) {
        if (position == -1)
            poses.push_back(pose);
        else
            poses.insert(poses.begin() + position, pose);
    }

    void setPose(const std::string& pose, int position) {
        poses[position] = pose;
    }
};
