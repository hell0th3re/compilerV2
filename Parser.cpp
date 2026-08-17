#include "Parser.h"
#include <iostream>

using namespace std;

Parser::Parser(const std::vector <Token> &toks) {
    this->tokens = toks;
}

Token Parser::peek() {
    return tokens[current];
}

void Parser::advance() {
    current++;
}

bool Parser::check(TokenType type) {
    if (type == peek().type) {
        return true;
    }
    return false;
}

void Parser::consume(TokenType type) {
    if (check(type)) {
        advance();
    }
    else {
        cerr << "Syntax error";
    }
}

void Parser::parseProgram() const {
    for (int i = 0; i < tokens.size(); i++) {
        
    }
}
