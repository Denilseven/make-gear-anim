#pragma once

#include "pose.hxx"
#include <iostream>
#include <string>
#include <vector>

struct Sequence : std::vector<Pose> {
    void addAt(const Pose& pose, int position=-1) {
        if (position == -1)
            push_back(pose);
        else
            insert(begin() + position, pose);
    }

    void setAt(const Pose& pose, int position) {
        (*this)[position] = pose;
    }

    void removeAt(int position) {
        erase(begin() + position);
    }

    Pose& getAt(int position) {
        return (*this)[position];
    }
};
