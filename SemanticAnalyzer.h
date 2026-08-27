#ifndef COMPILERV2_SEMANTICANALYZER_H
#define COMPILERV2_SEMANTICANALYZER_H
#include <map>
#include <unordered_set>
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
    std::unordered_set<std::string> assignments;
    Diagnostics &diagnostics;

    TokenType getExpressionType(const Expression &expression);
    static opGeneral getGeneralType(TokenType opType);
    static void checkArithmetic(TokenType opType, const BinaryExpression &binary);
public:
    explicit SemanticAnalyzer(Program program, Diagnostics &diagnostics);
    Program analyze();
};

#endif //COMPILERV2_SEMANTICANALYZER_H
