#pragma once

#define EDITOR_MODE_COUNT 2
enum EditorMode : int {
    rotate,
    translate,
};

#define ONION_MODE_COUNT 3
enum OnionMode : int {
    none = 0,
    adjacent = 1,
    all = 2,
};

OnionMode onionMode{OnionMode::all};
EditorMode editorMode{EditorMode::rotate};

const float rotationSpeed{100.0f};
const float translationSpeed{150.0f};

float editorMultiplier{1.0f};

float frameDuration{1.0f/12.0f}; // 12 fps animation
float editorTimer{0.0f};
float gridSpace{0.0f};

bool animationPlaying{false};

// WARNING: many magic numbers!!!
void cycleGridSpace() {
    if (gridSpace == 0.0f)
        gridSpace = 250.0f;
    else if (gridSpace == 250.0f)
        gridSpace = 125.0f;
    else if (gridSpace == 125.0f)
        gridSpace = 62.5f;
    else
        gridSpace = 0.0f;
}

void cycleEditorMode() {
    editorMode = (EditorMode)(((int)editorMode+1) % EDITOR_MODE_COUNT);
}

void cycleOnionMode() {
    onionMode = (OnionMode)(((int)onionMode+1) % ONION_MODE_COUNT);
}
