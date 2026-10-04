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
    restorefp
};

//This is a struct for the items, it has a name, type, effect and value
struct Item{
    std::string name{};
    ItemEffect effect{};
    int value{0};
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
struct ActiveStatus{
    Status status;
    int duration;
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
    std::vector<ActiveStatus> statuses;
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
    else if(target == "allenemies"){
        return Target::allenemies;
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

ItemEffect StringToItemEffect(std::string& effect){
    if(effect == "heal"){
        return ItemEffect::heal;
    }
    else if(effect == "restorefp"){
        return ItemEffect::restorefp;
    }
    else{
        std::cerr << "Error effect in enum does not exist, using default";
        return ItemEffect::heal;
    }
}



std::unordered_map<std::string, Item> LoadItems(const std::string& path){
    std::unordered_map<std::string, Item> items;

    std::ifstream file(path);
    int linenumber{0};

    if(!file){
        std::cerr<< "Error, could not open file";
        return items;
    }

    std::string line;

    while(std::getline(file, line)){
        ++linenumber;
        if(line.empty()){
            continue;
        }

        auto parts = Split(line, ',');
        if(parts.size() != 3){
            std::cout << "error line number" <<linenumber;
            continue;
        }
        else{
            Item e;
            e.name = parts[0];
            e.effect = StringToItemEffect(parts[1]);
            e.value = std::stoi(parts[2]);

             items.insert({e.name, e});
        }
    }

   return items;
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
    std::unordered_map<std::string, int> itemsCount;
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
    for(const auto& status: attacker.statuses){
        if(status.status == Status::attackup){
           float extraDam = 1.5;
           int Damage = extraDam * attacker.stats.Attack * attacker.stats.Attack / (attacker.stats.Attack + defender.stats.Defense);
           return Roll(Damage);
        }else{
            continue;
        }
    }
    int Damage = attacker.stats.Attack * attacker.stats.Attack / (attacker.stats.Attack + defender.stats.Defense);
    return Roll(Damage);
}

int ComputeSkillDamage(const Actor& attacker, const Actor& defender, const SkillDef& skill){
for (const auto& status : attacker.statuses) {
        if (status.status == Status::attackup) {
            float extraDam = 1.5;
            int Damage = extraDam * skill.BasePow * attacker.stats.Attack / (attacker.stats.Attack + defender.stats.Defense);
            return Roll(Damage);
        } else {
            continue;
        }
    }
    int Damage = skill.BasePow * attacker.stats.Attack / (attacker.stats.Attack + defender.stats.Defense);
    return Roll(Damage);
}

int main(){

    std::string Skillspath = "sandbox/console-battle/skills.csv";
    std::string Heroespath = "sandbox/console-battle/heroes.csv";
    std::string Villainpath = "sandbox/console-battle/enemies.csv";
    std::unordered_map<int, SkillDef> skills = loadedSkill(Skillspath);
    std::vector<Actor> heroes = LoadActors(Heroespath, Side::ally);
    std::vector<Actor> villain = LoadActors(Villainpath, Side::foe);
    std::unordered_map<std::string, Item> items = LoadItems("sandbox/console-battle/items.csv");
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
    // this is for the items, we insert the items into the battle.items unordered map
    // items.first is the key, items.second is the value, which is the entire struct
    // we insert 3 of each item into the battle.itemsCount unordered map, which is a map of string and int
     for(const auto& item: items){
       battle.itemsCount.insert({item.first, 3});
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
                    std::cout << StatusToString(person.statuses[i].status) << " (Duration: " << person.statuses[i].duration << ") ";
             }
             std::cout << "\n";
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
                 int choice4 = 0;
                 std::vector<int> skillIds = person.skillsIds;
                 if(skillIds.empty()){
                     std::cout << "No skills available" << "\n";
                     options = 0;
                 }else{
                     do{
                         std::cout << "Choose a skill: " << "\n";
                         for(size_t i{0}; i<skillIds.size(); i++){
                             std::cout << i + 1 << " : " << skills[skillIds[i]].Name << " (FP Cost: " << skills[skillIds[i]].FpCost << ")" << "\n";
                         }
                         std::cin >> choice4;
                         if(std::cin.fail()) {
                             std::cout << "just numbers" <<"\n";
                             std::cin.clear();
                             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                             choice4 = 0;
                         }
                     }while (choice4 < 1 || choice4 > skillIds.size());
                     int skillId = skillIds[choice4 - 1];
                     if(person.stats.CurrentFp < skills[skillId].FpCost){
                         std::cout << "Not enough FP" << "\n";
                         options = 0;
                     }
                     else if(skills[skillId].target == Target::allenemies){
                         std::vector<int> targets;
                         for(size_t i{0}; i<battle.actors.size(); i++){
                             if(battle.actors[i].side == Side::foe && battle.actors[i].stats.CurrentHp > 0){
                                 targets.push_back(i);
                             }
                         }
                         if(targets.empty()){
                             std::cout << "No enemies left to attack" << "\n";
                             options = 0;
                         }
                         for(size_t j{0}; j<targets.size(); j++){
                             int seat = targets[j];
                             int damage = ComputeSkillDamage(person, battle.actors[seat], skills[skillId]);
                             battle.actors[seat].stats.CurrentHp -= damage;
                             if (battle.actors[seat].stats.CurrentHp <= 0) {
                                 battle.actors[seat].stats.CurrentHp = 0;
                             }
                             std::cout << person.name << " used " << skills[skillId].Name
                                       << " on " << battle.actors[seat].name
                                       << " for " << damage << "\n";
                         }
                         if(!targets.empty()){
                         person.stats.CurrentFp -= skills[skillId].FpCost;
                         }
                     }
                     else if(skills[skillId].target == Target::enemy){
                         std::vector<int> targets;
                         for(size_t i{0}; i<battle.actors.size(); i++){
                             if(battle.actors[i].side == Side::foe && battle.actors[i].stats.CurrentHp > 0){
                                 targets.push_back(i);
                             }
                         }
                         if(targets.empty()){
                             std::cout << "No enemies left to attack" << "\n";
                             options = 0;
                         }else{
                             int choice5 = 0;
                             do{
                                 std::cout <<"choose an enemy: " << "\n";
                                 for(size_t j{0}; j < targets.size(); j++){
                                     std::cout << j + 1 << " :" << battle.actors[targets[j]].name << "\n";
                                 }
                                 std::cin >> choice5;
                                 if(std::cin.fail()) {
                                     std::cout << "just numbers" <<"\n";
                                     std::cin.clear();
                                     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                     choice5 = 0;
                                 }
                             }while(choice5 < 1 || choice5 > static_cast<int>(targets.size()));
                             int seat = targets[choice5 - 1];
                             int damage = ComputeSkillDamage(person, battle.actors[seat], skills[skillId]);
                             battle.actors[seat].stats.CurrentHp -= damage;

                             if (battle.actors[seat].stats.CurrentHp <= 0) {
                                 battle.actors[seat].stats.CurrentHp = 0;
                             }
                             bool found = false;
                             if(battle.actors[seat].stats.CurrentHp > 0){
                                 for(size_t i{0}; i < battle.actors[seat].statuses.size(); i++){
                                     if(battle.actors[seat].statuses[i].status == Status::poison){
                                         battle.actors[seat].statuses[i].duration = 2;
                                         std::cout << battle.actors[seat].name << " is already poisoned, duration reset to 2" << "\n";
                                         found = true;
                                         break;
                                     }
                                 }
                                 if(!found){
                                     battle.actors[seat].statuses.push_back({Status::poison, 2});
                                 }
                             }
                             person.stats.CurrentFp -= skills[skillId].FpCost;
                             std::cout << person.name << " used " << skills[skillId].Name
                                       << " on " << battle.actors[seat].name
                                       << " for " << damage << "\n";
                         }
                     }
                     else{
                         std::vector<int> allies;
                            for(size_t i{0}; i<battle.actors.size(); i++){
                                if(battle.actors[i].side == Side::ally && battle.actors[i].stats.CurrentHp > 0){
                                    allies.push_back(i);
                                }
                            }
                            if(allies.empty()){
                                std::cout << "No allies left to support" << "\n";
                                options = 0;
                            }else{
                                int choice6 = 0;
                                do{
                                    std::cout << "Choose an ally to support: " << "\n";
                                    for(size_t j{0}; j < allies.size(); j++){
                                        std::cout << j + 1 << " : " << battle.actors[allies[j]].name << "\n";
                                    }
                                    std::cin >> choice6;
                                    if(std::cin.fail()) {
                                        std::cout << "just numbers" <<"\n";
                                        std::cin.clear();
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                        choice6 = 0;
                                    }
                                }while(choice6 < 1 || choice6 > static_cast<int>(allies.size()));
                                int seat = allies[choice6 - 1];
                                int healAmount = skills[skillId].BasePow;
                                battle.actors[seat].stats.CurrentHp += healAmount;
                                if(battle.actors[seat].stats.CurrentHp > battle.actors[seat].stats.MaxHp){
                                    battle.actors[seat].stats.CurrentHp = battle.actors[seat].stats.MaxHp;
                                }
                                bool found = false;
                                if(battle.actors[seat].stats.CurrentHp > 0){
                                    for(size_t i{0}; i < battle.actors[seat].statuses.size(); i++){
                                        if(battle.actors[seat].statuses[i].status == Status::attackup){
                                            battle.actors[seat].statuses[i].duration = 2;
                                            std::cout << battle.actors[seat].name << " already has attack up, duration reset to 2" << "\n";
                                            found = true;
                                            break;
                                        }
                                    }
                                }
                                if(!found){
                                    battle.actors[seat].statuses.push_back({Status::attackup, 2});
                                }
                                person.stats.CurrentFp -= skills[skillId].FpCost;
                                std::cout << person.name << " used " << skills[skillId].Name
                                          << " on " << battle.actors[seat].name
                                          << " and healed for " << healAmount << "\n";
                            }
                     }
                 }
             }
             else if (options == 3){
                 int choice3 = 0;
                 std::vector<std::string> itemNames;

                    for(const auto& item: battle.itemsCount){
                        if(item.second > 0){
                            itemNames.push_back(item.first);
                        }
                        else{
                            continue;
                        }
                    }
                    if(itemNames.empty()){
                        std::cout << "No items left" << "\n";
                        options = 0;
                    }else{
                 do{
                     std::cout << "Choose an item: " << "\n";
                     int index = 1;
                     for(const auto& item: itemNames){
                         std::cout << index << " : " << battle.itemsCount[item]<< item <<"\n";
                         index++;
                     }
                     std::cin >> choice3;
                     if(std::cin.fail()) {
                         std::cout << "just numbers" <<"\n";
                         std::cin.clear();
                         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                         choice3 = 0;
                     }
             }while(choice3 < 1 || choice3 > itemNames.size());
                std::string itemName = itemNames[choice3 - 1];
                if(battle.itemsCount[itemName] <= 0){
                    std::cout << "No more " << itemName << " left" << "\n";
                    options = 0;
                }
                else{
                    battle.itemsCount[itemName]--;
                    if(items[itemName].effect == ItemEffect::heal){
                        person.stats.CurrentHp += items[itemName].value;
                        if(person.stats.CurrentHp > person.stats.MaxHp){
                            person.stats.CurrentHp = person.stats.MaxHp;
                        }
                        std::cout << person.name << " used " << itemName << " and healed for " << items[itemName].value << "\n";
                    }
                    else if(items[itemName].effect == ItemEffect::restorefp){
                        person.stats.CurrentFp += items[itemName].value;
                        if(person.stats.CurrentFp > person.stats.Maxfp){
                            person.stats.CurrentFp = person.stats.Maxfp;
                        }
                        std::cout << person.name << " used " << itemName << " and restored FP for " << items[itemName].value << "\n";
                    }
                }
             }
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
                if(targets.empty()){
                    continue;
                }
                auto lowesthp = std::min_element(targets.begin(), targets.end(), [&battle](int a, int b){
                    return battle.actors[a].stats.CurrentHp < battle.actors[b].stats.CurrentHp;
                });



                std::random_device rd;
                std::mt19937 gen(rd());

                std::uniform_int_distribution<int> distAction(1, 100);
                int roll = distAction(gen);

                if (roll <= 60){
                    if(lowesthp != targets.end()){
                        int seat = *lowesthp;
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
                }
                    else if(roll <= 85){
                        std::cout << person.name << " Uses a skill" << "\n";
                    }
                    else{
                        std::uniform_int_distribution<int> dist(0, targets.size() - 1);
                        int randomIndex = dist(gen);
                        int seat = targets[randomIndex];
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
         }
          std::cout << "\n" <<"\n";
        }
        }
        for(auto& actor: battle.actors){
            for(size_t i{0}; i < actor.statuses.size(); i++){
                if(actor.statuses[i].status == Status::attackup){
                    std::cout << actor.name << "'s attack is boosted" << "\n";
                }
                else if(actor.statuses[i].status == Status::poison){
                    actor.stats.CurrentHp -= 5;
                    std::cout << actor.name << " is poisoned and takes 5 damage" << "\n";
                    if(actor.stats.CurrentHp <= 0){
                        actor.stats.CurrentHp = 0;
                    }
                }
                else if(actor.statuses[i].status == Status::burn){
                    actor.stats.CurrentHp -= 5;
                    std::cout << actor.name << " is burned and takes 5 damage" << "\n";
                    if(actor.stats.CurrentHp <= 0){
                        actor.stats.CurrentHp = 0;
                    }
                }
                actor.statuses[i].duration--;
                if(actor.statuses[i].duration <= 0){
                    std::cout << actor.name << " is no longer " << StatusToString(actor.statuses[i].status) << "\n";
                    actor.statuses.erase(actor.statuses.begin() + i);
                    i--;
                }
            }
        }
    round++;
    auto status = CheckEnd(battle);
    std::cout << BattleResultToString(status) << "\n";
    }
    return 0;
}
