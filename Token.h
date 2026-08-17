#ifndef COMPILERV2_TOKEN_H
#define COMPILERV2_TOKEN_H

#include <string>

enum class TokenType {
    //Keywords
    Let,
    IntType,
    CharType,

    //Literials
    Integer,
    Character,

    //Operators
    Equals,
    Add,
    Subtract,
    Multiply,

    //Punctuation
    Colon,
    Semicolon,

    //Other
    Identifier,
    Undefined
};

struct Token {
    TokenType type;
    std::string value;
};

#endif //COMPILERV2_TOKEN_H
