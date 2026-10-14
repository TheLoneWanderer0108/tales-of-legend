#include "item.h"
#include <iostream>
#include <fstream>
#include "util.h"

ItemEffect StringToItemEffect(std::string& effect){
    if(effect == "heal"){
        return ItemEffect::heal;
    }
    else if(effect == "restorefp"){
        return ItemEffect::restorefp;
    }
    else if(effect == "curePoison"){
        return ItemEffect::curePoison;
    }
    else if(effect == "bomb"){
        return ItemEffect::bomb;
    }
    else{
        std::cerr << "Error effect in enum does not exist, using default";
        return ItemEffect::heal;
    }
}

std::unordered_map<std::string, Item> LoadItems(const std::string& path){
    std::unordered_map<std::string, Item> items;

    std::ifstream file(path);
    int linenumber{0};

    if(!file){
        std::cerr<< "Error, could not open file";
        return items;
    }

    std::string line;

    while(std::getline(file, line)){
        ++linenumber;
        if(line.empty()){
            continue;
        }

        auto parts = Split(line, ',');
        if(parts.size() != 3){
            std::cout << "error line number" <<linenumber;
            continue;
        }
        else{
            Item e;
            e.name = parts[0];
            e.effect = StringToItemEffect(parts[1]);
            e.value = std::stoi(parts[2]);

             items.insert({e.name, e});
        }
    }

   return items;
}
