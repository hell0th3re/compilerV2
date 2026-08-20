#ifndef COMPILERV2_TOKEN_H
#define COMPILERV2_TOKEN_H

#include <string>

enum class TokenType {
    //Keywords
    Let,
    IntType,
    CharType,
    BoolType,

    //Literials
    Integer,
    Character,
    Boolean,

    //Operators
    Equals,
    Add,
    Subtract,
    Multiply,
    Divide,
    GreaterThan,
    LessThan,
    EqualTo,

    OpenParen,
    CloseParen,

    //Punctuation
    Colon,
    Semicolon,

    //Other
    Identifier,
    Undefined,
    Eof
};

inline std::string tokenTypeToString(TokenType type) { //maybe should be inline, not static? idk.
    switch (type) {
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Integer:
            return "Integer";
        case TokenType::Character:
            return "Character";
        case TokenType::IntType:
            return "IntType";
        case TokenType::CharType:
            return "CharType";
        case TokenType::BoolType:
            return "BoolType";
        case TokenType::Colon:
            return "Colon";
        case TokenType::Equals:
            return "Equals";
        case TokenType::Let:
            return "Let";
        case TokenType::Add:
            return "Add";
        case TokenType::Subtract:
            return "Subtract";
        case TokenType::Multiply:
            return "Multiply";
        case TokenType::Divide:
            return "Divide";
        case TokenType::OpenParen:
            return "OpenParen";
        case TokenType::CloseParen:
            return "CloseParen";
        case TokenType::Semicolon:
            return "Semicolon";
        case TokenType::Eof:
            return "EOF";
        default:
            return "Undefined";
    }
}

struct Token {
    TokenType type;
    std::string value;
};

#endif //COMPILERV2_TOKEN_H
