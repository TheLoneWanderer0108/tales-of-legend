#include <iostream>

int main(){

    int array[4] = {1,2,3,4};
    //the purpose of the code is to try to reach an address that does not exsit in the array by adding one
    for(int i = 0; i < 5; i++){
        std::cout << "array value is: " << array[i] <<"\n";
    };

    return 0;
}