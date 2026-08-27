#ifndef COMPILERV2_LEXER_H
#define COMPILERV2_LEXER_H

#include <map>
#include <fstream>
#include "Token.h"
#include <string>
#include <vector>
#include "Diagnostics.h"

class Lexer {
    std::ifstream file;
    std::vector <Token> tokens;
    std::size_t line;
    std::size_t column;
    Location TokenStart{1,1};
    Diagnostics &diagnostics;

    inline static std::map <std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"exit", TokenType::Exit},
        {"=", TokenType::Assign},
        {"+", TokenType::Add},
        {"-", TokenType::Subtract},
        {"*", TokenType::Multiply},
        {"/", TokenType::Divide},
        {"(", TokenType::OpenParen},
        {")", TokenType::CloseParen},
        {">", TokenType::GreaterThan},
        {"<", TokenType::LessThan},
        {"==", TokenType::Equals},
        {"!=", TokenType::NotEquals},
        {"!", TokenType::Not},
        {"&&", TokenType::And},
        {"||", TokenType::Or},
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
    void consume(char ch);

public:
    explicit Lexer(std::ifstream &file, Diagnostics &diagnostics);
    std::vector <Token> lex();
};

#endif //COMPILERV2_LEXER_H
