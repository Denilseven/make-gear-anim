#include <raylib.h>

#define WIN_WIDTH 1000
#define WIN_HEIGHT 1000

int main(){
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Awesome");
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        EndMode2D();

        Vector2 mouseWorldPos = GetMousePosition();
        DrawText(TextFormat("%.f", mouseWorldPos.x), 200, 10, 20, BLACK);
        DrawText(TextFormat("%.f", mouseWorldPos.y), 200, 30, 20, BLACK);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
