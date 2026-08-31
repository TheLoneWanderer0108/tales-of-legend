#pragma once
#include <string>
#include "stats.h"
#include <vector>

struct Actor{
    std::string name{' '};
    Stats stats;
};
std::vector<int> TurnOrder(const std::vector<Actor>& actors);