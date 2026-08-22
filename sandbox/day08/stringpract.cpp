#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>
#include <vector>

/*Write `std::string WrapText(const std::string& text, int columnWidth)` that inserts
      newlines at word boundaries. This is the actual algorithm your dialogue box will need
      in Phase 4, so do it properly*/
      /*Write a typewriter effect in the console: print one character at a time with a small
      sleep (`std::this_thread::sleep_for`). This is also a Phase 4 system, prototyped in
      15 lines*/
/* Write `std::vector<std::string> Split(const std::string& line, char delimiter)`.
      You will need this for parsing data files on Day 20*/
/*Build a message formatter: given `"{attacker} hits {target} for {damage}!"`,
      replace the placeholders with real values*/
/*Parse `"Goblin,12,4,2,3"` into name and four integers, with `std::stoi`. Handle a
      malformed line without crashing*/

std::vector<std::string> Split(const std::string& line, char delimeter);

void lastformat(const std::string& text){
    int one = 0;
    int two = 0;
    int three = 0;
    int four = 0;

    auto parts = Split(text, ',');
    if (parts.size() != 5){
        std::cout << "Line is no good!";
        return;
    }

    std::string name = parts[0];

     try {
        one = std::stoi(parts[1]);
        two = std::stoi(parts[2]);
        three = std::stoi(parts[3]);
        four = std::stoi(parts[4]);
    } 
    catch (const std::invalid_argument& e) {
        // Se ejecuta si una parte contiene letras en vez de números (ej: "diez")
        std::cout << "Error: Uno de los valores no es un numero valido.\n";
        return; // Salimos de la función de forma segura para no imprimir basura
    }
    catch (const std::out_of_range& e) {
        // Se ejecuta si el número es demasiado grande para caber en un 'int'
        std::cout << "Error: Uno de los numeros es demasiado grande.\n";
        return;
    }

    // Si todo salió bien en el 'try', se ejecuta la impresión normal
    std::cout << "\n" << name << "\n" << one << "\n" << two << "\n" << three << "\n" << four;
}

std::string formatter(const std::string& enemieName, const std::string& target, const int& damage){
    std::string messageTemplate = "{attacker} hits {target} for {damage}!";

    size_t pos = messageTemplate.find("{attacker}");
    //the first argument is starting position, the second is the lenght and thethird what we are replacing it for
    messageTemplate.replace(pos, 10, enemieName);
    size_t posTar = messageTemplate.find("{target}");
    messageTemplate.replace(posTar, 8, target);

    size_t posDam = messageTemplate.find("{damage}");
    //since damage is an int we have to convert it to a string
    std::string damText = std::to_string(damage);

    messageTemplate.replace(posDam, 8, damText);



    return messageTemplate;


}
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

void typewriter(const std::string text){
    //we initialzed the for loop, a typewriter effect, essentially it grabs a string and prints letter by letter with a pause
    for(size_t i =0; i < text.length(); i++){
        //we print out the text, cout has a buffer that can only be shown on screen with a new line character
        //because we want to see each letter, we need it to drop the info it has after every word
        std::cout << text[i] << std::flush;
        //we make the dealy before continuing the loop
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
    }
}

std::string WrapText(const std::string& text, size_t columnWidth){
    if(columnWidth <= 0){
        return text;
    }
    //this variable is waht is being used to iritate the text
    size_t currentWord = 0;
    //this holds the full string after we do all the changes
    std::string full = "";
    //this stores the current full word
    std::string currentFullWord = "";
    //this stores the lenght of the line
    std::string lineLenght = "";
    //we iritate until we have the lenght of the text
    while(currentWord < text.length()){
        //our cut of for the word is space, if space is found we go thru this if
        if(text[currentWord] == ' '){
            //if there is a space it means we have a word, we first check if the lenght of this word and the lenght of line is greater then the width
            //example, we formed the word hello, since lenght is empty it would be hello + empty > 10, which is false, which means we go to the else
            //we continue until wehit another white space which means we have another word, in this case "my", we check the lenght of the line and of the currentword
            //eg. "hello " + "my" > 10, this is false so we got to the else
            //we formed the next word "good", which is held by currentFullword, and we check if adding it with lenght passes the width
            //"hello my " + "good" > 10, this is true, which means we enter the if
            //the final word is friend but beacuse friend has no sapce at the end we never enter this if, which means we now hold to variables
            //linelength with the value "good " and currentfullword with the value "friend", we exit the loop and continue on the enxt if since
            //the condition of the loop has been fullfield
            if(lineLenght.length() + currentFullWord.length() > columnWidth){
                //now inside, we add current linelegnth e.g "hello my " to the full variable which now holds the value
                full += lineLenght;
                //we add a new line to represent the wrap
                full += '\n';
                //we then clear lineleght since the contents have been safely put inside full, so now lineleght is empty
                lineLenght.clear();
                //now taht is empty we add the currentfullword to it which i "good"
                lineLenght += currentFullWord;
                //we add a space, which means the value is now "good "
                lineLenght += " ";
                //and now we clear the currentfullword, which means it holds nothing
                currentFullWord.clear();
                currentWord ++;
            }//in the else following the example, if it does not exceed width, we add the current word in this case hello, to the linelenght
            //and then add a space, then we clear the current full word, that means that now line lenght has "hello " and currentfullword is empty
            //we then itirate on the next position after the white space
            //conituing the example, we add "my" to the linelegnth and then we add a space, and then clear currentfullword
            //so now we have in linelegnht "hello my "
            else{
                lineLenght += currentFullWord;
                lineLenght+= " ";
                currentFullWord.clear();
                currentWord++;
            }
            //if not we keep adding letters
        }else{
            currentFullWord += text[currentWord];
            currentWord++;
        }

    }
    //we check if the linelenght which has "good " + currentfullword which has "friend" is > 10
    //it is, they have more, which means we have to separate the lines
    if (lineLenght.length() + currentFullWord.length() > columnWidth){
        //we add linelentgh which is "good " to full, so now full is "hello my \n good "
        full += lineLenght;
        //we add a new line, so its now "hello my \n good \n"
        full += '\n';
        //and finally we add the currentfullword to full which means the value is "hello my \n good \n friend"
        full += currentFullWord;
    }
    else{
        //if this was not the case we would simply put them together on the same line
        full += lineLenght;
        full += currentFullWord;
    }
    return full;


}

int main(){

    std::string text {"Hello my good friend"};
    size_t colwidth = 10;

    std::string nextext = WrapText(text, colwidth);

    std::cout << nextext << "\n";

    //typewriter(text);

    std::vector<std::string> parts = Split(text, ' ');
    for(size_t i = 0; i < parts.size() ; i++){
        std::cout << parts[i] << " ";
    }

    std::string complete = formatter("goblin", "knight", 12);

    std::cout <<"\n" << complete;

    lastformat("Goblin, 12, 5, 2, 3");

    return 0;
}