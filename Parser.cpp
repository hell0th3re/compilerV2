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
        case TokenType::Eof:
            return "EOF";
        default:
            return "Undefined";
    }
}

const Token &Parser::peek() const{
    return tokens[current];
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
    return type == peek().type;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::Eof;
}

void Parser::consume(TokenType type){

    if (check(type)) {
        advance();
    }
    else {
        cerr << "Expected: " << tokenTypeToString(type)
        << ", found: " << tokenTypeToString(peek().type) << endl;
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
    else if (peek().type == TokenType::Identifier) {
        parseAssignment();
    }
    else {
        cerr << "Unknown token" << endl;
        exit(1);
    }
}

void Parser::parseDeclaration() {
    Variable var;
    consume(TokenType::Let);
    if (check(TokenType::Identifier)) {
        var.name = peek().value;
    }

    consume(TokenType::Identifier);
    consume(TokenType::Colon);

    var.type = parseType();
    // parseType() now consumes the token as well
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
    cerr << "Expected type, found " << tokenTypeToString(peek().type) << endl;
    exit(1);
}

void Parser::parseAssignment() {
    Assignment assignment;
    assignment.name = peek().value;
    consume(TokenType::Identifier);
    consume(TokenType::Equals);

    if (check(TokenType::Integer)) {
        assignment.type = peek().type;
        assignment.value = stoi(peek().value);
        advance();
    }
    else if (check(TokenType::Character)) {
        assignment.type = peek().type;
        assignment.value = peek().value[1];
        advance();
    }
    else {
        cerr << "Expected int or char literal, found " << tokenTypeToString(peek().type) << endl;
        exit(1);
    }

    consume(TokenType::Semicolon);
    assignments.push_back(assignment);
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

const std::vector<Assignment> &Parser::getAssignments() const{
    return assignments;
}
