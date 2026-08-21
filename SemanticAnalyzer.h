#ifndef COMPILERV2_SEMANTICANALYZER_H
#define COMPILERV2_SEMANTICANALYZER_H
#include <map>
#include "Parser.h"

enum class opGeneral {
    Arithmetic,
    Comparison,
    Equality,
    Logic
};

class SemanticAnalyzer {
    Program program;
    void process();
    std::map<std::string, TokenType> variables;

    TokenType getExpressionType(const Expression &expression);
    static opGeneral getGeneralType(TokenType opType);
    static void checkArithmetic(TokenType opType, const BinaryExpression &binary);
public:
    explicit SemanticAnalyzer(Program program);
    Program analyze();
};

#endif //COMPILERV2_SEMANTICANALYZER_H
