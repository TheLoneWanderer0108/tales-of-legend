#pragma once
#include "battle.h"
#include <vector>

int LowestHpSeat(Battle& battle, const std::vector<int>& seats);
void DealBasicAttack(Battle& battle, Actor& person, int seat, bool verbose);
void ApplyPoison(Actor& target);
void ApplyAttackUp(Actor& target);
bool ReadMenuInt(int& out, int minV, int maxV);
