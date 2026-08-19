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
        Statement statement = parseStatement();
        program.statements.push_back(std::move(statement));
    }
}

Statement Parser::parseStatement() {
    Statement statement;
    if (peek().type == TokenType::Let) {
        statement = parseDeclaration();
    }
    else if (peek().type == TokenType::Identifier) {
        statement = parseAssignment();
    }
    else {
        cerr << "Unknown token" << endl;
        exit(1);
    }
    return statement;
}

Statement Parser::parseDeclaration() {
    Statement statement;
    VariableDeclaration var;
    consume(TokenType::Let);
    if (check(TokenType::Identifier)) {
        var.name = peek().value;
    }

    consume(TokenType::Identifier);
    consume(TokenType::Colon);

    var.type = parseType();
    // parseType() now consumes the token as well
    consume(TokenType::Semicolon);

    statement.value = var;
    return statement;
    //program.statements.push_back(std::move(statement));
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

Statement Parser::parseAssignment() {
    Statement statement;
    Assignment assignment;

    assignment.name = peek().value;
    consume(TokenType::Identifier);
    consume(TokenType::Equals);

    assignment.value = parseExpression();

    consume(TokenType::Semicolon);

    statement.value = std::move(assignment);
    return statement;
    //program.statements.push_back(std::move(statement));
}

Expression Parser::parseExpression() {
    Expression left = parseTerm();

    while (check(TokenType::Add) || check(TokenType::Subtract)) {
        TokenType opType = peek().type;

        consume(opType);
        Expression right = parseTerm();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        Expression result;
        result.value = std::move(binary);

        left = std::move(result);
    }

    return left;
}

Expression Parser::parseTerm() {
    Expression left = parseFactor();

    while (check(TokenType::Multiply) || check(TokenType::Divide)) {
        TokenType opType = peek().type;
        consume(opType);

        Expression right = parseFactor();
        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        Expression result;
        result.value = std::move(binary);

        left = std::move(result);
    }

    return left;
}

Expression Parser::parseFactor() {
    Expression result;
    if (check(TokenType::OpenParen)) {
        consume(TokenType::OpenParen);
        result = parseExpression();
        consume(TokenType::CloseParen);
    }
    else if (check(TokenType::Integer)) {
        result.value = stoi(peek().value);
        advance();
    }
    else if (check(TokenType::Character)) {
        result.value = peek().value[1];
        advance();
    }
    else if (check(TokenType::Identifier)) {
        result.value = peek().value;
        advance();
    }
    else {
        cerr << "Expected expression, found "
        << tokenTypeToString(peek().type) << endl;
        exit(1);
    }
    return result;
}

//public
Parser::Parser(const std::vector <Token> &toks) {
    this->tokens = toks;
}

Program Parser::parse() {
    parseProgram();
    return std::move(program);
}