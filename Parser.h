#ifndef COMPILERV2_PARSER_H
#define COMPILERV2_PARSER_H
#include <vector>

#include "Token.h"

class Parser {
    std::vector <Token> tokens;
    size_t current = 0;
    Token peek();
    void advance();
    bool check(TokenType type);
    void consume(TokenType type);
    void parseProgram() const;

public:
    Parser(const std::vector <Token> &toks);
};

#endif //COMPILERV2_PARSER_H
