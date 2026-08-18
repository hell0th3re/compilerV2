#ifndef COMPILERV2_PARSER_H
#define COMPILERV2_PARSER_H
#include <vector>
#include "Token.h"

struct Variable {
    std::string name;
    TokenType type;
};

class Parser {
    std::vector <Token> tokens;
    size_t current = 0;
    std::vector <Variable> variables;
    static std::string tokenTypeToString(TokenType type);
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token &peek() const;
    void advance();
    [[nodiscard]] bool check(TokenType type) const;
    void consume(TokenType type);
    void parseProgram();
    void parseStatement();
    void parseDeclaration();
    TokenType parseType();

public:
    explicit Parser(const std::vector <Token> &toks);
    void parse();
    [[nodiscard]] const std::vector <Variable> &getVariables() const;
};

#endif //COMPILERV2_PARSER_H
