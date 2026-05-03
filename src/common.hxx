#pragma once

#include <iostream>
#include <raylib.h>
#include <raymath.h>

#define DEFAULT_TEXTURE_FILENAME "texture.png"
#define DEFAULT_PARTS_FILENAME "character.parts"
#define DEFAULT_SEQUENCE_FILENAME "default.sequence"

const char* textureFilename{DEFAULT_TEXTURE_FILENAME};
const char* partsFilename{DEFAULT_PARTS_FILENAME};
const char* sequenceFilename{DEFAULT_SEQUENCE_FILENAME};

#define WIN_WIDTH 1000
#define WIN_HEIGHT 1000

#define TRANSPARENT (Color){255, 255, 255, 0}
#define SPECTRE (Color){255, 0, 0, 100}
#define PHANTOM (Color){0, 255, 0, 100}
#define MELLOWS (Color){0, 0, 255, 100}

Vector2 rotateAround(const Vector2& point, const Vector2& center, float degrees) {
    float radians = degrees * (M_PI / 180);
    Vector2 result = (Vector2){
        result.x = (point.x-center.x)*std::cos(radians) - (point.y-center.y)*std::sin(radians),
        result.y = (point.x-center.x)*std::sin(radians) + (point.y-center.y)*std::cos(radians)
    };
    return result + center;
}

void printHelp() {
    std::cout << "\n";
    std::cout << "(arguments help)\n\n";
    std::cout << "-t filename.png" << "\n\tsets the texture\n";
    std::cout << "\t(default is `" << DEFAULT_TEXTURE_FILENAME <<"`)\n";
    std::cout << "-p filename.parts" << "\n\tsets the parts data file (basically the \"model\")\n";
    std::cout << "\t(default is `" << DEFAULT_PARTS_FILENAME <<"`)\n";
    std::cout << "-s filename.sequence" << "\n\tsets the sequence file (basically the \"animation\")\n";
    std::cout << "\t(default is `" << DEFAULT_SEQUENCE_FILENAME <<"`)\n";
    std::cout << std::endl;
}
