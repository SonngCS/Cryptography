#include "tokengenerator.h"

std::string token_generator(void){

    std::string new_token;

    srand(time(NULL));

    for(int i = 0; i < TOKEN_LENGTH; i++){
        new_token.insert(i, 1, ((char) CHARS.at(rand()%92)));
    }

    return new_token;
}

int main(void){

    std::string token;

    token = token_generator();

    std::cout << token;

    return 0;
}