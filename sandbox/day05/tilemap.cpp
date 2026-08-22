#include <iostream>
/*Write a nested loop that prints a 10x5 grid of `.` characters with `#` walls around
the edge. This is your first tilemap, in text*/

int main(){



    for (int row = 0; row < 10; row++){
        for(int col = 0; col < 5; col++){
            if (row==0 || row ==9 || col == 0 || col == 4){
                std::cout << '#';
            }
            else{
                std::cout << ".";
            }
        }
        std::cout << '\n';
    }

    return 0;
}