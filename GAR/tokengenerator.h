#ifndef TOKENGENERATOR_H
#define TOKENGENERATOR_H

#define TOKEN_LENGTH 120

#include <iostream>
#include <ctime>
#include <string>

std::string CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,~^´`'!?/><;:+=-_)(*&%$#@[]{}";

std::string token_generator(void);

#endif //TOKENGENERATOR_H
