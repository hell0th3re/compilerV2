#ifndef COMPILERV2_LEXER_H
#define COMPILERV2_LEXER_H

#include <map>
#include <fstream>
#include "Token.h"
#include <string>
#include <vector>
using namespace std;

class Lexer {
    ifstream file;
    vector <string> words;
    vector <Token> tokens;

    map <string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"=", TokenType::Assign},
        {"int", TokenType::IntType},
        {"char", TokenType::CharType},
        {":", TokenType::Colon},
        {";", TokenType::Semicolon},
    };

    void readWords();

    static bool isIdentifier(string &word);
    static bool isInteger(string &word);
    static bool isChar(string &word);
    void tokenize();

public:
    Lexer(ifstream &file);
    vector <Token> getTokens();
};

#endif //COMPILERV2_LEXER_H
