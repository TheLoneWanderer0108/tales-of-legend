#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

enum class ElementType{
    fire,
    water,
    earth,
    wind,
    normal
};

enum class BattleResult{
    ongoing,
    victory,
    defeat
};

enum class Target{
    ally,
    enemy,
    self,
    allenemies,
};

enum class Status{
    poison,
    burn,
    attackup,
};

enum class Side{
    ally,
    foe
};

enum class ItemEffect{
    heal,
    restorefp,
    curePoison,
    bomb
};

enum class Phase{
    StartBattle,
    RoundStart,
    DecideTurnOrder,
    ActorTurn,
    RoundEnd,
    CheckEnd
};

struct Item{
    std::string name{};
    ItemEffect effect{};
    int value{0};
};

struct Stats{
    int MaxHp{0};
    int Maxfp{0};
    int CurrentHp{0};
    int CurrentFp{0};
    int Attack{0};
    int Defense{0};
    int Spd{0};
};

struct ActiveStatus{
    Status status;
    int duration;
};

struct SkillDef{
    int ID;
    std::string Name {};
    int BasePow{0};
    ElementType type{ElementType::normal};
    int FpCost{0};
    Target target{Target::enemy};
};

struct Actor{
    int BattleId{0};
    std::string name{};
    Stats stats;
    Side side{Side::ally};
    std::vector<ActiveStatus> statuses;
    bool defending{false};
    std::vector<int> skillsIds;
};

class Battle{
public:
    std::vector<Actor> actors;
    std::mt19937 rng{std::random_device{}()};
    std::unordered_map<std::string, int> itemsCount;
    Phase phase{Phase::StartBattle};
    int round{1};
};
