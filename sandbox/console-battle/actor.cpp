#include "actor.h"
#include <iostream>
#include <fstream>
#include "util.h"


std::vector<Actor> LoadActors(const std::string& path,Side side){
    std::vector<Actor> heroes;

    std::ifstream file(path);
    int linenumber{0};

    if(!file){
        std::cerr<< "Error, could not open file";
        return heroes;
    }

    std::string line;

    while(std::getline(file, line)){
        ++linenumber;
        if(line.empty()){
            continue;
        }

        auto parts = Split(line, ',');
        if(parts.size() != 8){
            std::cout << "error line number" <<linenumber;
            continue;
        }
        else{
            Actor e;
            e.BattleId = std::stoi(parts[0]);
            e.name = parts[1];
            e.stats.MaxHp = std::stoi(parts[2]);
            e.stats.Maxfp = std::stoi(parts[3]);
            e.stats.CurrentHp = e.stats.MaxHp;
            e.stats.CurrentFp = e.stats.Maxfp;
            e.stats.Attack = std::stoi(parts[4]);
            e.stats.Defense = std::stoi(parts[5]);
            e.stats.Spd = std::stoi(parts[6]);
            e.side = side;

            auto skillsparts = Split(parts[7], ';');
            for(size_t i{0}; i<skillsparts.size(); i++){
                e.skillsIds.push_back(stoi(skillsparts[i]));
            }

            heroes.push_back(e);
        }
    }

    return heroes;
}
