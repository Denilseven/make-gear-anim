#include "common.hxx"
#include "part.hxx"
#include "pose.hxx"
#include "sequence.hxx"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>

int main() {
    Texture2D texture = LoadTexture(TEXTURE_TO_LOAD);

    float editorTimer{0.0f};
    float frameDuration{1.0f/12.0f}; // 12 fps animation

    std::vector<Part> parts{};
    std::vector<Color> debugColors = {GOLD, PINK, GREEN, SKYBLUE, PURPLE};

    InitWindow(texture.width, texture.height, "FIRST_WINDOW");
    SetTargetFPS(60);

    texture = LoadTexture(TEXTURE_TO_LOAD);
    bool partsAreValid = readPartsListFromFile(parts, PARTS_DESCRIPTION_FILE);

    while (!WindowShouldClose() && !IsKeyDown(KEY_ENTER)) {
        SetMouseCursor(3);
        editorTimer += GetFrameTime();
        if (editorTimer >= 0.1f) {
            editorTimer = 0.0f;
            partsAreValid = readPartsListFromFile(parts, PARTS_DESCRIPTION_FILE);
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        Vector2 mousePos = GetMousePosition();
        DrawLine(mousePos.x, 0, mousePos.x, texture.height, GRAY);
        DrawLine(0, mousePos.y, texture.width, mousePos.y, GRAY);
        DrawText(TextFormat("%.f\n%.f", mousePos.x, mousePos.y), 10, 10, 20, LIGHTGRAY);

        DrawTexture(texture, 0, 0, BLACK);
        for (int i = 0; i < parts.size(); i++) {
            Part& part = parts[i];
            Color color = debugColors[i%debugColors.size()];

            DrawRectangleLinesEx(part.bounds, 1, color);

            DrawPolyLines(part.pivot + (Vector2){part.bounds.x, part.bounds.y}, 5, 20, GetTime()*100+(27*i), color);
            DrawCircleV(part.pivot + (Vector2){part.bounds.x, part.bounds.y}, 2, color);

            for (int j = 0; j < part.localNotches.size(); j++) {
                Vector2& notch = part.localNotches[j];
                DrawCircleLinesV(
                    notch + (Vector2){part.bounds.x, part.bounds.y},
                    14+(j*2), color);
                DrawCircleV(
                    notch + (Vector2){part.bounds.x, part.bounds.y},
                    2, color
                );
            }
        }
        if (!partsAreValid)
            DrawText("Parts description file is invalid!", 10, 50, 20, RED);
        DrawText("Enter (continue)", 10, texture.height - 30, 20, LIGHTGRAY);
        EndDrawing();
    }
    if (!partsAreValid || WindowShouldClose()) {
        UnloadTexture(texture);
        CloseWindow();
        return 0;
    }
    CloseWindow();

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "SECOND_WINDOW");
    SetTargetFPS(60);

    texture = LoadTexture(TEXTURE_TO_LOAD);
    int selectedPart{0};
    int currentPose{0};
    bool animationPlaying{false};

    float dt{0.0f};
    float editorMultiplier{1.0f};
    const float rotationSpeed{100.0f};

    Sequence seq{};
    Pose pose{};
    pose.readPose(parts);
    seq.add(pose);

    while(!WindowShouldClose()) {
        dt = GetFrameTime();
        editorMultiplier = IsKeyDown(KEY_LEFT_SHIFT) ? 0.1f : 1.0f;

        // DEBUG
        if (IsKeyPressed(KEY_ONE)) {
            pose.readPose(parts);
            seq.add(pose);
        }
        if (IsKeyPressed(KEY_TWO)) {
            pose.readPose(parts);
            seq.add(pose, currentPose);
        }
        if (IsKeyPressed(KEY_THREE)) animationPlaying = !animationPlaying;

        if (IsKeyPressed(KEY_LEFT)) { currentPose--; if (currentPose < 0) currentPose = seq.poses.size() - 1; }
        if (IsKeyPressed(KEY_RIGHT)) currentPose = ++currentPose % seq.poses.size();

        if (animationPlaying) {
            editorTimer += GetFrameTime();
            if (editorTimer >= frameDuration) {
                editorTimer = 0.0f;
                currentPose = ++currentPose % seq.poses.size();
                seq.poses[currentPose].setPose(parts);
            }
        }

        if (IsKeyPressed(KEY_W)) { selectedPart--; if (selectedPart < 0) selectedPart = parts.size() - 1; }
        if (IsKeyPressed(KEY_S))
            selectedPart = ++selectedPart % parts.size();

        if (IsKeyDown(KEY_T))
            parts[selectedPart].position = GetMousePosition();
        if (IsKeyDown(KEY_A))
            parts[selectedPart].localRotation -= rotationSpeed * editorMultiplier * dt;
        if (IsKeyDown(KEY_D))
            parts[selectedPart].localRotation += rotationSpeed * editorMultiplier * dt;

        for (int i = 0; i < parts.size(); i++) {
            Part& part = parts[i];
            // Set parts world position
            if (part.parent != nullptr) {
                if (part.connectedNotch == -1)
                    part.position = part.parent->position;
                else
                    part.position = part.parent->worldNotches[part.connectedNotch];
                part.worldRotation = part.localRotation + part.parent->worldRotation;
            }
            else {
                part.worldRotation = part.localRotation;
            }
            // Set notches world position
            for (int i = 0; i < part.localNotches.size(); i++) {
                part.worldNotches[i] = rotateAround(
                    part.localNotches[i] + part.position - part.pivot,
                    part.position,
                    part.worldRotation
                );
            }
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Draw all parts
        for (int i = 0; i < parts.size(); i++) {
            Part& part = parts[i];
            DrawTexturePro(
                texture,
                part.bounds,
                (Rectangle){part.position.x, part.position.y, part.bounds.width, part.bounds.height},
                (Vector2){part.pivot.x, part.pivot.y},
                part.worldRotation,
                WHITE
            );
        }
        // Draw selected part over everything
        Part& part = parts[selectedPart];
        DrawTexturePro(
            texture,
            part.bounds,
            (Rectangle){part.position.x, part.position.y, part.bounds.width, part.bounds.height},
            (Vector2){part.pivot.x, part.pivot.y},
            part.worldRotation,
            debugColors[selectedPart%debugColors.size()]
        );

        EndMode2D();

        Vector2 mouseWorldPos = GetMousePosition();
        DrawText(TextFormat("%.f", mouseWorldPos.x), 200, 10, 20, BLACK);
        DrawText(TextFormat("%.f", mouseWorldPos.y), 200, 30, 20, BLACK);
        
        // Interface
        for (int i = 0; i < parts.size(); i++) {
            Part& part = parts[i];
            // Parts list
            DrawText(
                TextFormat("%s", part.name.c_str()),
                10, 10+(20*i), 20,
                selectedPart == i ? debugColors[selectedPart%debugColors.size()] : WHITE
            );
        }
        
        if (seq.poses.size() > 0) {
            DrawRectangle(0, WIN_HEIGHT-10, WIN_WIDTH, 10, GRAY);
            int a = WIN_WIDTH / seq.poses.size();
            DrawRectangle(
                a*currentPose, WIN_HEIGHT-20,
                currentPose == seq.poses.size()-1 ? WIN_WIDTH: a, 20,
                BLUE
            );
            DrawText(TextFormat("%d", currentPose), 10, WIN_HEIGHT-60, 40, BLUE);
            DrawText(TextFormat("[%d]", seq.poses.size()), 60, WIN_HEIGHT-45, 20, GRAY);
        }

        // DrawText("Shift, A/D (rotate), T (set pos), W/S (select)", 10, WIN_HEIGHT - 30, 20, LIGHTGRAY);
        
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
