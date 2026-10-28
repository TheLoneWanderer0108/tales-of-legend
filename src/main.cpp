#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>

struct Window {
    Window() {
        InitWindow(1280, 720, "Tales of Legend");
        SetTargetFPS(60);
    }
    ~Window() {
        CloseWindow();
    }
};

struct Actor{
    std::string name{"player"};
    double hp{10};
    double maxhp{10};
    double PosX {0};
    double PosY {0};
};

const Color lowhp = RED;
const Color middlehp = YELLOW;
const Color highhp = LIME;
const Color mainColorContainer = GREEN;
const Color textColor = BLACK;
const Color hpbarColor = DARKGRAY;
const Color lineColor = RED;
const Color backgroundColor = RAYWHITE;
const Color cursorColor = YELLOW;
const Color choiceMenuContainer = BLACK;
const Color menuOutline = DARKBLUE;
const Color choiceTextColor = BLUE;
const Color nonSelectedTextColor = BLACK;

void DrawActor(std::vector<Actor> actors){
    for(size_t i{0}; i<actors.size(); i++){
        double ratio = actors[i].hp / actors[i].maxhp;
        Color color = highhp;
        if(ratio <= 0.5 && ratio > 0.25){
            color = middlehp;
        }
        else if(ratio <= 0.25){
            color = lowhp;
        }
        else{
            color = highhp;
        }
        DrawRectangle(actors[i].PosX, actors[i].PosY, 100, 100, mainColorContainer); //remember to draw the shapes first before applying anything to them
        DrawText(TextFormat("%s", actors[i].name.c_str()), actors[i].PosX, actors[i].PosY + 20, 20, textColor);
        DrawRectangle(actors[i].PosX, actors[i].PosY - 20, 100, 10, hpbarColor);
        DrawText(TextFormat("%.2f", actors[i].hp), actors[i].PosX, actors[i].PosY + 50, 20, textColor);
        DrawRectangle(actors[i].PosX, actors[i].PosY - 20, ratio * 100, 10, color);
    }
}

int main(void){
    Window window; // RAII window, InitWindow and CloseWindow are called automatically

    std::vector<Actor> heroes {{"jack", 10, 10, 100.0, 400.0}, {"belle", 1, 4, 225.0, 400.0}, {"christi", 3, 7, 350.0, 400.0}};
    std::vector<Actor> enemies {{"Goblin", 2, 5, 900.0, 400.0}, {"Wolf", 6, 7, 1025.0, 400.0}};

    size_t number {0};
    float rightKeyTimer = 0.0;
    float upKeyTimer = 0.0;
    float leftKeyTimer = 0.0;
    float keyDownTimer = 0.0;
    size_t selected {0};
    std::vector<std::string> options = {"Attack", "Skills", "Items", "Run"};

    //loop
    while (!WindowShouldClose()){
        // update
        float dt = GetFrameTime();
        bool moveLeft = false;
        bool moveRight = false;
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
            keyDownTimer += dt;
            if(keyDownTimer > 0.4){
                moveDown = true;
                keyDownTimer = 0.3;
            }
        }else{
            keyDownTimer = 0.0;
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
        if(IsKeyPressed(KEY_RIGHT)){
            moveRight = true;
        }
        else if(IsKeyPressed(KEY_LEFT)){
            moveLeft = true;
        }

        if(IsKeyDown(KEY_RIGHT)){
            rightKeyTimer += dt;
            if(rightKeyTimer > 0.4){
                moveRight = true;
                rightKeyTimer = 0.3;
            }
        }else{
            rightKeyTimer = 0.0;
        }
        if(IsKeyDown(KEY_LEFT)){
            leftKeyTimer += dt;
            if(leftKeyTimer > 0.4){
                moveLeft = true;
                leftKeyTimer = 0.3;
            }
        }else{
            leftKeyTimer = 0.0;
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
        if(moveLeft){
            if(number <= 0){
                number = enemies.size() - 1;
            }else{
                number -= 1;
            }
        }
        if(moveRight){
            if(enemies.size() - 1 <= number){
                number = 0;
            }else{
                number += 1;
            }
        }
        BeginDrawing(); // draw
        ClearBackground(backgroundColor);
        DrawLineEx({0, 500}, {1280, 500}, 3.0, lineColor);
        DrawActor(heroes);
        DrawActor(enemies);
        //DrawText(TextFormat("%.2f", enemies[number].PosX), 200, 200, 20, textColor);
        DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.5f));
        DrawTriangle({enemies[number].PosX + 40, enemies[number].PosY - 50}, {enemies[number].PosX + 50, enemies[number].PosY - 30}, {enemies[number].PosX + 60, enemies[number].PosY - 50}, cursorColor);
        DrawRectangle(100, 520, 400, 180, Fade(choiceMenuContainer, 0.6f));
        DrawRectangleLines(100, 520, 400, 180, menuOutline);
        for(size_t i{0}; i<options.size(); i++){
            if( i == selected){
                DrawText(TextFormat("%s", options[i].c_str()), 120, 530 + i * 45, 25, choiceTextColor);
            }else{
                DrawText(TextFormat("%s", options[i].c_str()), 120, 530 + i * 45, 25, nonSelectedTextColor);
            }
        }
        EndDrawing(); // draw
    }

    return 0;
}
