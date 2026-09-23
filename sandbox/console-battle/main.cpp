#include <cmath>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
//Enum class for storing the Elements
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
//This stores the targets
enum class Target{
    ally,
    enemy,
    self
};
enum class Status{
    poison,
    burn
};
enum class Side{
    ally,
    foe
};
//This is the Stats struct
struct Stats{
    int MaxHp{0};
    int Maxfp{0};
    int CurrentHp{0};
    int CurrentFp{0};
    int Attack{0};
    int Defense{0};
    int Spd{0};
};
//This is SkillDef, ID is used for checking with unordered and ordered maps
//Remember to use enum::variable, since with class you have to be more specific
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
    Side side{Side::ally}; //friend or foe
    std::vector<Status> statuses;
    bool defending{false};
    std::vector<int> skillsIds;
};
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
std::string BattleResultToString(BattleResult& battleresult){
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
    else{
        std::cerr << "Error target in enum does not exist, using default";
        return Target::enemy;
    }
}
std::vector<std::string> Split(const std::string& line, char delimeter){

    size_t i = 0;
    std::string current = "";
    std::vector<std::string> full = {};

    while(i < line.size()){
        //we build each letter or word, that is divided by its delimeter, if the delimeter appears we enter the if
        // when we reach the final word we dont have a delimeter and we wont add a new word, which means the while is fulfield
        //leaving current with the value of "friend"
        if(line[i] == delimeter){
            //here we start at index since the vector is empty, and we push the entire word up unitl the delimeter, in this case
            //"hello"
            full.push_back(current);
            //now full has {"hello", }, and current is empty
            current.clear();
            //now we go to next index
            i++;
        }
        //if we dont we keep adding new letter or number string
        else{
            current += line[i];
            i++;
        }
    }
    //full has {"hello", "my", "good"} and current has "friend", witht the pushback we isnert the final word
    //so now full is {"hello", "my", "good", "friend"}
    full.push_back(current);


    return full;
}

std::vector<Actor> LoadActors(const std::string& path,Side side){
    std::vector<Actor> heroes;

    std::ifstream file(path);
    int linenumber{0};

    if(!file){
        std::cerr<< "Error, could not open file";
        return heroes;
    }

    std::string line;

    while(std::getline(file, line)){
        ++linenumber;
        if(line.empty()){
            continue;
        }

        auto parts = Split(line, ',');
        if(parts.size() != 8){
            std::cout << "error line number" <<linenumber;
            continue;
        }
        else{
            Actor e;
            e.BattleId = std::stoi(parts[0]);
            e.name = parts[1];
            e.stats.MaxHp = std::stoi(parts[2]);
            e.stats.Maxfp = std::stoi(parts[3]);
            e.stats.CurrentHp = e.stats.MaxHp;
            e.stats.CurrentFp = e.stats.Maxfp;
            e.stats.Attack = std::stoi(parts[4]);
            e.stats.Defense = std::stoi(parts[5]);
            e.stats.Spd = std::stoi(parts[6]);
            e.side = side;

            auto skillsparts = Split(parts[7], ';');
            for(size_t i{0}; i<skillsparts.size(); i++){
                e.skillsIds.push_back(stoi(skillsparts[i]));
            }

            heroes.push_back(e);
        }
    }

    return heroes;
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

struct Battle{
    std::vector<Actor> actors;
};

void HpBar(int maxhp, int currenthp){
    char health{'#'};
    char minushealth{'-'};

    for(size_t i{0}; i<maxhp; i++){
        if(i < currenthp){
            std::cout << health;
        }
        else{
            std::cout << minushealth;
        }
    }

}

BattleResult CheckEnd(const Battle& battle){
        bool AlliesAlive{false};
        bool FoesAlive{false};

        for(const auto& actor: battle.actors){
            if(actor.stats.CurrentHp <= 0){
                continue;
            }
            if(actor.side == Side::ally){
                AlliesAlive = true;
            }
            if(actor.side == Side::foe){
                FoesAlive = true;
            }
        }

        if(!AlliesAlive){
            return BattleResult::defeat;
        }
        else if(!FoesAlive){
            return BattleResult::victory;
        }

        return BattleResult::ongoing;
}
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
        if( actors[a].stats.Spd > actors[b].stats.Spd){
            return true;  // a is faster -> a before b
        }
        else if(actors[a].stats.Spd < actors[b].stats.Spd){
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
int clamp(int value, int min, int max){
    if(value < min){
        return min;
    }
    else if(value > max){
        return max;
    }
    else{
        return value;
    }
}

int Roll(int damage){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(damage * 0.9, damage * 1.1);
    std::uniform_int_distribution<int> distCrit(1, 100);
    int critChance = distCrit(gen);
    int FinalDamage = clamp(dist(gen), 0, 99);
    if(critChance <= 5){
        std::cout << "Critical Hit!" << "\n";
        return FinalDamage * 1.5;
    }
    else{
        return FinalDamage;
    }
}

int ComputeDamage(const Actor& attacker, const Actor& defender){
    int Damage = attacker.stats.Attack * attacker.stats.Attack / (attacker.stats.Attack + defender.stats.Defense);
    return Roll(Damage);
}



int main(){

    std::string Skillspath = "sandbox/console-battle/skills.csv";
    std::string Heroespath = "sandbox/console-battle/heroes.csv";
    std::string Villainpath = "sandbox/console-battle/enemies.csv";
    std::unordered_map<int, SkillDef> skills = loadedSkill(Skillspath);
    std::vector<Actor> heroes = LoadActors(Heroespath, Side::ally);
    std::vector<Actor> villain = LoadActors(Villainpath, Side::foe);
    /*for(const auto& heroe: heroes){
        std::cout << heroe.name << " " << SideToString(heroe.side) <<"\n";
        for(size_t i{0}; i<heroe.skillsIds.size(); i++){
            std::cout << heroe.skillsIds[i] << "\n";
        }
    }
    for(const auto& enemy: villain){
        std::cout << enemy.name << " " << SideToString(enemy.side) <<"\n";
        for(size_t i{0}; i<enemy.skillsIds.size(); i++){
            std::cout << enemy.skillsIds[i] << "\n";
        }
    }
    for(const auto& i: skills){
        std::cout << i.first << " " << i.second.Name << "\n";
        }*/

    Battle battle;
    for(const auto& heroe: heroes){
        battle.actors.push_back(heroe);
    }
    for(const auto& enemy: villain){
        battle.actors.push_back(enemy);
    }

    int options{0};
    int round{1};
    while(CheckEnd(battle) == BattleResult::ongoing){
    auto orderTurn = TurnOrder(battle.actors);
    std::cout << "---- Round " <<round <<" ----" <<"\n";
        for(const auto& actor: orderTurn){
        auto& person = battle.actors[actor];
        std::cout << person.name << " " << "side: " <<SideToString(person.side) << "\n";
        std::cout << "Health: ";
        HpBar(person.stats.MaxHp, person.stats.CurrentHp);
        std::cout << " " << person.stats.MaxHp << "/" << person.stats.CurrentHp;
        std::cout<<"\n";
        std::cout <<"Fp: ";
        HpBar(person.stats.Maxfp, person.stats.CurrentFp);
        std::cout << " " << person.stats.Maxfp << "/" << person.stats.CurrentFp;
        std::cout << "\n";
         std::cout << "Status: ";
         if(person.statuses.empty()){
             std::cout << "None" << "\n";
         }
         else{
             for(size_t i{0}; i<person.statuses.size(); i++){

             }
         }
         if(person.stats.CurrentHp <= 0){
             continue;
         }else{
         if(person.side == Side::ally){
             person.defending = false;
             do{
             std::cout <<"choose an action: 1) Attack 2) Skill 3) Item 4) Defend" << "\n";
             std::cin >> options;
             if(std::cin.fail()) {
                 std::cout << "just numbers" <<"\n";
                 std::cin.clear();
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                 options = 0;
             }

             else if( options == 1){
                    int choice2 = 0;
                    std::vector<int> targets;
                    for(size_t i{0}; i<battle.actors.size(); i++){
                        if(battle.actors[i].side == Side::foe && battle.actors[i].stats.CurrentHp > 0){
                            targets.push_back(i);
                        }
                    }
                    if(targets.empty()){
                        std::cout << "No enemies left to attack" << "\n";
                        continue;
                    }
                 do{
                                 std::cout <<"choose an enemy: " << "\n";
                                for(size_t j{0}; j < targets.size(); j++){
                                    std::cout << j + 1 << " :" << battle.actors[targets[j]].name << "\n";

                                }
                                 std::cin >> choice2;
                                 if(std::cin.fail()) {
                                     std::cout << "just numbers" <<"\n";
                                     std::cin.clear();
                                     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                     choice2 = 0;
                         }
                     }while(choice2 < 1 || choice2 > targets.size());
                     int seat = targets[choice2 - 1];
                     int damage = ComputeDamage(person, battle.actors[seat]);
                     if(battle.actors[seat].defending){
                         damage /= 2;
                     }
                         battle.actors[seat].stats.CurrentHp -= damage;
                     if (battle.actors[seat].stats.CurrentHp <= 0) {

                         battle.actors[seat].stats.CurrentHp = 0;
                     }
                     std::cout << person.name << " attacks " << "for " << damage << battle.actors[seat].name << "\n";
             }
             else if (options == 2){
                 std::cout << "You Used a skill\n";
             }
             else if (options == 3){
                 std::cout << "You used an item\n";
             }
             else if (options == 4){
                 person.defending = true;
                 std::cout << person.name << " defended\n";
             }
             else{
                 std::cout << "Invalid option, try again" << "\n";
             }
         }while(options != 1 && options != 2 && options != 3 && options != 4);
         }
         else if(person.side == Side::foe){
             person.defending = false;
             std::vector <int> targets;
                for(size_t i{0}; i<battle.actors.size(); i++){
                    if(battle.actors[i].side == Side::ally && battle.actors[i].stats.CurrentHp > 0){
                        targets.push_back(i);
                    }
                }
                std::random_device rd;
                std::mt19937 gen(rd());
                if(targets.empty()){
                    continue;
                }
                std::uniform_int_distribution<int> dist(0, targets.size() - 1);
                int seat = targets[dist(gen)];
                int damage = ComputeDamage(person, battle.actors[seat]);
                if(battle.actors[seat].defending){
                    damage /= 2;
         }
                battle.actors[seat].stats.CurrentHp -= damage;
                if (battle.actors[seat].stats.CurrentHp <= 0) {

                    battle.actors[seat].stats.CurrentHp = 0;
                }
                std::cout << person.name << " attacks " << battle.actors[seat].name << " for " << damage << "\n";
         }
          std::cout << "\n" <<"\n";
        }
    round++;
    auto status = CheckEnd(battle);
    std::cout << BattleResultToString(status) << "\n";
        }
    }
    return 0;
}
