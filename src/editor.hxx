#pragma once

#include <raylib.h>

enum class EditorMode : int {
    rotate,
    translate,
    playing,
};
#define DEFAULT_EDITOR_MODE EditorMode::rotate

enum class OnionMode : int {
    none = 0,
    adjacent = 1,
    all = 2,
};
#define ONION_MODE_COUNT 3

enum class GridMode : int {
    none = 0,
    grid = 1,
    rect = 2,
};

OnionMode onionMode{OnionMode::all};
EditorMode editorMode{EditorMode::rotate};
GridMode gridMode{GridMode::none};

const float rotationSpeed{100.0f};
const float translationSpeed{150.0f};

float editorMultiplier{1.0f};

float frameDuration{1.0f/12.0f}; // 12 fps animation
float editorTimer{0.0f};
float gridSpace{0.0f};
Vector2 referenceRectangle{0.0f, 0.0f};

// WARNING: many magic numbers!!!
void cycleGridSpace() {
    switch (gridMode) {
    case GridMode::none:
        gridMode = GridMode::grid;
        gridSpace = 250.0f;
        break;

    case GridMode::grid:
        if (gridSpace == 0.0f)
            gridSpace = 250.0f;
        else if (gridSpace == 250.0f)
            gridSpace = 125.0f;
        else if (gridSpace == 125.0f)
            gridSpace = 62.5f;
        else
            gridMode = referenceRectangle.y != 0.0f ? GridMode::rect : GridMode::none;
        break;
    
    case GridMode::rect:
        gridMode = GridMode::none;
        break;
    }
}

void cycleEditorMode() {
    if (editorMode == EditorMode::rotate)
        editorMode = EditorMode::translate;
    else if (editorMode == EditorMode::translate)
        editorMode = EditorMode::rotate;
    else
        editorMode = DEFAULT_EDITOR_MODE;
}

void cycleOnionMode() {
    onionMode = (OnionMode)(((int)onionMode+1) % ONION_MODE_COUNT);
}
