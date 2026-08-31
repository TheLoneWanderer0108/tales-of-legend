#include <iostream>
#include "actor.h"
#include <vector>

int main(){

     std::vector<Actor> party {
        {"Joe", {100,100,1,1,1,1,2}},       // party[0], spd 2
        {"Martha", {100, 100, 2, 2, 2, 2, 5}}, // party[1], spd 5  -> should act first
        {"Bertrude", {150, 100, 2, 2, 3, 4, 3}} // party[2], spd 3  -> second
    };
    std::vector<int> Turnorder = TurnOrder(party);

    // `i` here is turn NUMBER (0 = first turn, 1 = second turn), NOT a party index.
    // Turnorder[i] IS the party index for that turn — use it to look up the actor.
    for (size_t i{0}; i<Turnorder.size(); i++){
        std::cout << Turnorder[i] << " " << party[Turnorder[i]].name << "\n";
    }
    return 0;
}


void hello(){

    std::cout << "hello";
}