#include <iostream>
#include <iomanip>
#include <limits>
//name, level, HP/MaxHP, ATK, DEF, SPD. Align the columns with `std::setw`
//Compute and print HP as a percentage. Make it print `73%`, not `0%` - understand *why* `73 / 100` is `0` and `73.0 / 100` is not

int main(){
    std::string name = "jack";
    int level = 1;
    int HP = 50;
    int maxHP= 200;
    int currentHP = HP/(double)maxHP * 100;
    int ATK = 10;
    int DEF = 5;
    int SPD = 5;
    std::cout << "Enter your name: ";
    std::cin >> name;
    std::cout << "Enter your level: ";
    std::cin >> level;

    std::cout << std::setw(7)<< "name:" <<std::setw(7) << name <<"\n"
    << std::setw(7)<< "Level:"<<std::setw(7) << level <<"\n"
    << std::setw(7)<< "HP:" <<std::setw(7)<< HP << "/" << maxHP << " "<< currentHP << "%"<<"\n" 
    << std::setw(7)<< "ATK:" <<std::setw(7)<< ATK <<"\n"
    << std::setw(7)<< "DEF:" <<std::setw(7) << DEF <<"\n" 
    << std::setw(7)<< "SPD:"<<std::setw(7) << SPD <<"\n" <<
     "Max value of int is: " << std::numeric_limits<int>::max() << "\n";

     std::cout
    << std::setw(7)<< "Int:"<<std::setw(7) << sizeof(int) <<"\n"
    << std::setw(7)<< "Float:" <<std::setw(7)<< sizeof(float) <<"\n" 
    << std::setw(7)<< "double:" <<std::setw(7)<< sizeof(double) <<"\n"
    << std::setw(7)<< "char:" <<std::setw(7) << sizeof(char) <<"\n" 
    << std::setw(7)<< "BOOL:"<<std::setw(7) << sizeof(bool) <<"\n" <<
     "Max value of int is: " << std::numeric_limits<int>::max() + 1 << "\n";
     
     
    return 0;
}