#include <iostream>
#include <vector>

/*Define `struct Stats { int maxHp = 1; int hp = 1; int atk = 1; int def = 0; int spd = 1; };`*/
/*Define `struct Actor { std::string name; Stats stats; bool isEnemy = false; };`*/
/*Rewrite Day 7's party as `std::vector<Actor>` and delete the parallel vectors*/
/*Write free functions: `void PrintActor(const Actor&)`,
      `bool IsAlive(const Actor&)`, `void ApplyDamage(Actor&, int amount)`*/

/*Write `int ComputeDamage(const Actor& attacker, const Actor& target)` using a real
      formula. Start with `atk * atk / (atk + def)` and try several. Compare with the
      formulas in [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)*/
/*Write `int ComputeDamage(const Actor& attacker, const Actor& target)` using a real
      formula. Start with `atk * atk / (atk + def)` and try several. Compare with the
      formulas in [rpg-math-and-balance.md](../research/rpg-math-and-balance.md)*/
/*Print a damage table: your formula's output for attacker ATK 5-30 against target DEF
      0-20. Look for places where damage goes to zero or explodes*/

struct Stats
{
    int maxHp{1};
    int hp{1};
    int atk{1};
    int def{0};
    int spd{1};
};

struct Actor
{
    std::string name;
    Stats stats;
    bool isEnemy{false};
};

int ComputeDamage(const Actor& attacker, const Actor& defender){
    int attack = attacker.stats.atk * attacker.stats.atk / (attacker.stats.atk + defender.stats.def);

    return attack;
}

void PrintActor(const Actor& attacker, const Actor& defender){
    std::cout << attacker.name << " VS " << defender.name<<"\n";

    std::cout<< "Current: " << defender.name << " has: " <<defender.stats.hp <<"\n";


}
bool IsLive(const Actor& defender){
    bool isDead{false};
    if(defender.stats.hp <= 0){
        return isDead = true;
    }
    else{
        return isDead = false;
    }

}
void ApplyDamage(Actor& defender, int amount){

    defender.stats.hp -= amount;
}

int main(){
    std::vector<Actor> actor1{{"Johhny", {20, 20, 1, 1, 1}, false}, {"Goblin", {5, 5, 0, 0, 1}, true}};

    //int attack = ComputeDamage(actor1[0], actor1[1]);
    //int choice = 0;


    /*do{
        PrintActor(actor1[0], actor1[1]);
        std::cout << "Apply damage press 1\n";
        std::cin >> choice;

        if(choice == 1){
        ApplyDamage(actor1[1], attack);
        }

        else{
           std::cout << "No valid";
        }

    }while(IsLive(actor1[1]) == false);

    std::cout <<"enemie defeated"<<"\n";*/

    for(size_t i{5}; i < 31; i++){
        actor1[0].stats.atk = i;
        for (size_t j{0}; j < 21; j++)
        {

            actor1[1].stats.def = j;
            int attack = ComputeDamage(actor1[0], actor1[1]);
            std::cout << actor1[0].stats.atk << " against defense: " <<actor1[1].stats.def<< " Damage taken is: " <<attack << "\n";
        }
        
    }




    return 0;
}