#include <iostream>
#include <vector>
#include <memory>
#include <string>
/*Redesign it with composition: one `Enemy` class holding
      `bool canFly; Element element; std::vector<std::unique_ptr<Skill>> skills; AiProfile ai;`*/
/*Prove object slicing to yourself: store derived objects in a `std::vector<Base>`
      (not pointers), call a virtual function, watch the base version run*/
/*Write your rule in [../design/architecture.md](../design/architecture.md) as decision
      **D002**: where you will allow inheritance (interfaces with behaviour: `Skill`,
      `GameState`, `AiBehaviour`) and where you will not (enemy types, item types)*/
/*Convert the three skills from Day 14 into data where possible: could `Fireball` be
      a `Skill` **struct** with fields (power, element, cost, target rule) instead of a
      subclass? Try it. Note which approach you would rather add 40 skills to*/

enum ElementType{
    earth,
    fire,
    water,
    wind
};
enum Target{
    ally,
    enemy
};

enum Kind{
    damage,
    heal,
    basic
};

struct Stats{
        int hp{100};
        int maxHP{100};
        int att{1};
        int def{1};
        int maxfp{10};
        int fp{10};
};

struct Actor{
    std::string name{' '};
    Stats stats;
};
struct Skill{

    int power{0};
    ElementType type{fire};
    int cost{0};
    Target target{enemy};
    Kind kind{damage};
    
    void execute(Actor& user, Actor& target, Kind skillkind){
            switch (skillkind)
            {
            case damage:{
            user.stats.fp -= cost;
            int damageDealt = power * user.stats.maxfp / (user.stats.maxfp + (target.stats.def/2));
            target.stats.hp -= damageDealt;
            std::cout << " User: " <<user.name << " Attacks: " << target.name << "For: " << damageDealt << "\n";
            std::cout << "Current hp: " <<target.stats.hp <<"\n";
                break;
            }
            case heal:{
                int healed = power * user.stats.maxfp / 2;
                target.stats.hp += healed;
                user.stats.fp -= cost;

                std::cout << " User: " << user.name << " Has healed: "<< target.name << " for: " << healed <<"\n";
                std::cout << "target current health: " <<target.stats.hp<<"\n";
                break;
                }   
            default:
                break;
            }
        }


};

struct Attack{

        void execute(Actor& user, Actor& target){
            int damage = user.stats.att * user.stats.att / (user.stats.att + target.stats.def);
            target.stats.hp -= damage;
            std::cout << " User: " << user.name << " Attacks: " << target.name << " For: " << damage << "\n";
            std::cout << "Current hp: " <<target.stats.hp <<"\n";
        }
        std::string name() const{
            return "Attack";
        }

};

struct Element
{
    ElementType type{fire};
};
struct AiProfile{
    int agressive{1};
};

struct Enemy
{
    Actor actor;
    Element element;
    bool canFly{false};
    std::vector<Skill> skills;
    AiProfile ai;
};

int main(){

    Enemy goblin {{"Goblin", {100, 100, 2, 2, 2, 2}}, {fire}, false, {{5, fire, 1, enemy, damage}, {5, water, 1, ally, heal}}, {1}};
    Enemy troll {{"troll", {100, 100, 2, 2, 2, 2}}, {earth}, false, {{2, earth, 1, enemy, damage}}, {1}};

    for(size_t i{0}; i<goblin.skills.size(); i++){
        goblin.skills[i].execute(goblin.actor, troll.actor, goblin.skills[i].kind);
    }
    
    
    return 0;
}