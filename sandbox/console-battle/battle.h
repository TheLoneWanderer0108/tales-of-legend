#pragma once
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "types.h"

int clamp(int value, int min, int max);
int Roll(int damage, std::mt19937& gen);
int ComputeDamage(const Actor& attacker, const Actor& defender, std::mt19937& gen);
int ComputeSkillDamage(const Actor& attacker, const Actor& defender, const SkillDef& skill, std::mt19937& gen);
BattleResult CheckEnd(const Battle& battle);
std::vector<int> TurnOrder(const std::vector<Actor>& actors);
void HpBar(int maxhp, int currenthp);

std::vector<int> LivingSeats(const Battle& battle, Side side);
void ApplyDamageToSeat(Battle& battle, int seat, int damage, bool verbose);
void ApplyEndOfRoundStatuses(Battle& battle, bool verbose);

void RunFoeTurn(Battle& battle, Actor& person, const std::unordered_map<int, SkillDef>& skills, bool verbose);
void RunAllyTurn(Battle& battle, Actor& person,
                 std::unordered_map<int, SkillDef>& skills,
                 std::unordered_map<std::string, Item>& items,
                 bool autoPlay,
                 bool verbose);

void PrintActorBoardLine(const Actor& person);
BattleResult RunBattle(Battle& battle,
                       std::unordered_map<int, SkillDef>& skills,
                       std::unordered_map<std::string, Item>& items,
                       bool autoPlay,
                       bool verbose);

void SetupBattleInventory(Battle& battle, const std::unordered_map<std::string, Item>& items, int countEach);
void FillBattleActors(Battle& battle, const std::vector<Actor>& heroes, const std::vector<Actor>& foes);
