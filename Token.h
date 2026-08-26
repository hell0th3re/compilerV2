#ifndef COMPILERV2_TOKEN_H
#define COMPILERV2_TOKEN_H

#include <string>

enum class TokenType {
    //Keywords
    Let,
    Exit,
    IntType,
    CharType,
    BoolType,

    //Literials
    Integer,
    Character,
    Boolean,

    //Operators
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide,
    GreaterThan,
    LessThan,
    Equals,
    NotEquals,
    Not,
    And,
    Or,

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

inline std::string tokenTypeToString(TokenType type) {
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
        case TokenType::Assign:
            return "Assign";
        case TokenType::Equals:
            return "Equals";
        case TokenType::NotEquals:
            return "NotEquals";
        case TokenType::Not:
            return "Not";
        case TokenType::And:
            return "And";
        case TokenType::Let:
            return "Let";
        case TokenType::Exit:
            return "Exit";
        case TokenType::Add:
            return "Add";
        case TokenType::Or:
            return "Or";
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
