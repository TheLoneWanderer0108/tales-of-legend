#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <fstream>

/*Rewrite three hand-written loops with algorithms: "is anyone alive"
      (`std::any_of`), "find lowest HP ally" (`std::min_element`), "total party HP"
      (`std::accumulate`)*/
/*Store a callback in `std::function<void(Actor&)>` and call it. Understand that this
      is how your event and menu systems will work later*/

struct Stats
{   
    std::string name{' '};
    int Hp{0};
};
struct Enemy{
    std::string name{' '};
    int maxHp{0};
    int atk{0};
    int def{0};
    int spd{0};
    int xp{0};
};
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

std::vector<Enemy> enemies(const std::string& path){
    std::ifstream file(path);
    std::vector<Enemy> result;
    int linenumber = 0;

    if (!file.is_open()) {
    std::cerr << "Error: Could not open file.\n";
    return {};
    }
    else{
        std::string line;
        while (std::getline(file, line)) {
        ++linenumber;
        if(line.empty()){
            continue;
        }
        if (line[0] == '#'){
            continue;
        }
        auto parts = Split(line, ',');
        if (parts.size() != 6){
            std::cout << "Error in line: " <<linenumber;
            continue;
        }
        else{
            Enemy e;
            e.name = parts[0];
            try{
            e.maxHp = std::stoi(parts[1]);
            e.atk = std::stoi(parts[2]);
            e.def = std::stoi(parts[3]);
            e.spd = std::stoi(parts[4]);
            e.xp = std::stoi(parts[5]);
            result.push_back(e);
            }catch(const std::invalid_argument& e){
                std::cout <<"ERROR loading arguments" << linenumber;
                continue;
            }

        }
    }
    }
    return result;
}

void SaveEnemies(const std::string& path, const std::vector<Enemy>& enemies){
    std::ofstream File(path);
    if (!File) {
        std::cerr << "Error opening file!" << std::endl;
    }
    for(size_t i{0}; i<enemies.size(); i++){
         File << enemies[i].name <<","<< enemies[i].maxHp <<"," << enemies[i].atk <<","<< enemies[i].def <<","<< enemies[i].spd <<","<< enemies[i].xp <<"\n";
    }

    File.close();

}


int main(){

    std::vector<Stats> party = {{"Jack", 1}, {"Linsey", 20}, {"Agatha", 30}};

    auto loaded = enemies("enemies.csv");
    for(size_t i{0}; i<loaded.size(); i++){
        std::cout << loaded[i].name;
    }

    SaveEnemies("enemies_out.csv", loaded);



    bool anyAlive = std::any_of(party.begin(), party.end(), [](const Stats& member){
        
        if(member.Hp <= 0){
            return false;
        }

        else{
            return true;
        }


    });

    std::cout << anyAlive;

    auto lowestHp = std::min_element(party.begin(), party.end(), [](const Stats& a, const Stats& b){

        return a.Hp < b.Hp;

        
    });

    if(lowestHp != party.end()){
        std::cout << lowestHp ->name << lowestHp ->Hp;
    }

    auto CurrentPartyHp = std::accumulate(party.begin(), party.end(), 0, [](int total, const Stats& member){
        return total + member.Hp;

    });

    std::cout << CurrentPartyHp;

    std::function<void(Stats&)> Addhp;

    Addhp = [](Stats& a){
        a.Hp += 10;
        std::cout << a.name << a.Hp;
    };

    Addhp(party[0]);


    return 0;
}