#include <iostream>
#include <vector>
/*Write a `ScopedLog` class that prints `"enter X"` in its constructor and
      `"exit X"` in its destructor. Create them in nested scopes, in a vector, inside an
      `if`. Predict the output order **before** running, every time*/
/*Add print statements to the copy constructor of a class and count how many copies
      happen when you `push_back` an object into a vector, then when you `reserve` first*/
/*Pass an object by value to a function and count the copies. Change to `const&` and
      count again*/
/*Use `std::move` when pushing a large object into a vector and observe the difference*/
class ScopedLog{

    public:
        ScopedLog(){
            std::cout << "Enter x" << "\n";
        }
        ScopedLog(const ScopedLog&){
            std::cout << "Copy of x" << "\n";
        }
        ~ScopedLog(){
            std::cout << "exit x" << "\n";
        }
};

void print(ScopedLog& log){
    std::cout << "hello";
}

int main(){
    std::vector<ScopedLog> logs;
    ScopedLog a;
    ScopedLog b;
    //“Each push_back copies once. Without reserve, the vector growing copied my first element again. With reserve(2), I only got 2 copies.”
    //logs.reserve(2);

    logs.push_back(ScopedLog());
    logs.push_back(ScopedLog());

    logs.push_back(a);
    logs.push_back(std::move(b));
    //print(logs[0]);
    /*if (1 == 1){
        ScopedLog log;
    }

    if (1 == 1)
    {
        ScopedLog log;
        if( 1 == 1){
            ScopedLog log2;
        }
    }*/
    
    

    return 0;
}