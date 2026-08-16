#include <iostream>
#include <random>
/*Write a damage classifier: given a damage number, print `Miss`, `Graze`, `Hit`,
`Critical` using `if`/`else if` chains*/
/*Rewrite it with a `switch` over an `enum`-free integer "tier". Note which version
reads better and why `switch` needs `break, to asnwer the question
Without break, it keeps going into the next case until it hits a break (or the end).`*/
/*Simulate 1000 attacks with a random damage roll (`<random>`: `std::mt19937` and
`std::uniform_int_distribution`) and print how many landed in each tier*/

int main(){
    //initialze counters outisde of the loops so they dont get rewritten
    int tier = 0;
    int misses = 0;
    int hit = 0;
    int graze = 0;
    int critical = 0;
    //this initializes the seed
    std::random_device rd;
    //this initiales the algorith used
    std::mt19937 gen(rd());
    //this is the range
    std::uniform_int_distribution<int> dist(1, 20);
for(int i = 0; i < 1000; i++){
    int roll = dist(gen);
    if(roll < 5){
        tier = 0;
    }
    else if(roll >= 5 && roll < 10){
        tier = 1;
    }
    else if (roll >= 10 && roll < 15){
        tier = 2;
    }
    else if (roll >= 15 && roll <=20){
        tier = 3;
    }
    else{
        tier = 4;
    };

    if (tier == 0){
        misses++;
    }
    else if(tier == 1){
        graze++;
    }
    else if(tier == 2){
        hit++;
    }
    else if(tier == 3){
        critical++;
    }
}

    std::cout << "missed: \n" <<misses;
    std::cout << "\ngrazed: \n" <<graze;
    std::cout << "\nhit: \n" <<hit;
    std::cout << "\ncritical: \n" <<critical <<"\n";

    

    return 0;
}