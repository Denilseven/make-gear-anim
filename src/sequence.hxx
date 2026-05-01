#pragma once

#include "pose.hxx"
#include <iostream>
#include <string>
#include <vector>

struct Sequence {
    std::vector<Pose> poses{};

    void addAt(const Pose& pose, int position=-1) {
        if (position == -1)
            poses.push_back(pose);
        else
            poses.insert(poses.begin() + position, pose);
    }

    void setAt(const Pose& pose, int position) {
        poses[position] = pose;
    }

    void removeAt(int position) {
        poses.erase(poses.begin() + position);
    }

    Pose& getAt(int position) {
        return poses[position];
    }

    int size() {
        return poses.size();
    }
};
