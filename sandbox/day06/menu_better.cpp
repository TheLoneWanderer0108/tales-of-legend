#include <iostream>
#include "battle.h"
/*Write a menu loop: print `1) Attack 2) Defend 3) Run 4) Quit`, read a choice, act,
loop until quit. Handle invalid input without crashing or infinite-looping*/
/*Take yesterday's menu program and extract at least five functions:
`PrintMenu`, `ReadChoice`, `RollDamage`, `ApplyDamage`, `PrintStatus`*/
/*Write `int RollDamage(int attack, int defense, int variancePercent)` and test it
      with edge cases: zero attack, defense higher than attack (should never return
      negative), huge numbers*/
/*Write `bool IsDead(int hp)` and `int Clamp(int value, int low, int high)` yourself.
      You will use `Clamp` hundreds of times over the next eight months*/
/*Overload `Heal` for a flat amount and for a percentage. Decide whether overloading
      or two clear names (`HealFlat`, `HealPercent`) is better here, and justify it*/

int main(){
    //while loops technically at the end looks for outside variables
    int choice = 0;
    int enHP = 120;
    int enDef = 10;
    int MyHP = 50;
    int maxHP = 120;
    int playerAtt = 20;
    double variance = 0.20;
    bool isDead = false;
    do{
        PrintMenu();
        choice = ReadChoice();

        if( choice == 1){
            int damage = RollDamage(enDef, playerAtt, variance);
            int currentEnHP = ApplyDamage(damage, enHP);
            std::cout << "you attacked\n";
            PrintStatus(damage, currentEnHP, enHP);
            enHP = currentEnHP;
            //check after we do damage
            isDead = IsDead(enHP);
        }
        else if (choice == 2){
            int currentplayerhp = selfHealth(MyHP, maxHP);
            std::cout << "You have: \n" <<MyHP;
            std::cout << "You healed for: \n" << currentplayerhp;
            MyHP = currentplayerhp;
            std::cout <<"Current Health: \n" << MyHP;
        }
        else if (choice == 3){
            std::cout << "You ran\n";
        }
        else if (choice == 4){
            std::cout << "You quit\n";
        }
        else if (choice == 0){
            std::cout << "\njust numbers\n";
        }
        else{
            std::cout << "\nInvalid option\n";
        }
    }while(choice != 4 && isDead == false);

    
    return 0;

}