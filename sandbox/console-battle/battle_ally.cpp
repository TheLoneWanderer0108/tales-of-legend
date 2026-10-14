#include "battle.h"
#include "battle_internal.h"
#include <iostream>
#include <vector>

void AllyBasicAttack(Battle& battle, Actor& person, bool autoPlay, bool verbose){
    auto targets = LivingSeats(battle, Side::foe);
    if(targets.empty()){
        return;
    }
    int seat = 0;
    if(autoPlay){
        seat = LowestHpSeat(battle, targets);
    }
    else{
        int choice = 0;
        do{
            std::cout << "choose an enemy:\n";
            for(size_t j{0}; j < targets.size(); j++){
                std::cout << j + 1 << " : " << battle.actors[targets[j]].name << "\n";
            }
        }while(!ReadMenuInt(choice, 1, static_cast<int>(targets.size())));
        seat = targets[choice - 1];
    }
    DealBasicAttack(battle, person, seat, verbose);
}

void AllySkillOnFoes(Battle& battle, Actor& person, const SkillDef& skill, bool autoPlay, bool verbose){
    auto targets = LivingSeats(battle, Side::foe);
    if(targets.empty()){
        return;
    }
    if(skill.target == Target::allenemies){
        for(int seat: targets){
            int damage = ComputeSkillDamage(person, battle.actors[seat], skill, battle.rng);
            battle.actors[seat].stats.CurrentHp -= damage;
            if(battle.actors[seat].stats.CurrentHp < 0){
                battle.actors[seat].stats.CurrentHp = 0;
            }
            if(verbose){
                std::cout << person.name << " used " << skill.Name << " on "
                          << battle.actors[seat].name << " for " << damage << "\n";
            }
        }
        person.stats.CurrentFp -= skill.FpCost;
        return;
    }
    int seat = autoPlay ? LowestHpSeat(battle, targets) : 0;
    if(!autoPlay){
        int choice = 0;
        do{
            std::cout << "choose an enemy:\n";
            for(size_t j{0}; j < targets.size(); j++){
                std::cout << j + 1 << " : " << battle.actors[targets[j]].name << "\n";
            }
        }while(!ReadMenuInt(choice, 1, static_cast<int>(targets.size())));
        seat = targets[choice - 1];
    }
    int damage = ComputeSkillDamage(person, battle.actors[seat], skill, battle.rng);
    battle.actors[seat].stats.CurrentHp -= damage;
    if(battle.actors[seat].stats.CurrentHp < 0){
        battle.actors[seat].stats.CurrentHp = 0;
    }
    if(battle.actors[seat].stats.CurrentHp > 0){
        ApplyPoison(battle.actors[seat]);
    }
    person.stats.CurrentFp -= skill.FpCost;
    if(verbose){
        std::cout << person.name << " used " << skill.Name << " on "
                  << battle.actors[seat].name << " for " << damage << "\n";
    }
}

void AllySkillOnAllies(Battle& battle, Actor& person, const SkillDef& skill, bool autoPlay, bool verbose){
    auto allies = LivingSeats(battle, Side::ally);
    if(allies.empty()){
        return;
    }
    int seat = autoPlay ? allies.front() : 0;
    if(!autoPlay){
        int choice = 0;
        do{
            std::cout << "Choose an ally to support:\n";
            for(size_t j{0}; j < allies.size(); j++){
                std::cout << j + 1 << " : " << battle.actors[allies[j]].name << "\n";
            }
        }while(!ReadMenuInt(choice, 1, static_cast<int>(allies.size())));
        seat = allies[choice - 1];
    }
    int healAmount = skill.BasePow;
    battle.actors[seat].stats.CurrentHp += healAmount;
    if(battle.actors[seat].stats.CurrentHp > battle.actors[seat].stats.MaxHp){
        battle.actors[seat].stats.CurrentHp = battle.actors[seat].stats.MaxHp;
    }
    ApplyAttackUp(battle.actors[seat]);
    person.stats.CurrentFp -= skill.FpCost;
    if(verbose){
        std::cout << person.name << " used " << skill.Name << " on "
                  << battle.actors[seat].name << " and healed for " << healAmount << "\n";
    }
}

bool AllyUseSkill(Battle& battle, Actor& person,
                  std::unordered_map<int, SkillDef>& skills,
                  bool autoPlay, bool verbose){
    if(person.skillsIds.empty()){
        if(verbose){
            std::cout << "No skills available\n";
        }
        return false;
    }
    int skillId = person.skillsIds[0];
    if(!autoPlay){
        int choice = 0;
        do{
            std::cout << "Choose a skill:\n";
            for(size_t i{0}; i < person.skillsIds.size(); i++){
                const auto& s = skills[person.skillsIds[i]];
                std::cout << i + 1 << " : " << s.Name << " (FP " << s.FpCost << ")\n";
            }
        }while(!ReadMenuInt(choice, 1, static_cast<int>(person.skillsIds.size())));
        skillId = person.skillsIds[choice - 1];
    }
    const SkillDef& skill = skills[skillId];
    if(person.stats.CurrentFp < skill.FpCost){
        if(verbose){
            std::cout << "Not enough FP\n";
        }
        return false;
    }
    if(skill.target == Target::ally || skill.target == Target::self){
        AllySkillOnAllies(battle, person, skill, autoPlay, verbose);
    }
    else{
        AllySkillOnFoes(battle, person, skill, autoPlay, verbose);
    }
    return true;
}

bool AllyUseItem(Battle& battle, Actor& person,
                 std::unordered_map<std::string, Item>& items,
                 bool autoPlay, bool verbose){
    std::vector<std::string> names;
    for(const auto& kv: battle.itemsCount){
        if(kv.second > 0){
            names.push_back(kv.first);
        }
    }
    if(names.empty()){
        if(verbose){
            std::cout << "No items left\n";
        }
        return false;
    }
    std::string itemName = names[0];
    if(!autoPlay){
        int choice = 0;
        do{
            std::cout << "Choose an item:\n";
            for(size_t i{0}; i < names.size(); i++){
                std::cout << i + 1 << " : " << battle.itemsCount[names[i]] << " " << names[i] << "\n";
            }
        }while(!ReadMenuInt(choice, 1, static_cast<int>(names.size())));
        itemName = names[choice - 1];
    }
    if(battle.itemsCount[itemName] <= 0){
        return false;
    }
    battle.itemsCount[itemName]--;
    Item& def = items[itemName];
    if(def.effect == ItemEffect::heal){
        person.stats.CurrentHp += def.value;
        if(person.stats.CurrentHp > person.stats.MaxHp){
            person.stats.CurrentHp = person.stats.MaxHp;
        }
        if(verbose){
            std::cout << person.name << " used " << itemName << "\n";
        }
    }
    else if(def.effect == ItemEffect::restorefp){
        person.stats.CurrentFp += def.value;
        if(person.stats.CurrentFp > person.stats.Maxfp){
            person.stats.CurrentFp = person.stats.Maxfp;
        }
        if(verbose){
            std::cout << person.name << " used " << itemName << "\n";
        }
    }
    else if(def.effect == ItemEffect::curePoison){
        std::erase_if(person.statuses, [](const ActiveStatus& s){
            return s.status == Status::poison;
        });
        if(verbose){
            std::cout << person.name << " used " << itemName << "\n";
        }
    }
    else if(def.effect == ItemEffect::bomb){
        for(int seat: LivingSeats(battle, Side::foe)){
            battle.actors[seat].stats.CurrentHp -= def.value;
            if(battle.actors[seat].stats.CurrentHp < 0){
                battle.actors[seat].stats.CurrentHp = 0;
            }
            if(verbose){
                std::cout << person.name << " used " << itemName << " on "
                          << battle.actors[seat].name << " for " << def.value << "\n";
            }
        }
    }
    return true;
}


void RunAllyTurn(Battle& battle, Actor& person,
                 std::unordered_map<int, SkillDef>& skills,
                 std::unordered_map<std::string, Item>& items,
                 bool autoPlay,
                 bool verbose){
    person.defending = false;
    if(autoPlay){
        AllyBasicAttack(battle, person, true, verbose);
        return;
    }
    int options = 0;
    do{
        std::cout << "choose an action: 1) Attack 2) Skill 3) Item 4) Defend\n";
        if(!ReadMenuInt(options, 1, 4)){
            options = 0;
            continue;
        }
        if(options == 1){
            AllyBasicAttack(battle, person, false, verbose);
        }
        else if(options == 2){
            if(!AllyUseSkill(battle, person, skills, false, verbose)){
                options = 0;
            }
        }
        else if(options == 3){
            if(!AllyUseItem(battle, person, items, false, verbose)){
                options = 0;
            }
        }
        else if(options == 4){
            person.defending = true;
            if(verbose){
                std::cout << person.name << " defended\n";
            }
        }
    }while(options < 1 || options > 4);
}
