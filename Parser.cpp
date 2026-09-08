#include "Parser.h"
#include <utility>
#include <iostream>

const Token &Parser::peek() const{
    return tokens[current];
}

void Parser::advance(){
    if (!isAtEnd()) {
        current++;
    }
}

bool Parser::check(const TokenType type) const {
    return type == peek().type;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::Eof;
}

bool Parser::isStatementBoundary(TokenType type) const {
    bool boundaryCheck = (
             peek().type == TokenType::Let ||
             peek().type == TokenType::IntType ||
             peek().type == TokenType::CharType ||
             peek().type == TokenType::Identifier ||
             peek().type == TokenType::Exit ||
             peek().type == TokenType::If ||
             peek().type == TokenType::While ||
             peek().type == TokenType::For ||
             peek().type == TokenType::CloseBraces ||
             peek().type == TokenType::Eof
    );
    return boundaryCheck;
}

bool Parser::consume(TokenType type){
    if (check(type)) {
        advance();
        return true;
    }
    diagnostics.error(
        "Expected: " + tokenTypeToString(type) +
        ", found: " + tokenTypeToString(peek().type),
        peek().location
    );
    return false;
}

void Parser::parseProgram(){
    while (!isAtEnd()) {
        Statement statement = parseStatement();
        program.statements.push_back(std::move(statement));
    }
}

void Parser::synchronise() {
    while (!isStatementBoundary(peek().type)) {
        advance();
    }
}

static bool isVarType(TokenType type) {
    if (type == TokenType::IntType ||
        type == TokenType::CharType ||
        type == TokenType::BoolType) {
        return true;
    }
    return false;
}

Statement Parser::parseStatement() {
    Statement statement;
    if (isVarType(peek().type)) {
        statement = parseDeclaration();
    }
    else if (peek().type == TokenType::Identifier) {
        statement = parseAssignment();
    }
    else if (peek().type == TokenType::Exit) {
        statement = parseExit();
    }
    else if (peek().type == TokenType::If) {
        statement = parseIfStatement();
    }
    else if (peek().type == TokenType::While) {
        statement = parseWhileLoop();
    }
    else if (peek().type == TokenType::For) {
        statement = parseForLoop();
    }
    else {
        ErrorStatement error{};
        error.location = peek().location;

        diagnostics.error(
    "Unexpected token: " + tokenTypeToString(peek().type),
            peek().location
        );

        statement.value = error;
        synchronise();
    }
    return statement;
}

Statement Parser::parseForLoop() {
    Statement statement;
    Block block;
    ForLoop forLoop;

    forLoop.location = peek().location;
    consume(TokenType::For);

    if (!consume(TokenType::OpenParen)) {
        synchronise();
    }

    if (check(TokenType::IntType)) {
        Statement declStat = parseDeclaration();

        if (std::holds_alternative<VariableDeclaration>(declStat.value)) {
            VariableDeclaration loopVar = std::move(std::get<VariableDeclaration>(declStat.value));
            //loopVar.initialiser == nullptr should be an error, might leave it for semantic analisis
            forLoop.declaration = std::move(loopVar);
        }
    }
    else {
        synchronise();
    }
    //for(int a = 5; ...
    Expression condition = parseLogicOr();
    if (!consume(TokenType::Semicolon)) {
        synchronise();
    }
    forLoop.condition = std::move(condition);
    //for(int a = 5; a > 1; ...

    Assignment assignment;

    assignment.name = peek().value;
    assignment.location = peek().location;
    if (!consume(TokenType::Identifier)) {
        synchronise();
    }
    if (!consume(TokenType::Assign)) {
        synchronise();
    }
    assignment.value = parseLogicOr();
    forLoop.action = std::move(assignment);
    //for(int a = 5; a > 1; a = a+1)

    if (!consume(TokenType::CloseParen)) {
        synchronise();
    }
    block.location = peek().location;
    if (!consume(TokenType::OpenBraces)) {
        synchronise();
    }

    while (peek().type != TokenType::CloseBraces && peek().type != TokenType::Eof) {
        block.statements.push_back(parseStatement());
    }

    consume(TokenType::CloseBraces);

    forLoop.forBlock = std::make_unique<Block>(std::move(block));
    statement.value = std::move(forLoop);

    //for(int a = 5; a > 1; a = a+1){...}
    return statement;
}

Statement Parser::parseWhileLoop() {
    Statement statement;
    Block block;
    WhileLoop whileLoop;

    whileLoop.location = peek().location;
    consume(TokenType::While);

    if (!consume(TokenType::OpenParen)) {
        synchronise();
    }

    Expression condition = parseLogicOr();

    if (!consume(TokenType::CloseParen)) {
        synchronise();
    }

    block.location = peek().location;
    if (!consume(TokenType::OpenBraces)) {
        synchronise();
    }

    while (peek().type != TokenType::CloseBraces && peek().type != TokenType::Eof) {
        block.statements.push_back(parseStatement());
    }
    consume(TokenType::CloseBraces);

    whileLoop.condition = std::move(condition);
    whileLoop.whileBlock = std::make_unique<Block>(std::move(block));
    statement.value = std::move(whileLoop);

    return statement;
}

Statement Parser::parseIfStatement() {
    Statement statement;
    Block block;
    IfStatement ifStat;

    ifStat.location = peek().location;

    consume(TokenType::If);

    if (!consume(TokenType::OpenParen)) {
        synchronise();
    }

    Expression condition = parseLogicOr();

    if (!consume(TokenType::CloseParen)) {
        synchronise();
    }

    block.location = peek().location;
    if (!consume(TokenType::OpenBraces)) {
        synchronise();
    }

    while (peek().type != TokenType::CloseBraces && peek().type != TokenType::Eof) {
        block.statements.push_back(parseStatement());
    }
    consume(TokenType::CloseBraces);

    if (peek().type == TokenType::Else) {
        consume(TokenType::Else);
        Block elseBlock;
        bool consumeClose = true;
        bool consumeOpen = true;
        if (peek().type == TokenType::If) {
            Statement elseStat = parseIfStatement();
            elseBlock.statements.push_back(std::move(elseStat));
            consumeClose = false;
            consumeOpen = false;
        }
        if (consumeOpen) {
            elseBlock.location = peek().location;
            if (!consume(TokenType::OpenBraces)) {
                synchronise();
            }
        }
        while (peek().type != TokenType::CloseBraces && peek().type != TokenType::Eof) {
            elseBlock.statements.push_back(parseStatement());
        }
        if (consumeClose) {
            if (!consume(TokenType::CloseBraces)) {
                synchronise();
            }
        }

        ifStat.elseBlock = std::make_unique<Block>(std::move(elseBlock));
    }
    ifStat.condition = std::move(condition);
    ifStat.thenBlock = std::make_unique<Block>(std::move(block));
    statement.value = std::move(ifStat);
    return statement;
}

Statement Parser::parseExit() {
    Statement statement;
    Exit exit;

    exit.location = peek().location;

    consume(TokenType::Exit);
    Expression temp = parseLogicOr();

    exit.value = std::move(temp);
    statement.value = std::move(exit);

    if (!consume(TokenType::Semicolon)) {
        synchronise();
    }
    return statement;
}

Statement Parser::parseDeclaration() {
    Statement statement;
    VariableDeclaration var;

    var.type = parseType();

    //consume(peek().type); - done in parseType
    if (check(TokenType::Identifier)) {
        var.name = peek().value;
    }

    var.location = peek().location;

    if (!consume(TokenType::Identifier)) {
        synchronise();
    }

    if (check(TokenType::Assign)) {
        consume(TokenType::Assign);
        std::unique_ptr<Expression>ex = std::make_unique<Expression>(parseLogicOr());
        var.initializer = std::move(ex);
    }

    if (!consume(TokenType::Semicolon)) {
        synchronise();
    }

    statement.value = std::move(var);
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
    diagnostics.error(
        "Expected type, found " + tokenTypeToString(peek().type),
        peek().location
    );
    return TokenType::Undefined;
}

Statement Parser::parseAssignment() {

    Statement statement;
    Assignment assignment;

    assignment.name = peek().value;
    assignment.location = peek().location;
    if (!consume(TokenType::Identifier)) {
        synchronise();
    }
    if (!consume(TokenType::Assign)) {
        synchronise();
    }
    assignment.value = parseLogicOr();
    if (!consume(TokenType::Semicolon)) {
        synchronise();
    }
    statement.value = std::move(assignment);
    return statement;
}

Expression Parser::parseUnary() {
    if (peek().type == TokenType::Not) {
        Expression result;
        result.location = peek().location;
        consume(TokenType::Not);
        auto unary = std::make_unique<UnaryExpression>();
        unary->operand = std::make_unique<Expression>(parseUnary());
        unary->op = TokenType::Not;
        result.value = std::move(unary);

        return result;
    }
    return parseFactor();
}

Expression Parser::parseLogicOr() {
    Expression left = parseLogicAnd();


    while (check(TokenType::Or)) {
        Expression result;
        TokenType opType = peek().type;
        result.location = peek().location;

        if (!consume(opType)) {
            synchronise();
        }

        Expression right = parseLogicAnd();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        result.value = std::move(binary);
        left = std::move(result);
    }

    return left;
}

Expression Parser::parseLogicAnd() {
    Expression left = parseEquality();


    while (check(TokenType::And)) {
        Expression result;
        TokenType opType = peek().type;
        result.location = peek().location;
        if (!consume(opType)) {
            synchronise();
        }

        Expression right = parseEquality();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        result.value = std::move(binary);
        left = std::move(result);
    }

    return left;
}

Expression Parser::parseEquality() {
    Expression left = parseComparison();


    while (check(TokenType::Equals) || check(TokenType::NotEquals)) {
        Expression result;
        result.location = peek().location;

        TokenType opType = peek().type;
        if (!consume(opType)) {
            synchronise();
        }

        Expression right = parseComparison();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        result.value = std::move(binary);
        left = std::move(result);
    }

    return left;
}

Expression Parser::parseComparison() {

    Expression left = parseExpression();

    while (check(TokenType::GreaterThan) || check(TokenType::LessThan)) {
        Expression result;
        result.location = peek().location;
        TokenType opType = peek().type;

        if (!consume(opType)) {
            synchronise();
        }
        Expression right = parseExpression();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        result.value = std::move(binary);
        left = std::move(result);
    }

    return left;
}

Expression Parser::parseExpression() {

    Expression left = parseTerm();

    while (check(TokenType::Add) || check(TokenType::Subtract)) {
        Expression result;
        TokenType opType = peek().type;
        result.location = peek().location;

        if (!consume(opType)) {
            synchronise();
        }
        Expression right = parseTerm();

        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));

        result.value = std::move(binary);

        left = std::move(result);
    }

    return left;
}

Expression Parser::parseTerm() {

    Expression left = parseUnary();

    while (check(TokenType::Multiply) || check(TokenType::Divide)) {
        TokenType opType = peek().type;
        Expression result;
        result.location = peek().location;
        if (!consume(opType)) {
            synchronise();
        }

        Expression right = parseUnary();
        auto binary = std::make_unique<BinaryExpression>();

        binary->op = opType;
        binary->left = std::make_unique<Expression>(std::move(left));
        binary->right = std::make_unique<Expression>(std::move(right));


        result.value = std::move(binary);

        left = std::move(result);
    }

    return left;
}

Expression Parser::parseFactor() {
    Expression result;
    result.location = peek().location;
    if (check(TokenType::OpenParen)) {
        consume(TokenType::OpenParen);
        result = parseLogicOr(); //highest precedence
        if (!consume(TokenType::CloseParen)) {
            synchronise();
        }
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
        ErrorExpression error{};
        error.location = peek().location;

        diagnostics.error(
    "Unexpected token: " + tokenTypeToString(peek().type),
            peek().location
        );

        result.value = error;
        synchronise();
    }
    return result;
}

//public
Parser::Parser(const std::vector <Token> &toks, Diagnostics &diagnostics) :
    tokens(toks),
    diagnostics(diagnostics) {}


Program Parser::parse() {
    parseProgram();
    return std::move(program);
}