#include "common.hxx"
#include "part.hxx"
#include "pose.hxx"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>

int main() {
    Texture2D texture = LoadTexture(TEXTURE_TO_LOAD);

    std::vector<Part> parts{};
    std::vector<Color> debugColors = {GOLD, PINK, GREEN, SKYBLUE, PURPLE};

    InitWindow(texture.width, texture.height, "FIRST_WINDOW");
    SetTargetFPS(60);

    texture = LoadTexture(TEXTURE_TO_LOAD);
    bool partsAreValid = readPartsListFromFile(parts, PARTS_DESCRIPTION_FILE);
    float timer{0.0f}; // This `timer` thing could be improved but it's ok

    while (!WindowShouldClose() && !IsKeyDown(KEY_ENTER)) {
        SetMouseCursor(3);
        timer += GetFrameTime();
        if (timer >= 0.1f) {
            timer = 0.0f;
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
    float dt{0.0f};
    float multiplier{1.0f};
    const float rotationSpeed{100.0f};

    while(!WindowShouldClose()) {
        dt = GetFrameTime();
        multiplier = IsKeyDown(KEY_LEFT_SHIFT) ? 0.1f : 1.0f;

        // DEBUG
        if (IsKeyPressed(KEY_ONE)) writePoseToFile(parts);
        if (IsKeyPressed(KEY_TWO)) setPoseFromFile(parts);

        if (IsKeyPressed(KEY_W)) {
            selectedPart--;
            if (selectedPart < 0)
                selectedPart = parts.size() - 1;
        }
        if (IsKeyPressed(KEY_S))
            selectedPart = ++selectedPart % parts.size();

        if (IsKeyDown(KEY_T))
            parts[selectedPart].position = GetMousePosition();
        if (IsKeyDown(KEY_A))
            parts[selectedPart].localRotation -= rotationSpeed * multiplier * dt;
        if (IsKeyDown(KEY_D))
            parts[selectedPart].localRotation += rotationSpeed * multiplier * dt;

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
        
        DrawText("Shift, A/D (rotate), T (set pos), W/S (select)", 10, WIN_HEIGHT - 30, 20, LIGHTGRAY);
        
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
