#include "common.hxx"
#include "figure.hxx"
#include "part.hxx"
#include "persistenceManager.hxx"
#include "pose.hxx"
#include "sequence.hxx"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    // Read given arguments
    for (int i = 0; i < argc; i++) {
        if (std::string_view(argv[i]) == "-h") {
            printHelp();
            return 1;
        }
        else if (std::string_view(argv[i]) == "-t") {
            textureFilename = argv[++i];
        }
        else if (std::string_view(argv[i]) == "-p") {
            partsFilename = argv[++i];
        }
        else if (std::string_view(argv[i]) == "-s") {
            sequenceFilename = argv[++i];
        }
        else if (i > 0) {
            std::cerr << "Error: Invalid argument given, `" << argv[i] << "`" << std::endl;
            printHelp();
            return 1;
        }
    }

    Texture texture = LoadTexture(textureFilename);
    if (texture.width <= 0) {
        std::cerr << "Error: Failed to load `" << textureFilename << "` or it's an invalid file for a texture." << std::endl;
        UnloadTexture(texture);
        CloseWindow();
        return 1;
    }

    float editorTimer{0.0f};
    float frameDuration{1.0f/12.0f}; // 12 fps animation

    Figure fig{};
    Figure dummy{};
    std::vector<Color> debugColors = {GOLD, PINK, GREEN, SKYBLUE, PURPLE};

    InitWindow(texture.width, texture.height, "FIRST_WINDOW");
    SetTargetFPS(60);

    texture = LoadTexture(textureFilename);
    bool partsAreValid = fig.readFromFile(partsFilename);

    while (!WindowShouldClose() && !IsKeyDown(KEY_ENTER)) {
        SetMouseCursor(3);
        editorTimer += GetFrameTime();
        if (editorTimer >= 0.1f) {
            editorTimer = 0.0f;
            partsAreValid = fig.readFromFile(partsFilename);
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        Vector2 mousePos = GetMousePosition();
        DrawLine(mousePos.x, 0, mousePos.x, texture.height, GRAY);
        DrawLine(0, mousePos.y, texture.width, mousePos.y, GRAY);
        DrawText(TextFormat("%.f\n%.f", mousePos.x, mousePos.y), 10, 10, 20, LIGHTGRAY);

        DrawTexture(texture, 0, 0, BLACK);
        for (int i = 0; i < fig.size(); i++) {
            Part& part = fig[i];
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
        return partsAreValid ? 0 : 1;
    }
    CloseWindow();

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "SECOND_WINDOW");
    SetTargetFPS(60);

    dummy.readFromFile(partsFilename);
    texture = LoadTexture(textureFilename);
    int selectedPart{0};
    int currentPose{0};
    int onionMode{2};
    int gridSpace{-1};
    bool animationPlaying{false};

    float dt{0.0f};
    float editorMultiplier{1.0f};
    const float rotationSpeed{100.0f};

    Sequence seq{};
    seq.addAt(fig.getPose());
    // If possible, load in the given sequence, if not, continue with empty
    PersistenceManager::setSequenceFromFile(seq, sequenceFilename);
    currentPose = 0;
    fig.setPose(seq.getAt(currentPose));

    while(!WindowShouldClose()) {
        dt = GetFrameTime();

        // Save current animation sequence
        if (IsKeyPressed(KEY_FIVE)) {
            PersistenceManager::save(seq, sequenceFilename);
        }
        // Load animation sequence
        if (IsKeyPressed(KEY_SIX)) {
            PersistenceManager::setSequenceFromFile(seq, sequenceFilename);
            currentPose = 0;
            fig.setPose(seq.getAt(currentPose));
        }

        // Play or stop
        if (IsKeyPressed(KEY_P)) {
            animationPlaying = !animationPlaying; editorTimer = 0.0f;
        }
        // Duplicate current frame
        if (IsKeyPressed(KEY_ONE)) {
            seq.addAt(fig.getPose(), currentPose);
            currentPose++;
        }
        // Delete current frame
        if (IsKeyPressed(KEY_TWO) && seq.size() > 1) {
            seq.removeAt(currentPose);
            if (currentPose > 0) currentPose--;
            fig.setPose(seq.getAt(currentPose));
        }
        // Go to the previous frame
        if (IsKeyPressed(KEY_LEFT)) {
            currentPose--;
            if (currentPose < 0)
                currentPose = seq.size() - 1;
            fig.setPose(seq.getAt(currentPose));
        }
        // Go to the next frame
        if (IsKeyPressed(KEY_RIGHT)) {
            currentPose = ++currentPose % seq.size();
            fig.setPose(seq.getAt(currentPose));
        }

        // Change current selected part
        if (IsKeyPressed(KEY_W)) {
            selectedPart--;
            if (selectedPart < 0)
                selectedPart = fig.size() - 1;
        }
        if (IsKeyPressed(KEY_S)) { selectedPart = ++selectedPart % fig.size(); }

        // Change onion mode
        // WARNING: magic number!
        if (IsKeyPressed(KEY_O)) { onionMode = ++onionMode % 3; }

        // Change grids
        // WARNING: many magic numbers!!!
        if (IsKeyPressed(KEY_G)) {
            if (gridSpace == -1)
                gridSpace = 250;
            else if (gridSpace == 250)
                gridSpace = 125;
            else
                gridSpace = -1;
        }

        editorMultiplier = IsKeyDown(KEY_LEFT_SHIFT) ? 0.1f : 1.0f;
        // Change part rotation
        if (IsKeyDown(KEY_A)) { fig[selectedPart].localRotation -= rotationSpeed * editorMultiplier * dt; }
        if (IsKeyDown(KEY_D)) { fig[selectedPart].localRotation += rotationSpeed * editorMultiplier * dt; }
        // Change part position
        if (IsKeyDown(KEY_T)) { fig[selectedPart].position = GetMousePosition(); }

        if (animationPlaying) {
            editorTimer += GetFrameTime();
            if (editorTimer >= frameDuration) {
                editorTimer = 0.0f;
                currentPose = ++currentPose % seq.size();
                fig.setPose(seq.getAt(currentPose));
            }
        }
        else {
            // We're saving the position on every frame rather than when there's a change to it
            // I don't know yet how much that affects performance but we'll roll with this for now
            seq.setAt(fig.getPose(), currentPose);
        }

        fig.update();

        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        // Onion-skinning
        if (seq.size() > 1) {
            if (onionMode == 1) {
                // "Adjacent poses only"
                if (currentPose != 0) dummy.setPose(seq[currentPose-1]);
                else dummy.setPose(seq[seq.size()-1]);
                dummy.update();
                dummy.draw(texture, SPECTRE);
                if (currentPose < seq.size()-1) dummy.setPose(seq[currentPose+1]);
                else dummy.setPose(seq[0]);
                dummy.update();
                dummy.draw(texture, MELLOWS);
            }
            else if (onionMode == 2) {
                // "All poses"
                for (int i = 0; i < seq.size(); i++) {
                    if (i == currentPose) continue;
                    dummy.setPose(seq[i]);
                    dummy.update();
                    dummy.draw(texture, PHANTOM);
                }
            }
        }
        
        // Draw grid
        if (gridSpace != -1) {
            for (int i = 0; i < WIN_WIDTH; i+=gridSpace)
                DrawLine(i, 0, i, WIN_HEIGHT, BLACK);
            for (int i = 0; i < WIN_HEIGHT; i+=gridSpace)
                DrawLine(0, i, WIN_WIDTH, i, BLACK);
        }

        // Draw all parts
        fig.draw(texture, WHITE);
        // Draw selected part over everything
        fig[selectedPart].draw(texture, debugColors[selectedPart%debugColors.size()]);

        // Draw parts list
        for (int i = 0; i < fig.size(); i++) {
            Part& part = fig[i];
            DrawText(
                TextFormat("%s", part.name.c_str()),
                10, 10+(20*i), 20,
                selectedPart == i ? debugColors[selectedPart%debugColors.size()] : WHITE
            );
        }
        
        // Draw timeline
        if (seq.size() > 0) {
            DrawRectangle(0, WIN_HEIGHT-10, WIN_WIDTH, 10, GRAY);
            int a = WIN_WIDTH / seq.size();
            DrawRectangle(
                a*currentPose, WIN_HEIGHT-20,
                currentPose == seq.size()-1 ? WIN_WIDTH: a, 20,
                BLUE
            );
            DrawText(TextFormat("%d", currentPose), 10, WIN_HEIGHT-60, 40, BLUE);
            DrawText(TextFormat("[%d]", seq.size()), 60, WIN_HEIGHT-45, 20, GRAY);
        }
        
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
