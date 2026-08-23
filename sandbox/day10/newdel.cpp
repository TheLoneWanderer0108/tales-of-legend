#include <iostream>
#include <vector>

/*Allocate an `int` with `new`, use it, `delete` it. Then forget the `delete` and find
the leak with `-fsanitize=address` (or `valgrind ./prog` if you install it)*/

/*Allocate an array with `new int[10]`, fill it, and free it with `delete[]`. Free it
      with plain `delete` instead and read the sanitizer complaint*/
/*Write the same program with `std::vector<int>` and count how many lines of
      bookkeeping disappeared*/

/*Write a C-style array function `int Sum(const int* data, int count)` so you
      understand what "array decays to pointer" means. This is why raylib's C API takes
      pointer + count everywhere*/

int Sum(const int* data, int count){

    int sum{0};

    for(size_t i{0}; i < count; i++){
        sum += data[i];
    }

    return sum;

}


int main(){

    std::vector<int> arr{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};


    for(size_t i{0}; i < arr.size(); i++){
        std::cout << arr[i];
    }
    
    int* prt = nullptr;


    int* array = new int[10];

    for (size_t i{0}; i < 10; i++){
        array[i] = i + 1;
    }

    for (size_t i = 0; i < 10; i++) {
        std::cout << array[i] << " ";
    }

    int allsum = Sum(array, 10);

    std::cout <<allsum << "\n";

    // Deallocate the memory
    delete[] array;

    prt = new int;
    *prt = 40;

    std::cout << *prt << "\n";

    //free memories
    delete prt;
    //stops danglin pointer
    prt = nullptr;


    return 0;
}