#ifndef COMPILERV2_TOKEN_H
#define COMPILERV2_TOKEN_H

#include <string>
using namespace std;

enum class TokenType {
    //Keywords
    Let,
    IntType,
    CharType,

    //Literials
    Integer,
    Character,

    //Operators
    Assign,

    //Punctuation
    Colon,
    Semicolon,

    //Other
    Identifier
};

struct Token {
    TokenType type;
    string value;
};

#endif //COMPILERV2_TOKEN_H
