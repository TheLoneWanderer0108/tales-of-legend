#include <iostream>
#include <limits>
/*Write a menu loop: print `1) Attack 2) Defend 3) Run 4) Quit`, read a choice, act,
loop until quit. Handle invalid input without crashing or infinite-looping*/

int main(){

    int options = 0;

    do{
        std::cout << "\nChoose what you want to do: 1) attack, 2)defend, 3)run, 4)quit\n";
        std::cin >> options;

        if(std::cin.fail()) {
            std::cout << "\njust numbers";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }


        else if( options == 1){
            std::cout << "you attacked\n";
        }
        else if (options == 2){
            std::cout << "You defend\n";
        }
        else if (options == 3){
            std::cout << "You ran\n";
        }
        else if (options == 4){
            std::cout << "You quit\n";
        }
        else{
            std::cout << "\nInvalid option";
        }
    }while(options != 4);
    
    return 0;

}