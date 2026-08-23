#include <iostream>
#include <random>
#include <limits>

/***Mini project:** a stat generator. Roll a character's stats from a seed, apply a
      class modifier (Warrior +ATK, Mage +MAG, Rogue +SPD), print the sheet, and let the
      user reroll. Use only what you have learned so far*/

int Roll(std::mt19937& gen){
    //this is the range
    std::uniform_int_distribution<int> dist(1, 20);

    return dist(gen);

}

int main(){
    int choice = 0;
    char choice2 = ' ';
    std::random_device rd;
    //this initiales the algorith used combined with the seed
    std::mt19937 gen(rd());

    do{
        std::cout << "What class do you want? \n1)Warrior\n2)Mage\n3)Rogue\n";
        std::cin >> choice;

        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        }
        }while(choice !=1 && choice !=2 && choice !=3);

    do{
        int HP = Roll(gen);
        int ATK = Roll(gen);
        int DEF = Roll(gen);
        int MAG = Roll(gen);
        int SPD = Roll(gen);
        
        if(choice == 1){
            ATK+=5;
        }
        else if (choice == 2)
        {
            MAG+=5;
        }
        else if (choice == 3)
        {
            SPD += 5;
        }
    
        std::cout << HP <<"\n" <<ATK <<"\n" <<DEF <<"\n"<<MAG <<"\n" <<SPD <<"\n";

        std::cout <<"Do you want to reroll? (y)es or (n)o";
        std::cin >> choice2;

    }while(choice2 == 'y');

    return 0;
}