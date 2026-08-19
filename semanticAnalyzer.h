#ifndef COMPILERV2_SEMANTICANALYZER_H
#define COMPILERV2_SEMANTICANALYZER_H
#include <map>

#include "Parser.h"

class SemanticAnalyzer {
    Program program;
    Program programAnalysed;
    void process();
    std::map<std::string, TokenType> variables;

    TokenType getExpressionType(const Expression &expression);
public:
    explicit SemanticAnalyzer(Program program);
    Program analyze();
};

#endif //COMPILERV2_SEMANTICANALYZER_H
