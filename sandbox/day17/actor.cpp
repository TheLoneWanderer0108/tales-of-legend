#include <iostream>
#include <vector>
#include "actor.h"
#include <algorithm>


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