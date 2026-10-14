#include "util.h"


std::vector<std::string> Split(const std::string& line, char delimeter){

    size_t i = 0;
    std::string current = "";
    std::vector<std::string> full = {};

    while(i < line.size()){
        //we build each letter or word, that is divided by its delimeter, if the delimeter appears we enter the if
        // when we reach the final word we dont have a delimeter and we wont add a new word, which means the while is fulfield
        //leaving current with the value of "friend"
        if(line[i] == delimeter){
            //here we start at index since the vector is empty, and we push the entire word up unitl the delimeter, in this case
            //"hello"
            full.push_back(current);
            //now full has {"hello", }, and current is empty
            current.clear();
            //now we go to next index
            i++;
        }
        //if we dont we keep adding new letter or number string
        else{
            current += line[i];
            i++;
        }
    }
    //full has {"hello", "my", "good"} and current has "friend", witht the pushback we isnert the final word
    //so now full is {"hello", "my", "good", "friend"}
    full.push_back(current);


    return full;
}
