#include <iostream>
#include <vector>
/*Write `void HealByValue(int hp)` and `void HealByReference(int& hp)`. Call both.
      Print the result. Explain out loud why they differ*/

/*Write `void PrintParty(const std::vector<std::string>& party)`. Remove the `const&`
      and note that it still works - then explain what changed invisibly (a copy of the
      whole vector)*/

/*Take a pointer to a local variable, print the address with `std::cout`, dereference
      it, modify through it*/
/*Write a function that returns a pointer to the party member with the lowest HP, or
      `nullptr` if the party is empty. Call it, check for null before using it*/
/*Deliberately create a dangling reference: return a reference to a local variable from
      a function. Build with sanitizers and see what happens. **Write down the rule you
      just learned***/

/*int* reference (int a){
    int x {0};
    int* Px = &x;

    return Px;
}*/

std::string* lowesthp(std::vector<std::string>& party, std::vector<int>& hpList){

    if (party.size() == 0)
    {
        return nullptr;
    }
    int smallest {0};
    

    size_t i {0};
    //we run this after checking each element in the vector, in this case the party
    while(i < party.size()){
        //if hp at index i is smaller, eg. 0, we want to compare current index to the next one
        // index[0] has 20 > hpList[1] has 40
        //index[0] has 20 > hplist[2] has 50, 
        //right now highest is jack at 0 with a value fo 30 > penny has 20
        if (hpList[smallest] > hpList[i])
        {
            //so now smallest holds index of 0 whcih has a vlue of 20, the vakue of smallest is 0 which is the index
            smallest = i;
            i++;
        }
        else{
            i++;
        }
        
    }

    return &party[smallest];

}

void PrintParty(const std::vector<std::string>& party){

    for( size_t i{0}; i < party.size(); i++){
        std::cout << party[i] << "\n";
    }

}


void HealByValue(int hp){
    hp+=10;
    std::cout << hp <<"\n";
    
}

void HealByReference(int& hp){
    hp+=20;
    std::cout << hp <<"\n";

}

int main(){
    int hp{20};

    int a{20};


    //int* Pa = reference(a);

    //std::cout <<*Pa <<"\n";
    int* pHp = &hp;

    std::cout << pHp << "\n";
    std::cout << *pHp << "\n";

    *pHp += 50;

    std::cout << hp << "\n";

    std::vector<std::string> party{"jack", "penny", "laura"};
    std::vector<int> hpList {100, 50, 10};

    std::string* weakest = lowesthp(party, hpList);
    if(weakest == nullptr){
        std::cout << "No values";
    }
    std::cout << "The weakest member is: "<<*weakest << "\n";

    PrintParty(party);

    HealByValue(hp);
    HealByReference(hp);

    std::cout << hp << "\n";

    return 0;
}