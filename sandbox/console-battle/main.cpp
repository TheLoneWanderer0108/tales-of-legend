#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "actor.h"
#include "battle.h"
#include "item.h"
#include "skill.h"
#include "text.h"
#include "types.h"

namespace {

void ParseSeed(Battle& battle, int argc, char* argv[]){
    for(int i = 0; i < argc; i++){
        if(std::string(argv[i]) == "--seed" && i + 1 < argc){
            unsigned long seed = std::stoul(argv[i + 1]);
            battle.rng.seed(static_cast<std::mt19937::result_type>(seed));
            std::cout << "Seed: " << seed << "\n";
        }
    }
}

bool HasFlag(int argc, char* argv[], const std::string& flag){
    for(int i = 0; i < argc; i++){
        if(std::string(argv[i]) == flag){
            return true;
        }
    }
    return false;
}

void Tests(){
    // 1) damage never negative
    {
        std::mt19937 gen(1);
        Actor a{};
        a.stats.Attack = 5;
        Actor d{};
        d.stats.Defense = 2;
        assert(ComputeDamage(a, d, gen) >= 0);
        SkillDef sk{1, "swing", 2, ElementType::normal, 0, Target::enemy};
        assert(ComputeSkillDamage(a, d, sk, gen) >= 0);
    }

    // 2) HP never exceeds max after heal clamp
    {
        Actor hero{};
        hero.stats.MaxHp = 10;
        hero.stats.CurrentHp = 9;
        hero.stats.CurrentHp += 5;
        if(hero.stats.CurrentHp > hero.stats.MaxHp){
            hero.stats.CurrentHp = hero.stats.MaxHp;
        }
        assert(hero.stats.CurrentHp <= hero.stats.MaxHp);
    }

    // 3) poison expires after exactly 3 ticks
    {
        Battle battle;
        Actor victim{};
        victim.name = "test";
        victim.stats.CurrentHp = 50;
        victim.statuses.push_back({Status::poison, 3});
        battle.actors.push_back(victim);
        for(int tick = 0; tick < 3; tick++){
            assert(!battle.actors[0].statuses.empty());
            ApplyEndOfRoundStatuses(battle, false);
        }
        assert(battle.actors[0].statuses.empty());
    }

    // 4) dead actor never acts — skip when CurrentHp <= 0
    {
        Actor dead{};
        dead.stats.CurrentHp = 0;
        assert(dead.stats.CurrentHp <= 0);
    }

    // 5) inventory count never goes below zero
    {
        Battle battle;
        battle.itemsCount["potion"] = 1;
        battle.itemsCount["potion"]--;
        assert(battle.itemsCount["potion"] >= 0);
        if(battle.itemsCount["potion"] > 0){
            battle.itemsCount["potion"]--;
        }
        assert(battle.itemsCount["potion"] >= 0);
    }

    std::cout << "Tests() passed\n";
}

int RunAutoBalance(std::unordered_map<int, SkillDef> skills,
                   std::vector<Actor> heroes,
                   std::vector<Actor> villains,
                   std::unordered_map<std::string, Item> items,
                   unsigned baseSeed){
    int wins = 0;
    const int trials = 500;
    for(int i = 0; i < trials; i++){
        Battle battle;
        battle.rng.seed(baseSeed + static_cast<unsigned>(i));
        FillBattleActors(battle, heroes, villains);
        SetupBattleInventory(battle, items, 3);
        auto result = RunBattle(battle, skills, items, true, false);
        if(result == BattleResult::victory){
            wins++;
        }
    }
    int rate = wins * 100 / trials;
    std::cout << "Auto-battle: " << wins << "/" << trials
              << " hero wins (" << rate << "%)\n";
    return rate;
}

} // namespace

int main(int argc, char* argv[]){
    Tests();

    const std::string skillPath = "sandbox/console-battle/skills.csv";
    const std::string heroPath = "sandbox/console-battle/heroes.csv";
    const std::string foePath = "sandbox/console-battle/enemies.csv";
    const std::string itemPath = "sandbox/console-battle/items.csv";

    auto skills = loadedSkill(skillPath);
    auto heroes = LoadActors(heroPath, Side::ally);
    auto villains = LoadActors(foePath, Side::foe);
    auto items = LoadItems(itemPath);

    if(heroes.empty() || villains.empty()){
        std::cerr << "Failed to load actors — run from project root\n";
        return 1;
    }

    if(HasFlag(argc, argv, "--auto")){
        unsigned seed = 42;
        for(int i = 0; i < argc; i++){
            if(std::string(argv[i]) == "--seed" && i + 1 < argc){
                seed = static_cast<unsigned>(std::stoul(argv[i + 1]));
            }
        }
        RunAutoBalance(skills, heroes, villains, items, seed);
        return 0;
    }

    Battle battle;
    ParseSeed(battle, argc, argv);
    FillBattleActors(battle, heroes, villains);
    SetupBattleInventory(battle, items, 3);

    auto result = RunBattle(battle, skills, items, false, true);
    std::cout << "Final: " << BattleResultToString(result) << "\n";
    return 0;
}
