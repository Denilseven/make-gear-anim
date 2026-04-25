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

    Part* parent{nullptr};
    int connectedNotch{-1};

    Vector2 position{};
    float localRotation{0.0f};
    float worldRotation{0.0f};
    std::vector<Vector2> localNotches{};
    std::vector<Vector2> worldNotches{};
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
        }
    );
    parts.push_back(
        (Part){
            .name = "Connected",
            .bounds = (Rectangle){0, 0, 50, 300},
            .pivot = (Vector2){25, 250},
        }
    );
    parts.push_back(
        (Part){
            .name = "Last",
            .bounds = (Rectangle){0, 0, 50, 150},
            .pivot = (Vector2){25, 0},
        }
    );
    parts[0].localNotches.push_back((Vector2){0, -100});
    parts[0].localNotches.push_back((Vector2){100, 0});
    parts[0].worldNotches.resize(2);

    parts[1].parent = &parts[0];
    parts[1].connectedNotch = 1;
    parts[1].localNotches.push_back((Vector2){0, -250});
    parts[1].worldNotches.resize(1);

    parts[2].parent = &parts[1];
    parts[2].connectedNotch = 0;

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        // parts[0].scale = (Vector2){(float)GetMouseX() / 100.0f, (float)GetMouseY() / 100.0f};
        parts[0].localRotation = GetTime() * 25;
        parts[1].localRotation = GetTime() * -50;
        parts[2].localRotation = GetTime() * 37;
        parts[0].position = GetMousePosition();

        for (Part& part : parts) {
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
            
            DrawTexturePro(
                texture,
                part.bounds,
                (Rectangle){part.position.x, part.position.y, part.bounds.width, part.bounds.height},
                (Vector2){part.pivot.x, part.pivot.y},
                part.worldRotation,
                ORANGE
            );
        }

        // Gizmos
        for (Part& part : parts) {
            for (int i = 0; i < part.localNotches.size(); i++) {
                part.worldNotches[i] = rotateAround(
                    part.localNotches[i] + part.position,
                    part.position,
                    part.worldRotation
                );
                DrawCircleGradient(part.worldNotches[i], 7*(i+1), TRANSPARENT, RED);
            }
            DrawCircleGradient(part.position, 10, TRANSPARENT, BLUE);
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
