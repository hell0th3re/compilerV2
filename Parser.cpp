#include "Parser.h"
#include <iostream>

using namespace std;

string Parser::tokenTypeToString(TokenType type) {
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
        case TokenType::Semicolon:
            return "Semicolon";
        default:
            return "Undefined";
    }
}

const Token &Parser::peek() const{
    if (!isAtEnd()) {
        return tokens[current];
    }
    exit(1);
}

void Parser::advance(){
    if (!isAtEnd()) {
        current++;
    }
    else {
        exit(1);
    }
}

bool Parser::check(const TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    if (type == peek().type) {
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    if (current >= tokens.size()) {
        return true;
    }
    return false;
}

void Parser::consume(TokenType type){

    if (check(type)) {
        advance();
    }
    else {
        cerr << "Expected: " << tokenTypeToString(type)
        << ", found: " << tokenTypeToString(peek().type) << endl;

        cerr << "Syntax error" << endl;
        exit(1);
    }
}

void Parser::parseProgram(){
    while (!isAtEnd()) {
        parseStatement();
    }
}

void Parser::parseStatement() {
    if (peek().type == TokenType::Let) {
        parseDeclaration();
    }
    else {
        cerr << "Unknown token" << endl;
        exit(1);
    }
}


void Parser::parseDeclaration() {

    // Rn only checking variable declearations, not other types of statements
    Variable var;
    consume(TokenType::Let);
    if (check(TokenType::Identifier)) {
        var.name = peek().value;
    }

    consume(TokenType::Identifier);
    consume(TokenType::Colon);

    var.type = parseType();
    // parseType() now consumes the token as well
    //consume(TokenType::Semicolon);
    consume(TokenType::Semicolon);
    variables.push_back(var);
}

TokenType Parser::parseType(){
    if (check(TokenType::IntType)) {
        consume(TokenType::IntType);
        return TokenType::IntType;
    }
    if (check(TokenType::CharType)) {
        consume(TokenType::CharType);
        return TokenType::CharType;
    }
    cerr << "Undefined token type" << endl;
    exit(1);
}

//public
Parser::Parser(const std::vector <Token> &toks) {
    this->tokens = toks;
}

void Parser::parse() {
    parseProgram();
}


const std::vector<Variable> &Parser::getVariables() const{
    return variables;
}
