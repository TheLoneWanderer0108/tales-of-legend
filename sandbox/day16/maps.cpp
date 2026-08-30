#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <map>

/*Build `std::unordered_map<std::string, Stats>` as an enemy database keyed by name.
      Look up `"Goblin"`. Look up `"Grblin"` with `operator[]` and notice it silently
      creates an empty entry - then fix it with `find` or `contains`*/
/*Build an inventory as `std::unordered_map<std::string, int>` (item name to count).
      Write `Add`, `Remove` (removing the key at zero), and `Count`*/
/*Sort a party by SPD descending with `std::sort` and a lambda comparator. This is
      literally your battle turn order*/

struct Stats{
    int hp{100};
    int maxHP{100};
    int att{1};
    int def{1};
    int maxfp{10};
    int fp{10};
    int spd{1};
};
struct Actor{
    std::string name{' '};
    Stats stats;
};

void add(std::unordered_map<std::string, int>& items, std::string name){
    items[name] += 1;

}
void remove(std::unordered_map<std::string, int>& items, std::string name){
    auto result = items.find(name);
    if(result != items.end()){
        result ->second -= 1;
        if(result ->second == 0){
            items.erase(result);
        }
    }
    else{
        std::cout << "item not found";
    }

}
int count(std::unordered_map<std::string, int>& items, std::string name){

    auto result = items.find(name);

    if (result != items.end()){
        return result ->second;
    }
    else{
        std::cout << "Item not found";
    }

    return 0;

}

// TurnOrder: given a party (vector of actors), return WHO ACTS FIRST as slot numbers.
// We do NOT reorder `actors` — the party stays at fixed indices 0, 1, 2, ...
// We only reorder a separate list of indices so other code can still say "Joe is party[0]".
std::vector<int> TurnOrder(const std::vector<Actor>& actors){
    // `Order` starts empty. It will become something like {1, 2, 0}:
    // "index 1 goes first, then index 2, then index 0".
    std::vector <int> Order;

    // Build the initial index list: {0, 1, 2, ...} — one entry per party member.
    // `i` walks party POSITIONS (0, 1, 2). We push `i` itself, NOT spd or the Actor.
    // Example party size 3: after this loop Order == {0, 1, 2} (no sorting yet).
    for(size_t i{0}; i<actors.size(); i++){
        Order.push_back(i);
    }

    // std::sort shuffles the numbers INSIDE `Order` using our rules.
    // `a` and `b` are NOT actors — they are two indices currently being compared.
    // [&actors] lets the lambda READ the party to look up each index's spd.
    std::sort(Order.begin(), Order.end(), [&actors](int a, int b){
        // Question sort asks: "should index `a` appear before index `b` in the final list?"
        // return true  -> yes, `a` goes earlier (acts sooner)
        // return false -> no, `b` goes earlier

        // Rule 1: higher spd acts first (descending speed).
        // actors[a] = whoever sits at slot `a` in the ORIGINAL party (party never moves).
        if( actors[a].stats.spd > actors[b].stats.spd){
            return true;  // a is faster -> a before b
        }
        else if(actors[a].stats.spd < actors[b].stats.spd){
            return false; // b is faster -> b before a
        }
        else{
            // Rule 2 (tie): same spd -> lower index wins so order is always the same
            // (deterministic — important for debugging "who went first on turn 3?").
            return a < b; // e.g. 0 before 1 when both have spd 5
        }

    });

    // `Order` now holds party indices in turn sequence. Caller uses party[Order[0]] for first actor.
    return Order;


}

int main(){

    std::unordered_map<std::string, int> items = {
        {"Potion", 2}, {"Ether", 4}
    };
    std::map<std::string, int> items2 = {
        {"Potion", 2}, {"Ether", 4}
    };

    for(auto i{items.begin()}; i != items.end(); i++){
        std::cout << i->first << ": " << i->second << "\n";
    }

    for(auto i{items2.begin()}; i != items2.end(); i++){
        std::cout << i->first << ": " << i->second << "\n";
    }


    std::cout << "Original: " << "\n";

    for(auto i{items.begin()}; i != items.end(); i++){
        std::cout << i->first << ": " << i->second << "\n";
    }

    add(items, "Potion");
    add(items, "Sword");

    std::cout << "After adding :" << "\n";

    for(auto i{items.begin()}; i != items.end(); i++){
        std::cout << i->first << ": " << i->second << "\n";
    }

    remove(items, "Sword");
    remove(items, "Potion");

    std::cout << "After taking :" << "\n";

    for(auto i{items.begin()}; i != items.end(); i++){
        std::cout << i->first << ": " <<  i->second << "\n";
    }

    int countItem = count(items, "Potion");

    std::cout << countItem << "\n";

    // Fixed roster: index 0 = Joe, 1 = Martha, 2 = Bertrude (these slots never change).
    std::vector<Actor> party {
        {"Joe", {100,100,1,1,1,1,2}},       // party[0], spd 2
        {"Martha", {100, 100, 2, 2, 2, 2, 5}}, // party[1], spd 5  -> should act first
        {"Bertrude", {150, 100, 2, 2, 3, 4, 3}} // party[2], spd 3  -> second
    };

    // Turnorder is the "sticky note" from TurnOrder: e.g. {1, 2, 0} means Martha, Bertrude, Joe.
    std::vector<int> Turnorder = TurnOrder(party);

    // `i` here is turn NUMBER (0 = first turn, 1 = second turn), NOT a party index.
    // Turnorder[i] IS the party index for that turn — use it to look up the actor.
    for (size_t i{0}; i<Turnorder.size(); i++){
        std::cout << Turnorder[i] << " " << party[Turnorder[i]].name << "\n";
    }


    /*std::unordered_map<std::string, Stats> actor ={
        {"Goblin", {200, 200, 1, 1, 2, 1, 1}},
        {"Troll", {100, 100, 1, 1, 2, 2, 2}}
    };
    auto result = actor.find("Goblin");

    if (result != actor.end()){
        std::cout << result -> second.hp;
    }
    else{
        std::cout << "Key not found";
    }*/

    return 0;
}