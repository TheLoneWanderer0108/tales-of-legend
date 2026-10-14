#include "battle_internal.h"
#include <algorithm>
#include <iostream>
#include <limits>

int LowestHpSeat(Battle& battle, const std::vector<int>& seats){
    auto it = std::min_element(seats.begin(), seats.end(), [&battle](int a, int b){
        return battle.actors[a].stats.CurrentHp < battle.actors[b].stats.CurrentHp;
    });
    return *it;
}

void DealBasicAttack(Battle& battle, Actor& person, int seat, bool verbose){
    int damage = ComputeDamage(person, battle.actors[seat], battle.rng);
    if(battle.actors[seat].defending){
        damage /= 2;
    }
    battle.actors[seat].stats.CurrentHp -= damage;
    if(battle.actors[seat].stats.CurrentHp < 0){
        battle.actors[seat].stats.CurrentHp = 0;
    }
    if(verbose){
        std::cout << person.name << " attacks " << battle.actors[seat].name
                  << " for " << damage << "\n";
    }
}

void ApplyPoison(Actor& target){
    for(auto& s: target.statuses){
        if(s.status == Status::poison){
            s.duration = 3;
            return;
        }
    }
    target.statuses.push_back({Status::poison, 3});
}

void ApplyAttackUp(Actor& target){
    for(auto& s: target.statuses){
        if(s.status == Status::attackup){
            s.duration = 3;
            return;
        }
    }
    target.statuses.push_back({Status::attackup, 3});
}

bool ReadMenuInt(int& out, int minV, int maxV){
    std::cin >> out;
    if(std::cin.fail()){
        std::cout << "just numbers\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        out = 0;
        return false;
    }
    return out >= minV && out <= maxV;
}

