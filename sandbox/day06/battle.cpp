#include <iostream>
#include <limits>
#include <random>
#include <algorithm>
#include "battle.h"
/*Move all your declarations to `battle.h` and definitions to `battle.cpp`, build both
      with your CMake target*/

int Clamp(int value,int low, int high){
    if (value < low){
        value = low;
        return value;
    }
    else if(value > high){
        value = high;
        return value;
    }
    else{
        return value;
    }
}
int selfHealth(int playerHealth){
    int amountHealth = 20 + playerHealth;

    int truehealth = Clamp(amountHealth, 0, playerHealth);
    return truehealth;
}
int selfHealth(int playerhealth, int maxHP){
    double porcent =  0.20;
    double trueporcentheal = maxHP * porcent;
    double NewHP = playerhealth + trueporcentheal;

    double trueporcenthealth = Clamp(NewHP, 0, maxHP);

    return trueporcenthealth;

}
void PrintMenu(){

    std::cout << "\nChoose what you want to do: 1) attack, 2)defend, 3)run, 4)quit\n";
}
bool IsDead(int hp){
    if(hp <= 0){
        return true;
    }
    else{
        return false;
    }
}
int ReadChoice(){

    int options =0;
    std::cin >> options;

    if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return 0;
        }
    else{
        return options;
    }
}
double RollDamage(int enDef, int playeratta, double variance){
    std::random_device rd;
    //this initiales the algorith used combined with the seed
    std::mt19937 gen(rd());
    //this calculates the base hero strength and substracts the enemies def
    int Basedamage = playeratta - enDef;
    int truedmg = Clamp(Basedamage, 0, 999);
    //we cant have damage be negative if enemie defense is lower then base strenght


    //give upper range of 20%
    double Highrange = truedmg + (truedmg * variance);
    //gives lower range of 20%
    double Lowrange = truedmg - (truedmg * variance);
    //grabs a random number from both ranges e.g, Baseddamage = 10, ranges will be (8,12)
    std::uniform_int_distribution<int> var(Lowrange, Highrange);
    //here we grab the random number in the range
    double TotalDmg = var(gen);
    return TotalDmg;

}
int ApplyDamage(int damage, int EnHP){

    double CurrentEnHP = EnHP - damage;
    return Clamp(CurrentEnHP, 0, 999);

}
void PrintStatus(int damage, int CurrentEnHP, int enHP){
    std::cout << "The Enemie has: " <<  enHP << "\n";
    std::cout << "You dealt: " << damage << " Damage!\n";
    std::cout << "Enemie has: " << CurrentEnHP;

}