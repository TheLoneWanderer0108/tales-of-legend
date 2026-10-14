#include "skill.h"
#include <iostream>
#include <fstream>
#include "util.h"


ElementType StringToElement(std::string& element){
    if(element == "fire"){
        return ElementType::fire;
    }
    else if(element == "water"){
        return ElementType::water;
    }
    else if(element == "earth"){
        return ElementType::earth;
    }
    else if(element == "wind"){
        return ElementType::wind;
    }
    else if(element == "normal"){
        return ElementType::normal;
    }
    else{
        std::cerr <<"Element does not exist, using default";
        return ElementType::normal;
    }
};

Target StringToTarget(std::string& target){
    if(target == "enemy"){
        return Target::enemy;
    }
    else if(target == "ally"){
        return Target::ally;
    }
    else if(target == "self") {
        return Target::self;
    }
    else if(target == "allenemies"){
        return Target::allenemies;
    }
    else{
        std::cerr << "Error target in enum does not exist, using default";
        return Target::enemy;
    }
}
//this function loads the skills in an unordered map to be used later
std::unordered_map<int, SkillDef> loadedSkill(const std::string& path){
    std::unordered_map<int, SkillDef> skills;
    std::ifstream file(path);
    int linenumber{0};
    if(!file){
        std::cerr<<"Could not opened the file";
        return skills;
    }
    //// Read file line by line
    std::string line;
    //this takes 3 parameters, the output its reading from, the line, and the delimeter
    // since its empty it defaults to \n
    while(std::getline(file, line)){
        ++linenumber;
        if(line.empty()){
            continue;
        }
        auto parts = Split(line, ',');
        if (parts.size() != 6){
            std::cout << "Error in line: " <<linenumber;
            continue;
        }else{
            SkillDef e;
            //this stores the skillDef in their respected variables
            e.ID = std::stoi(parts[0]);
            e.Name = parts[1];
            e.BasePow = std::stoi(parts[2]);
            e.type = StringToElement(parts[3]);
            e.FpCost = std::stoi(parts[4]);
            e.target = StringToTarget(parts[5]);
            //after that we store the entire e under their respected key
            skills[e.ID] = e;
        }
    }
    file.close();

    return skills;
}
