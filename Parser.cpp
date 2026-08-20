#include "Parser.h"
#include <iostream>

using namespace std;

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
    if (check(TokenType::BoolType)) {
        consume(TokenType::BoolType);
        return TokenType::BoolType;
    }

    cerr << "Expected type, found " << tokenTypeToString(peek().type) << endl;
    exit(1);
}

Statement Parser::parseAssignment() {

    Statement statement;
    Assignment assignment;

    assignment.name = peek().value;
    consume(TokenType::Identifier);
    consume(TokenType::Assign);

    assignment.value = parseEquality();
    cout << " ";
    consume(TokenType::Semicolon);

    statement.value = std::move(assignment);
    return statement;
}

Expression Parser::parseEquality() {
    Expression left = parseComparison();


    while (check(TokenType::Equals)) {;

        consume(TokenType::Equals);
        Expression right = parseComparison();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = TokenType::Equals;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        Expression result;
        result.value = std::move(binary);

        left = std::move(result);
    }

    return left;
}

Expression Parser::parseComparison() {

    Expression left = parseExpression();

    while (check(TokenType::GreaterThan) || check(TokenType::LessThan)) {

        TokenType opType = peek().type;

        consume(opType);
        Expression right = parseExpression();

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
        result = parseEquality();
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
    else if (check(TokenType::Boolean)) {
        if (peek().value == "true") {
            result.value = true;
        }
        else if (peek().value == "false") {
            result.value = false;
        }
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