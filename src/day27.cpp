#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"

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
        Window window;

        size_t selected {0};
        std::vector<std::string> options = {"Attack", "Skills", "Items", "Run"};
        float timer = 0.0;
        float upKeyTimer = 0.0;


    while (!WindowShouldClose()){
        // update
        float dt = GetFrameTime();
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        bool moveDown = false;
        bool moveUp = false;
        if(IsKeyPressed(KEY_DOWN)){
          moveDown = true;
        }
        else if(IsKeyPressed(KEY_UP)){
            moveUp = true;
        }

        if(IsKeyPressed(KEY_ENTER)){
            std::cout << options[selected] << "\n";
        }
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            std::cout << "Left click" << "\n";
        }
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
            std::cout << "Right click" << "\n";
        }
        if(IsKeyDown(KEY_DOWN)){
            timer += dt;
            if(timer > 0.4){
                moveDown = true;
                timer = 0.3;
            }
        }else{
            timer = 0.0;
        }
        if(IsKeyDown(KEY_UP)){
            upKeyTimer += dt;
            if(upKeyTimer > 0.4){
                moveUp = true;
                upKeyTimer = 0.3;
            }
        }else{
            upKeyTimer = 0.0;
        }
        if(moveUp){
            if(selected <= 0){
                selected = options.size() - 1;
            }else{
                selected -= 1;
            }
        }
        if(moveDown){
            if(options.size() - 1 <= selected){
                selected = 0;
            }else{
                selected += 1;
            }
        }
        BeginDrawing(); // draw
        ClearBackground(RAYWHITE);
        for(size_t i{0}; i<options.size(); i++){
            if( i == selected){
                DrawText(TextFormat("%s", options[i].c_str()), 50, 50 * i, 20, BLUE);
            }else{
                DrawText(TextFormat("%s", options[i].c_str()), 50, 50 * i, 20, BLACK);
            }
        }
        DrawCircle(mouseX, mouseY, 15, GOLD);
        DrawText(TextFormat("time: %.2f", timer), 300, 300, 20, GREEN);
        DrawText(TextFormat("mouseposX: %d", mouseX), 400, 400, 20, PURPLE);
        DrawText(TextFormat("mouseposY: %d", mouseY), 450, 450, 20, PINK);
        EndDrawing(); // draw
    }


    return 0;
}
