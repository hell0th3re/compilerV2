#ifndef COMPILERV2_LEXER_H
#define COMPILERV2_LEXER_H

#include <map>
#include <fstream>
#include "Token.h"
#include <string>
#include <vector>

class Lexer {
    std::ifstream file;
    std::vector <Token> tokens;

    inline static std::map <std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"=", TokenType::Equals},
        {"+", TokenType::Add},
        {"-", TokenType::Subtract},
        {"*", TokenType::Multiply},
        {"/", TokenType::Divide},
        {"(", TokenType::OpenParen},
        {")", TokenType::CloseParen},
        {">", TokenType::GreaterThan},
        {"<", TokenType::LessThan},
        {"int", TokenType::IntType},
        {"char", TokenType::CharType},
        {"bool", TokenType::BoolType},
        {"true", TokenType::Boolean},
        {"false", TokenType::Boolean},
        {":", TokenType::Colon},
        {";", TokenType::Semicolon}
    };
    static bool isIdentifier(const std::string &word);
    static bool isInteger(const std::string &word);
    static TokenType getTokenType(const std::string &tokenVal);
    static TokenType processWord(const std::string &word);
    void tokenize();

public:
    Lexer(std::ifstream &file);
    std::vector <Token> lex();
};

#endif //COMPILERV2_LEXER_H
