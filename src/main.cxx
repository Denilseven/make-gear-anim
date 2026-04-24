#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>

#define WIN_WIDTH 1000
#define WIN_HEIGHT 1000
#define TRANSPARENT (Color){1, 1, 1, 0}

struct Part {
    std::string name{};
    Rectangle bounds{};
    Vector2 pivot{};
    std::vector<Vector2> attachmentPoints{};

    std::string partAttachedTo{};
    int attachedToPointN{0};

    Vector2 position{};
    Vector2 scale{1.0f, 1.0f};
    float rotation{0.0f};
};


int main() {
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Awesome");
    SetTargetFPS(60);

    Texture2D texture = LoadTexture("assets/image.png");
    std::vector<Part> parts{};
     
    parts.push_back(
        (Part){
            .name = "Heheh",
            .bounds = (Rectangle){0, 0, 200, 200},
            .pivot = (Vector2){100, 200},
            .position = (Vector2){500, 500},
        }
    );
    parts[0].attachmentPoints.push_back((Vector2){100, 200});
    parts[0].attachmentPoints.push_back((Vector2){0, 200});

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        parts[0].scale = (Vector2){(float)GetMouseX() / 100.0f, (float)GetMouseY() / 100.0f};
        // parts[0].scale = (Vector2){1.0f, 1.0f};

        for (int i = 0; i < parts.size(); i++) {
            Part part = parts[i];
            DrawTexturePro(
                texture,
                part.bounds,
                (Rectangle){part.position.x, part.position.y, part.bounds.width * part.scale.x, part.bounds.height * part.scale.y},
                (Vector2){part.pivot.x * part.scale.x, part.pivot.y * part.scale.y},
                GetTime() * 100,
                ORANGE
            );
            DrawCircleGradient(part.position, 10, TRANSPARENT, BLUE);
            for (int j = 0; j < part.attachmentPoints.size(); j++) {
                DrawCircleGradient(part.attachmentPoints[j], 7, TRANSPARENT, RED);
            }
        }

        EndMode2D();

        Vector2 mouseWorldPos = GetMousePosition();
        DrawText(TextFormat("%.f", mouseWorldPos.x), 200, 10, 20, BLACK);
        DrawText(TextFormat("%.f", mouseWorldPos.y), 200, 30, 20, BLACK);
        
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
