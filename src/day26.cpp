#include <iostream>
#include "raylib.h"
#include <cmath>

// Move a rectangle with arrow keys using `position += speed * dt` where speed is in
//pixels per second (try 200)

struct Window {
    Window() {
        InitWindow(1280, 720, "Tales of Legend");
        SetTargetFPS(60);
    }
    ~Window() {
        CloseWindow();
    }
};

float clamp(float value, float min, float max){
    if(value < min){
        return value = min;
    }
    else if(value > max){
        return value = max;
    }

    return value;
}

int main(void){

    Window window; // RAII window, InitWindow and CloseWindow are called automatically
    float speed = 200.0f; // pixels per second
    float rectX = 400.0f;
    float rectY = 200.0f;
    float velX = 0;
    float vely = 0;
    float accel = 800;
    float friction = 0.1f;
    float rectWidth = 100.0f;  // Make it wide
    float rectHeight = 50.0f;

    //loop
    while (!WindowShouldClose()){
        // update
        float dt = GetFrameTime();
        float dirX = 0;
        float dirY = 0;
        if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
            dirX += 1;
        }
        if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
            dirX -= 1;
        }

        if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)){
            dirY -= 1;
        }
        if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)){
            dirY += 1;
        }
        float lenght = std::sqrt(pow(dirX, 2) + pow(dirY, 2));
        if(lenght > 0){
            dirX/=lenght;
            dirY/=lenght;
        }
        velX += dirX * accel * dt;
        vely += dirY * accel * dt;
        velX *= pow(friction, dt);
        vely *= pow(friction, dt);
        rectX += velX * dt;
        rectY += vely * dt;
        rectX = clamp(rectX, 0, GetScreenWidth() - rectWidth);
        rectY = clamp(rectY, 0, GetScreenHeight() - rectHeight);

        BeginDrawing(); // draw
        ClearBackground(RAYWHITE);
        DrawText(TextFormat("lenght: %.2f", lenght), 250, 200, 20, BLACK);
        DrawRectangle(rectX, rectY, rectWidth, rectHeight, BLUE);
        DrawText(TextFormat("ms: %.2f", dt * 1000), 300, 250, 20, GREEN);

        EndDrawing(); // draw
    }



    return 0;
}
