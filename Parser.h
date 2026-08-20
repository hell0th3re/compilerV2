#ifndef COMPILERV2_PARSER_H
#define COMPILERV2_PARSER_H
#include <memory>
#include <variant>
#include <vector>
#include "Token.h"

struct VariableDeclaration {
    std::string name;
    TokenType type;
};

struct Expression;

struct BinaryExpression {
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    TokenType op;
};

struct Expression {
    std::variant<int, char, bool, std::string, std::unique_ptr<BinaryExpression>> value;
};

struct Assignment {
    std::string name;
    Expression value;
};

struct Statement {
    std::variant<
        VariableDeclaration,
        Assignment
    > value;
};

struct Program {
    std::vector <Statement> statements;
};

class Parser {
    Program program;
    std::vector <Token> tokens;
    size_t current = 0;
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token &peek() const;
    void advance();
    [[nodiscard]] bool check(TokenType type) const;
    void consume(TokenType type);
    void parseProgram();
    Statement parseStatement();
    Statement parseDeclaration();
    Statement parseAssignment();
    Expression parseComparison();
    Expression parseExpression();
    Expression parseTerm();
    Expression parseFactor();
    TokenType parseType();

public:
    explicit Parser(const std::vector <Token> &toks);
    Program parse();
};

#endif //COMPILERV2_PARSER_H
