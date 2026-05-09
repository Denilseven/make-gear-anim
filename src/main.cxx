#include "common.hxx"
#include "editor.hxx"
#include "figure.hxx"
#include "files.hxx"
#include "part.hxx"
#include "pose.hxx"
#include "sequence.hxx"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    Texture texture{};
    Figure fig{};

    // Read given arguments
    {
        for (int i = 0; i < argc; i++) {
            std::string arg{std::string_view(argv[i])};

            if (arg == "-h" || arg == "--help") {
                printHelp();
                return 1;
            }
            else if (arg == "-t" || arg == "--texture") {
                textureFilename = argv[++i];
            }
            else if (arg == "-p" || arg == "--model") {
                figureFilename = argv[++i];
            }
            else if (arg == "-s" || arg == "--animation") {
                sequenceFilename = argv[++i];
            }
            else if (arg == "-z" || arg == "--zoom") {
                cameraZoomLevel = std::stof(argv[++i]);
            }
            else if (arg == "-r" || arg == "--rect") {
                referenceRectangle.x = std::stof(argv[++i]);
                referenceRectangle.y = std::stof(argv[++i]);
            }
            else if (i > 0) {
                std::cerr << "Error: Invalid argument given, `" << argv[i] << "`" << std::endl;
                printHelp();
                return 1;
            }
        }

        texture = LoadTexture(textureFilename);
        if (texture.width <= 0) {
            std::cerr << "Error: Failed to load `" << textureFilename << "` or it's an invalid file for a texture." << std::endl;
            UnloadTexture(texture);
            CloseWindow();
            return 1;
        }
    }

    // First window, "Parts slicer"
    {
        InitWindow(texture.width, texture.height, "FIRST_WINDOW");
        SetTargetFPS(60);

        texture = LoadTexture(textureFilename);
        bool partsAreValid = readFigureFromFile(fig, figureFilename);

        while (!WindowShouldClose() && !IsKeyDown(KEY_ENTER)) {
            SetMouseCursor(3);
            editorTimer += GetFrameTime();
            if (editorTimer >= 0.1f) {
                editorTimer = 0.0f;
                partsAreValid = readFigureFromFile(fig, figureFilename);
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
    }

    // Second window, "Sequencing window", the animation editor
    {
        InitWindow(windowWidth, windowHeight, "SECOND_WINDOW");
        SetTargetFPS(60);

        Sequence seq{};
        Figure dummy{};
        Camera2D cam{};

        cam.offset = (Vector2){windowWidth/2, windowHeight/2};
        cam.target = (Vector2){windowWidth/2, windowHeight/2};
        cam.zoom = cameraZoomLevel;

        int currentPose{0};
        int selectedPart{0};

        float dt{0.0f};

        readFigureFromFile(dummy, figureFilename);
        texture = LoadTexture(textureFilename);

        seq.addAt(fig.getPose());
        // If possible, load in the given sequence, if not, continue with empty
        readSequenceFromFile(seq, sequenceFilename);
        currentPose = 0;
        fig.setPose(seq.getAt(currentPose));

        while(!WindowShouldClose()) {
            dt = GetFrameTime();

            // General controls
            {
                // Play or stop
                if (IsKeyPressed(KEY_P)) {
                    editorMode = editorMode == EditorMode::playing ? DEFAULT_EDITOR_MODE : EditorMode::playing;
                    editorTimer = 0.0f;
                }
                if (IsKeyPressed(KEY_O)) { cycleOnionMode(); }
                if (IsKeyPressed(KEY_G)) { cycleGridSpace(); }
                if (IsKeyPressed(KEY_T)) { cycleEditorMode(); }

                // Save current animation sequence
                if (IsKeyPressed(KEY_FIVE)) {
                    writeSequenceToFile(seq, sequenceFilename);
                }
                // Load animation sequence
                if (IsKeyPressed(KEY_SIX)) {
                    readSequenceFromFile(seq, sequenceFilename);
                    currentPose = 0;
                    fig.setPose(seq.getAt(currentPose));
                }

                // Change currently selected part
                if (IsKeyPressed(KEY_UP)) {
                    selectedPart--;
                    if (selectedPart < 0)
                        selectedPart = fig.size() - 1;
                }
                if (IsKeyPressed(KEY_DOWN)) { selectedPart = ++selectedPart % fig.size(); }

                // Export
                if (IsKeyPressed(KEY_ZERO)) {
                    std::cout << "Tip: Started exporting!" << std::endl;
                    exportAsSpritesheet(texture, seq);
                    std::cout << "Tip: Done exporting!" << std::endl;
                }
            }

            // Editor controls
            if (editorMode != EditorMode::playing) {
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

                // Controls for modifying the figure
                editorMultiplier = IsKeyDown(KEY_LEFT_SHIFT) ? 0.1f : 1.0f;
                if (editorMode == EditorMode::rotate) {
                    // Change part rotation
                    if (IsKeyDown(KEY_A)) { fig[selectedPart].localRotation -= rotationSpeed * editorMultiplier * dt; }
                    if (IsKeyDown(KEY_D)) { fig[selectedPart].localRotation += rotationSpeed * editorMultiplier * dt; }
                }
                else if (editorMode == EditorMode::translate && fig.root != nullptr) {
                    // Change part position
                    if (IsKeyDown(KEY_A)) { fig.root->position.x -= translationSpeed * editorMultiplier * dt; }
                    else if (IsKeyDown(KEY_D)) { fig.root->position.x += translationSpeed * editorMultiplier * dt; }
                    if (IsKeyDown(KEY_W)) { fig.root->position.y -= translationSpeed * editorMultiplier * dt; }
                    else if (IsKeyDown(KEY_S)) { fig.root->position.y += translationSpeed * editorMultiplier * dt; }
                }

                // Mirror part
                if (IsKeyPressed(KEY_X)) {
                    fig[selectedPart].mirror = !fig[selectedPart].mirror;
                }

                // We're saving the position on every frame rather than when there's a change to it
                // I don't know yet how much that affects performance but we'll roll with this for now
                seq.setAt(fig.getPose(), currentPose);
            }
            // Play the animation by going through the sequence
            else {
                editorTimer += GetFrameTime();
                if (editorTimer >= frameDuration) {
                    editorTimer = 0.0f;
                    currentPose = ++currentPose % seq.size();
                    fig.setPose(seq.getAt(currentPose));
                }
            }

            fig.update();

            BeginDrawing(); {
                ClearBackground(DARKGRAY);

                BeginMode2D(cam); {
                    // Draw onion-skinning
                    if (seq.size() > 1) {
                        if (onionMode == OnionMode::adjacent) {
                            if (currentPose != 0) dummy.setPose(seq[currentPose-1]);
                            else dummy.setPose(seq[seq.size()-1]);
                            dummy.update();
                            dummy.draw(texture, SPECTRE);
                            if (currentPose < seq.size()-1) dummy.setPose(seq[currentPose+1]);
                            else dummy.setPose(seq[0]);
                            dummy.update();
                            dummy.draw(texture, MELLOWS);
                        }
                        else if (onionMode == OnionMode::all) {
                            for (int i = 0; i < seq.size(); i++) {
                                if (i == currentPose) continue;
                                dummy.setPose(seq[i]);
                                dummy.update();
                                dummy.draw(texture, PHANTOM);
                            }
                        }
                    }

                    // Draw grid
                    if (gridMode == GridMode::grid) {
                        for (float i = 0; i <= windowWidth; i+=gridSpace)
                            DrawLine(i, 0, i, windowHeight, BLACK);
                        for (float i = 0; i <= windowHeight; i+=gridSpace)
                            DrawLine(0, i, windowWidth, i, BLACK);
                    }
                    else if (gridMode == GridMode::rect) {
                        DrawRectangleLines(
                            (windowWidth / 2) - (referenceRectangle.x / 2),
                            (windowHeight / 2) - (referenceRectangle.y / 2),
                            referenceRectangle.x,
                            referenceRectangle.y,
                            BLACK
                        );
                    }

                    // Draw all parts
                    fig.draw(texture, WHITE);
                    // Draw selected part over everything
                    fig[selectedPart].draw(texture, debugColors[selectedPart%debugColors.size()]);
                }
                EndMode2D();

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
                    DrawRectangle(0, windowHeight-10, windowWidth, 10, GRAY);
                    int a = windowWidth / seq.size();
                    DrawRectangle(
                        a*currentPose, windowHeight-20,
                        currentPose == seq.size()-1 ? windowWidth: a, 20,
                        BLUE
                    );
                    DrawText(TextFormat("%d", currentPose), 10, windowHeight-60, 40, BLUE);
                    DrawText(TextFormat("[%d]", seq.size()), 60, windowHeight-45, 20, GRAY);
                }

                // Draw mode indicator
                {
                    Color color{WHITE};
                    int sides{0};
                    // NOTE: this if-cascate over an enum looks kinda sus lel
                    if (editorMode == EditorMode::rotate) { color = GREEN; sides = 6; }
                    else if (editorMode == EditorMode::translate) { color = RED; sides = 4; }
                    else if (editorMode == EditorMode::playing) { color = BLUE; sides = 3; }
                    DrawPoly(
                        (Vector2){windowWidth - 30, 30},
                        sides, 15.0f, 0.0f, color
                    );
                }
            }
            EndDrawing();
        }

        UnloadTexture(texture);
        CloseWindow();
        return 0;
    }
}
