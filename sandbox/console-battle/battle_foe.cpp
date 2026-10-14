#include "battle.h"
#include "battle_internal.h"
#include <iostream>
#include <random>

void RunFoeTurn(Battle& battle, Actor& person, const std::unordered_map<int, SkillDef>& skills, bool verbose){
    person.defending = false;
    auto targets = LivingSeats(battle, Side::ally);
    if(targets.empty()){
        return;
    }
    std::uniform_int_distribution<int> distAction(1, 100);
    int roll = distAction(battle.rng);
    if(roll <= 60){
        DealBasicAttack(battle, person, LowestHpSeat(battle, targets), verbose);
        return;
    }
    if(roll <= 85){
        if(person.skillsIds.empty()){
            return;
        }
        std::uniform_int_distribution<> pick(0, static_cast<int>(person.skillsIds.size()) - 1);
        int skillId = person.skillsIds[pick(battle.rng)];
        const SkillDef& skill = skills.at(skillId);
        if(person.stats.CurrentFp < skill.FpCost){
            if(verbose){
                std::cout << person.name << " lacked FP for " << skill.Name << "\n";
            }
            return;
        }
        std::uniform_int_distribution<> tpick(0, static_cast<int>(targets.size()) - 1);
        int seat = targets[tpick(battle.rng)];
        person.stats.CurrentFp -= skill.FpCost;
        int damage = ComputeSkillDamage(person, battle.actors[seat], skill, battle.rng);
        if(battle.actors[seat].defending){
            damage /= 2;
        }
        battle.actors[seat].stats.CurrentHp -= damage;
        if(battle.actors[seat].stats.CurrentHp < 0){
            battle.actors[seat].stats.CurrentHp = 0;
        }
        if(verbose){
            std::cout << person.name << " uses " << skill.Name << " on "
                      << battle.actors[seat].name << " for " << damage << "\n";
        }
        return;
    }
    std::uniform_int_distribution<> rpick(0, static_cast<int>(targets.size()) - 1);
    DealBasicAttack(battle, person, targets[rpick(battle.rng)], verbose);
}

