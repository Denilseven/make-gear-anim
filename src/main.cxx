#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>

#define TEXTURE_TO_LOAD "assets/character_sprites.png"
#define WIN_WIDTH 1000
#define WIN_HEIGHT 1000
#define TRANSPARENT (Color){255, 255, 255, 0}
#define PHANTOM (Color){0, 255, 0, 120}

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

Part* getPartByName(std::vector<Part>& list, const std::string name) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i].name == name) {
            return &list[i];
        }
    }
    return nullptr;
}

// NOTE: This is assuming there won't be any errors in the file itself
void setFromFile(std::vector<Part>& list, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `parts.txt`!" << std::endl;
        std::exit(1);
    }

    list.clear();

    std::vector<std::string> parents{};
    std::string s{};
    while (getline(file, s)) {
        if (s == "+") {
            Part newPart{};

            getline(file, s); newPart.name = s;
            getline(file, s); newPart.bounds.x = std::stoi(s);
            getline(file, s); newPart.bounds.y = std::stoi(s);
            getline(file, s); newPart.bounds.width = std::stoi(s);
            getline(file, s); newPart.bounds.height = std::stoi(s);
            getline(file, s); newPart.pivot.x = std::stoi(s);
            getline(file, s); newPart.pivot.y = std::stoi(s);
            getline(file, s); parents.push_back(s);
            getline(file, s); newPart.connectedNotch = std::stoi(s);
        
            getline(file, s);
            while (s == ".") {
                getline(file, s); int x = std::stoi(s);
                getline(file, s); int y = std::stoi(s);
                newPart.localNotches.push_back((Vector2){(float)x, (float)y});
                newPart.worldNotches.push_back((Vector2){(float)x, (float)y});
                getline(file, s);
            }
            std::cout << newPart.name << std::endl;
            std::cout << &newPart << " is my pointer." << std::endl;
            std::cout << newPart.parent << " is my parent pointer." << std::endl;
            list.push_back(newPart);
        }
    }
    for (int i = 0; i < list.size(); i++) {
        list[i].parent = getPartByName(list, parents[i]);
        // We set the parent pointers after the list is complete because setting
        // them while it is being built and resized seems to mess up everything.
    }
    file.close();
}

int main() {
    SetTargetFPS(60);
    Texture2D texture = LoadTexture(TEXTURE_TO_LOAD);

    std::vector<Part> parts{};
    setFromFile(parts, "parts.txt");

    InitWindow(texture.width, texture.height, "FIRST_WINDOW");
    texture = LoadTexture(TEXTURE_TO_LOAD);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawTexture(texture, 0, 0, WHITE);
        for (Part& part : parts) {
            DrawRectangleLinesEx(part.bounds, 1, LIME);
            DrawCircleLinesV(part.pivot + (Vector2){part.bounds.x, part.bounds.y}, 7, LIME);
        }
        EndDrawing();
    }
    CloseWindow();

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "SECOND_WINDOW");
    texture = LoadTexture(TEXTURE_TO_LOAD);
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
                WHITE
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
