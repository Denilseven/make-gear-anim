// TODO: take `scale` into account for `attachmentPoints` positioning

#include <iostream>
#include <raylib.h>
#include <raymath.h>
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

Vector2 rotateAround(const Vector2& point, const Vector2& center, float degrees) {
    float radians = degrees * (M_PI / 180);
    Vector2 result = (Vector2){
        result.x = (point.x-center.x)*std::cos(radians) - (point.y-center.y)*std::sin(radians),
        result.y = (point.x-center.x)*std::sin(radians) + (point.y-center.y)*std::cos(radians)
    };
    return result + center;
}

int main() {
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Awesome");
    SetTargetFPS(60);

    Texture2D texture = LoadTexture("assets/image.png");
    std::vector<Part> parts{};
     
    parts.push_back(
        (Part){
            .name = "Heheh",
            .bounds = (Rectangle){0, 0, 200, 200},
            .pivot = (Vector2){100, 100},
            .position = (Vector2){500, 500},
        }
    );
    parts[0].attachmentPoints.push_back((Vector2){0, -100});
    parts[0].attachmentPoints.push_back((Vector2){100, 0});

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        // parts[0].scale = (Vector2){(float)GetMouseX() / 100.0f, (float)GetMouseY() / 100.0f};
        parts[0].scale = (Vector2){1.0f, 1.0f};
        parts[0].rotation = GetTime() * 25;
        // parts[0].rotation = 40;
        parts[0].position = GetMousePosition();

        for (int i = 0; i < parts.size(); i++) {
            Part part = parts[i];
            DrawTexturePro(
                texture,
                part.bounds,
                (Rectangle){part.position.x, part.position.y, part.bounds.width * part.scale.x, part.bounds.height * part.scale.y},
                (Vector2){part.pivot.x * part.scale.x, part.pivot.y * part.scale.y},
                part.rotation,
                ORANGE
            );
            DrawCircleGradient(part.position, 10, TRANSPARENT, BLUE);

            for (int j = 0; j < part.attachmentPoints.size(); j++) {
                DrawCircleGradient(
                    rotateAround(
                        part.attachmentPoints[j] + part.position,
                        part.position,
                        part.rotation
                    ),
                    7 * (j + 1), TRANSPARENT, RED);
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
