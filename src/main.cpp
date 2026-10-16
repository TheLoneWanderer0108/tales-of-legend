#include <iostream>
#include "raylib.h"

//Get a 1280x720 window titled "Tales of Legend" opening and closing cleanly
//Draw text, a rectangle, and a circle. Confirm that y increases *downwards*
//Remove `ClearBackground` and move a shape - see the smearing. That tells you what
//clearing does
// Draw something *after* `EndDrawing` and observe that nothing appears
// Print `GetFrameTime()` and `GetFPS()` on screen with `DrawText` +
//`TextFormat`
// Wrap the window in an RAII class (`struct Window { Window(); ~Window(); }`) exactly
//like your Day 13 `ScopedLog`, so `CloseWindow` can never be forgotten
// Comment your `main` with the four-line structure: init, loop, update, draw

struct Window {
    Window() {
        InitWindow(1280, 720, "Tales of Legend");
        SetTargetFPS(60);
    }
    ~Window() {
        CloseWindow();
    }
};

int main(void){

    Window window; // RAII window, InitWindow and CloseWindow are called automatically

    int x_circle_pos = 250;
    int y_circle_pos = 300;
    //loop
    while (!WindowShouldClose()){
        // update
        BeginDrawing(); // draw
        ClearBackground(RAYWHITE);
        DrawText("Tales of Legend", 190, 200, 20, LIGHTGRAY);
        DrawRectangle(100, 100, 200, 200, RED);
        DrawCircle(x_circle_pos, y_circle_pos, 110, BLUE);
        DrawText("Y increases downwards", 50, 400, 20, BLACK);
        DrawText(TextFormat("Frame Time: %.02f ms", GetFrameTime() * 1000), 50, 450, 20, BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 50, 500, 20, BLACK);
        EndDrawing(); // draw
    }



    return 0;
}
