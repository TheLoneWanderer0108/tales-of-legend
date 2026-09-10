#include <iostream>
#include <optional>
#include <string>
#include <stdexcept>
#include <variant>

/*Use `std::variant` for something genuinely alternative, e.g. an item effect that is
      either a heal amount, a status cure, or a stat buff. Visit it with
      `std::visit` or `std::get_if`*/

std::optional<int> TakeText(const std::string& text){
    try{
        int num = stoi(text);
        return num;
    }catch(const std::invalid_argument&){
        return std::nullopt;
    }
}

enum class Element{
    fire,
    earth,
    wind,
    water
};

enum class TargetType{
    ally,
    self,
    enemy
};

enum class BattleResult{
    win,
    lose
};

enum class StatusEffects{
    burn,
    poison
};

enum class Command{
    attack,
    defend,
    run,
    quit
};

struct Buff
{
    int StrenBuff{0};
    int MagicBuff{0};
};



int main(){

    std::variant<int, StatusEffects, Buff> effect;
    const Buff buff {5, 2};

    effect = 50;
    
    effect = StatusEffects::poison;
    effect = buff;

    if(auto intpart = std::get_if<int>(&effect)){
        std::cout << "You have healed for: " <<*intpart;
    }
    if(auto Realeffect = std::get_if<StatusEffects>(&effect)){
        if(*Realeffect == StatusEffects::poison){
        std::cout << "You have been posioned";
        }
    }
    if(auto buff = std::get_if<Buff>(&effect)){
        std::cout << "You have a strength buff of: " << buff->StrenBuff << " And magic buff of: " << buff->MagicBuff;
    }

    int choice {0};
    int target{2};
    BattleResult battleRes {BattleResult::win};
    TargetType Realtarget { static_cast<TargetType> (target)};
    int element{0}; 
    Element RealElem {static_cast<Element> (element)};
    int status{0};
    StatusEffects RealStatus{static_cast<StatusEffects> (status)};

    std::cout << "Give me your choice: 1) attack, 2)defend, 3)run, 4)quit" << "\n";
    std::cin >> choice;
    Command command{ static_cast<Command>(choice - 1) };

    switch (command)
    {
    case Command::attack:
        std::cout << "You have attacked";
        if(RealElem == Element::fire){
            std::cout << "You sent fire" << "\n";
            if(RealStatus == StatusEffects::burn){
                std::cout << "You have been burned" << "\n";
            }
            else{
                std::cout << "You are not burned";
            }
        }
        if(battleRes == BattleResult::win){
            std::cout << "You won"; 
        }
        else if (battleRes == BattleResult::lose){
            std::cout << "You lost";
        }
        if(Realtarget == TargetType::enemy){
            std::cout << "You have hit an enemy";
        }

        break;
    case Command::defend:
        std::cout << "You have defended";
        break;
    case Command::run:
        std::cout << "You have ran";
        break;
    case Command::quit:
        std::cout << "You have quit";
        break;
    default:
        std::cout << "Not valid";
        break;
    }

    if(auto n = TakeText("42")){
        std::cout << n.value();
    }
    else{
        std::cout << "Not valid";
    }

    return 0;
}