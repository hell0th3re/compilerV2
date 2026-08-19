#ifndef COMPILERV2_PARSER_H
#define COMPILERV2_PARSER_H
#include <memory>
#include <variant>
#include <vector>
#include "Token.h"

struct Variable {
    std::string name;
    TokenType type;
};

struct Expression;

struct BinaryExpression {
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    TokenType op;

    // BinaryExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, TokenType op)
    //     :left(std::move(left)), right(std::move(right)), op(op) {}
};

struct Expression {
    std::variant<int,char, std::string, std::unique_ptr<BinaryExpression>> value;
};

struct Assignment {
    std::string name;
    Expression value;
};


class Parser {
    std::vector <Token> tokens;
    size_t current = 0;
    std::vector <Variable> variables;
    std::vector <Assignment> assignments;
    static std::string tokenTypeToString(TokenType type);
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token &peek() const;
    void advance();
    bool isOperator() const;
    [[nodiscard]] bool check(TokenType type) const;
    void consume(TokenType type);
    void parseProgram();
    void parseStatement();
    void parseDeclaration();
    void parseAssignment();
    Expression parseExpression();
    Expression parseTerm();
    Expression parseFactor();
    TokenType parseType();

public:
    explicit Parser(const std::vector <Token> &toks);
    void parse();
    [[nodiscard]] const std::vector <Variable> &getVariables() const;
    [[nodiscard]] const std::vector <Assignment> &getAssignments() const;
};

#endif //COMPILERV2_PARSER_H
