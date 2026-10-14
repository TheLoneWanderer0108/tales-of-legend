#include "battle.h"
#include <algorithm>
#include <iostream>
#include <random>

int clamp(int value, int min, int max){
    if(value < min){
        return min;
    }
    if(value > max){
        return max;
    }
    return value;
}

int Roll(int damage, std::mt19937& gen){
    int low = static_cast<int>(damage * 0.9);
    int high = static_cast<int>(damage * 1.1);
    if(high < low){
        high = low;
    }
    std::uniform_int_distribution<int> dist(low, high);
    std::uniform_int_distribution<int> distCrit(1, 100);
    int critChance = distCrit(gen);
    int finalDamage = clamp(dist(gen), 0, 99);
    if(critChance <= 5){
        return static_cast<int>(finalDamage * 1.5);
    }
    return finalDamage;
}

int ComputeDamage(const Actor& attacker, const Actor& defender, std::mt19937& gen){
    float mult = 1.0f;
    for(const auto& status: attacker.statuses){
        if(status.status == Status::attackup){
            mult = 1.5f;
            break;
        }
    }
    int atk = attacker.stats.Attack;
    int def = defender.stats.Defense;
    int damage = static_cast<int>(mult * atk * atk / (atk + def));
    return Roll(damage, gen);
}

int ComputeSkillDamage(const Actor& attacker, const Actor& defender, const SkillDef& skill, std::mt19937& gen){
    float mult = 1.0f;
    for(const auto& status: attacker.statuses){
        if(status.status == Status::attackup){
            mult = 1.5f;
            break;
        }
    }
    int atk = attacker.stats.Attack;
    int def = defender.stats.Defense;
    int damage = static_cast<int>(mult * skill.BasePow * atk / (atk + def));
    return Roll(damage, gen);
}

BattleResult CheckEnd(const Battle& battle){
    bool alliesAlive = false;
    bool foesAlive = false;
    for(const auto& actor: battle.actors){
        if(actor.stats.CurrentHp <= 0){
            continue;
        }
        if(actor.side == Side::ally){
            alliesAlive = true;
        }
        if(actor.side == Side::foe){
            foesAlive = true;
        }
    }
    if(!alliesAlive){
        return BattleResult::defeat;
    }
    if(!foesAlive){
        return BattleResult::victory;
    }
    return BattleResult::ongoing;
}

std::vector<int> TurnOrder(const std::vector<Actor>& actors){
    std::vector<int> order;
    for(size_t i{0}; i < actors.size(); i++){
        order.push_back(static_cast<int>(i));
    }
    std::sort(order.begin(), order.end(), [&actors](int a, int b){
        if(actors[a].stats.Spd != actors[b].stats.Spd){
            return actors[a].stats.Spd > actors[b].stats.Spd;
        }
        return a < b;
    });
    return order;
}

void HpBar(int maxhp, int currenthp){
    for(int i{0}; i < maxhp; i++){
        std::cout << (i < currenthp ? '#' : '-');
    }
}

std::vector<int> LivingSeats(const Battle& battle, Side side){
    std::vector<int> seats;
    for(size_t i{0}; i < battle.actors.size(); i++){
        if(battle.actors[i].side == side && battle.actors[i].stats.CurrentHp > 0){
            seats.push_back(static_cast<int>(i));
        }
    }
    return seats;
}

void ApplyDamageToSeat(Battle& battle, int seat, int damage, bool verbose){
    if(battle.actors[seat].defending){
        damage /= 2;
    }
    battle.actors[seat].stats.CurrentHp -= damage;
    if(battle.actors[seat].stats.CurrentHp < 0){
        battle.actors[seat].stats.CurrentHp = 0;
    }
    if(verbose){
        // caller prints the combat line
    }
    (void)verbose;
}

void ApplyEndOfRoundStatuses(Battle& battle, bool verbose){
    for(auto& actor: battle.actors){
        for(size_t i{0}; i < actor.statuses.size(); i++){
            if(actor.statuses[i].status == Status::attackup){
                if(verbose){
                    std::cout << actor.name << "'s attack is boosted\n";
                }
            }
            else if(actor.statuses[i].status == Status::poison ||
                    actor.statuses[i].status == Status::burn){
                actor.stats.CurrentHp -= 5;
                if(actor.stats.CurrentHp < 0){
                    actor.stats.CurrentHp = 0;
                }
                if(verbose){
                    std::cout << actor.name << " takes 5 status damage\n";
                }
            }
            actor.statuses[i].duration--;
            if(actor.statuses[i].duration <= 0){
                if(verbose){
                    std::cout << actor.name << " status expired\n";
                }
                actor.statuses.erase(actor.statuses.begin() + static_cast<long>(i));
                i--;
            }
        }
    }
}
