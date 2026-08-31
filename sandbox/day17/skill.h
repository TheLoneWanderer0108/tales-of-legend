#pragma once

struct Actor;

enum ElementType{
    earth,
    fire,
    water,
    wind
};
enum Target{
    ally,
    enemy
};

enum Kind{
    damage,
    heal,
    basic
};

struct Skill{

    int power{0};
    ElementType type{fire};
    int cost{0};
    Target target{enemy};
    Kind kind{damage};

    void execute(Actor& user, Actor& target, Kind skillkind);

};