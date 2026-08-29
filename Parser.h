#ifndef COMPILERV2_PARSER_H
#define COMPILERV2_PARSER_H
#include <memory>
#include <variant>
#include <vector>
#include "Token.h"
#include "Diagnostics.h"

struct VariableDeclaration {
    std::string name;
    TokenType type;
    Location location;
};

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
    //TokenType type;
    Location location;
    //std::string message;
};

struct Expression {
    std::variant<int, char, bool, std::string,
    std::unique_ptr<BinaryExpression>,
    std::unique_ptr<UnaryExpression>,
    ErrorExpression> value;
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

struct ErrorStatement {
    //TokenType type;
    Location location;
    //std::string message;
};

struct Statement {
    std::variant<
        VariableDeclaration,
        Assignment,
        Exit,
        IfStatement,
        ErrorStatement
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
    Statement parseExit();
    Statement parseDeclaration();
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
