#include "text.h"
#include <iostream>

std::string SideToString(Side side){
    if(side == Side::ally){
        return "ally";
    }
    else if(side == Side::foe){
        return "foe";
    }
    else{
        std::cerr << "Error detecting side";
        return "ally";
    }
}
std::string StatusToString(Status status){
    if(status == Status::poison){
        return "poison";
    }
    else if(status == Status::burn){
        return "burn";
    }
    else if(status == Status::attackup){
        return "attackup";
    }
    else{
        std::cerr << "Error detecting status";
        return "none";
    }
}
std::string BattleResultToString(BattleResult battleresult){
    if(battleresult == BattleResult::victory){
        return "Victory!";
    }
    else if(battleresult == BattleResult::defeat){
        return "Defeat!";
    }
    else if(battleresult == BattleResult::ongoing){
        return "The fight continues!!!!!";
    }
    else{
        return "Not valid";
    }
}
