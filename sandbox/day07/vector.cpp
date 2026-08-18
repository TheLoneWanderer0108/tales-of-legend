/*Store a party of 3 names in a `std::vector<std::string>` and print them numbered*/
/*Store HP in a parallel `std::vector<int>`. Feel the awkwardness of keeping two
      vectors in sync - that pain is the argument for `struct`, which arrives on Day 11*/
/*Write `int TotalHP(const std::vector<int>& hps)`. Note why the parameter is
      `const&`, even though you do not fully understand references until Day 9*/
/*Write a function that removes all dead party members from the vector. Do it with a
      loop, get the bug where erasing while iterating skips an element, then fix it.
      Afterwards look up `std::erase_if` and see how the standard library solves it*/
#include <iostream>
#include <vector>
#include <string>

void Dead(std::vector<std::string>& character, std::vector<int>& hp){
    std::size_t i = 0;

    /*size_t removed = std::erase_if(hp, [](int h){
                return h == 0;
            }); */

    //this while iterates the list in i, the size is less then the size of the vector in character
        while(i < character.size()){
            //checks if heath is 0
            if( hp[i] == 0){
            //if it is it deletes the healh and the character
            character.erase(character.begin() + i);
            hp.erase(hp.begin() + i);
            }
            //otherwise it increments i++ to the next iteration
            else{
                i++;
            }
        }

    for(std::size_t i = 0; i<character.size(); i++){
        std::cout <<"Alive characters are: " << character[i] << "\n";
        std::cout <<"Healths are: " << hp[i] << "\n";
    }

}

int TotalHp(const std::vector<int>& hps){
    int sum = 0;

    for(std::size_t i =0; i <hps.size(); i++){
        sum += hps[i];
    }

    return sum;
}

int main(){
    //make sure when using strings in vectors use double quotes
    std::vector<std::string> characters {"johnny", "margaret", "penny"};
    std::vector<int> HP {0, 0, 120};
    //when dealing with vectors always use .size(), also .size() uses size_t, while regular int can be negative as well
    for(std::size_t i = 0; i < characters.size(); i++){
        std::cout << "in position " << i << " here is: " <<characters[i] << " has: " <<HP[i]<< " Health" << "\n";
    }

    int sum = TotalHp(HP);

    std::cout << "The sum is: " << sum << "\n";

    Dead(characters, HP);

    return 0;
}