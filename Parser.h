#ifndef COMPILERV2_PARSER_H
#define COMPILERV2_PARSER_H
#include <memory>
#include <variant>
#include <vector>
#include "Token.h"
#include "Diagnostics.h"


struct Expression;

struct UnaryExpression {
    TokenType op;
    std::unique_ptr<Expression> operand;
};

struct BinaryExpression {
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    TokenType op;
};

struct ErrorExpression {
    Location location;
};

struct Expression {
    std::variant<int, char, bool, std::string,
    std::unique_ptr<BinaryExpression>,
    std::unique_ptr<UnaryExpression>,
    ErrorExpression> value;
    Location location;
};

struct VariableDeclaration {
    std::string name;
    TokenType type;
    std::unique_ptr<Expression> initializer;
    Location location;
};

struct Assignment {
    std::string name;
    Expression value;
    Location location;
};

struct Exit{
    Expression value;
    Location location;
};

struct Block;

struct IfStatement {
    Expression condition;
    std::unique_ptr<Block> thenBlock;
    std::unique_ptr<Block> elseBlock;
    Location location;
};

struct WhileLoop {
    Expression condition;
    std::unique_ptr<Block> whileBlock;
    Location location;
};

struct ForLoop {
    VariableDeclaration declaration;
    Expression condition;
    Assignment action;
    std::unique_ptr<Block> forBlock;
    Location location;
};

struct FunctionDeclaration {
    TokenType retType;
    std::string name;
    std::vector<VariableDeclaration> params;
    std::unique_ptr<Block> body;
    Location location;
};

struct FunctionCall {
    std::string name;
    std::vector<VariableDeclaration> params;
};

struct ErrorStatement {
    Location location;
};

struct Statement {
    std::variant<
        VariableDeclaration,
        Assignment,
        Exit,
        IfStatement,
        WhileLoop,
        ForLoop,
        ErrorStatement,
        FunctionDeclaration,
        FunctionCall
    > value;
};

struct Block {
    std::vector <Statement> statements;
    Location location;
};

struct Program {
    std::vector <Statement> statements;
};

class Parser {
    Program program;
    std::vector <Token> tokens;
    size_t current = 0;
    Diagnostics &diagnostics;
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token &peek() const;
    [[nodiscard]] bool isStatementBoundary(TokenType type) const;
    void advance();
    [[nodiscard]] bool check(TokenType type) const;
    bool consume(TokenType type);
    void parseProgram();
    Statement parseStatement();
    void synchronise();
    Statement parseIfStatement();
    Statement parseWhileLoop();
    Statement parseForLoop();
    Statement parseExit();
    Statement parseDeclaration();
    FunctionDeclaration parseFunctionDeclaration(std::string name, TokenType type, Location location);
    VariableDeclaration parseParameter();
    Statement parseVariableDeclaration(std::string name, TokenType type, Location location);
    Statement parseAssignment();
    Expression parseUnary();
    Expression parseLogicOr();
    Expression parseLogicAnd();
    Expression parseEquality();
    Expression parseComparison();
    Expression parseExpression();
    Expression parseTerm();
    Expression parseFactor();
    TokenType parseType();

public:
    explicit Parser(const std::vector <Token> &toks, Diagnostics &diagnostics);
    Program parse();
};

#endif //COMPILERV2_PARSER_H
