#include "actor.h"
#include "skill.h"
#include <iostream>

void Skill::execute(Actor& user, Actor& target, Kind skillkind){
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
        };

