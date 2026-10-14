#include "battle.h"
#include "text.h"
#include <iostream>

void PrintActorBoardLine(const Actor& person){
    std::cout << person.name << " side: " << SideToString(person.side) << "\n";
    std::cout << "Health: ";
    HpBar(person.stats.MaxHp, person.stats.CurrentHp);
    std::cout << " " << person.stats.CurrentHp << "/" << person.stats.MaxHp << "\n";
    std::cout << "Fp: ";
    HpBar(person.stats.Maxfp, person.stats.CurrentFp);
    std::cout << " " << person.stats.CurrentFp << "/" << person.stats.Maxfp << "\n";
    std::cout << "Status: ";
    if(person.statuses.empty()){
        std::cout << "None\n";
        return;
    }
    for(const auto& s: person.statuses){
        std::cout << StatusToString(s.status) << " (" << s.duration << ") ";
    }
    std::cout << "\n";
}

void SetupBattleInventory(Battle& battle, const std::unordered_map<std::string, Item>& items, int countEach){
    battle.itemsCount.clear();
    for(const auto& item: items){
        battle.itemsCount.insert({item.first, countEach});
    }
}

void FillBattleActors(Battle& battle, const std::vector<Actor>& heroes, const std::vector<Actor>& foes){
    battle.actors.clear();
    for(const auto& h: heroes){
        battle.actors.push_back(h);
    }
    for(const auto& f: foes){
        battle.actors.push_back(f);
    }
}

BattleResult RunBattle(Battle& battle,
                       std::unordered_map<int, SkillDef>& skills,
                       std::unordered_map<std::string, Item>& items,
                       bool autoPlay,
                       bool verbose){
    battle.phase = Phase::StartBattle;
    battle.round = 1;

    while(CheckEnd(battle) == BattleResult::ongoing){
        battle.phase = Phase::RoundStart;
        if(verbose){
            std::cout << "---- Round " << battle.round << " ----\n";
        }

        battle.phase = Phase::DecideTurnOrder;
        auto order = TurnOrder(battle.actors);

        battle.phase = Phase::ActorTurn;
        for(int seat: order){
            Actor& person = battle.actors[seat];
            if(verbose){
                PrintActorBoardLine(person);
            }
            if(person.stats.CurrentHp <= 0){
                continue;
            }
            if(person.side == Side::ally){
                RunAllyTurn(battle, person, skills, items, autoPlay, verbose);
            }
            else if(person.side == Side::foe){
                RunFoeTurn(battle, person, skills, verbose);
            }
            if(verbose){
                std::cout << "\n\n";
            }
            if(CheckEnd(battle) != BattleResult::ongoing){
                break;
            }
        }

        battle.phase = Phase::RoundEnd;
        ApplyEndOfRoundStatuses(battle, verbose);

        battle.phase = Phase::CheckEnd;
        auto status = CheckEnd(battle);
        if(verbose){
            std::cout << BattleResultToString(status) << "\n";
        }
        battle.round++;
    }
    return CheckEnd(battle);
}
