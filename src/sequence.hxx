#pragma once

#include "pose.hxx"
#include <iostream>
#include <string>
#include <vector>

struct Sequence {
    std::vector<Pose> poses{};

    void add(const Pose& pose, int position=-1) {
        if (position == -1)
            poses.push_back(pose);
        else
            poses.insert(poses.begin() + position, pose);
    }

    void set(const Pose& pose, int position) {
        poses[position] = pose;
    }
};
