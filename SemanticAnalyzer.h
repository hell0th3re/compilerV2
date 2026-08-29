#ifndef COMPILERV2_SEMANTICANALYZER_H
#define COMPILERV2_SEMANTICANALYZER_H
#include <map>
#include <unordered_set>
#include "Parser.h"

enum class OpGeneral {
    Arithmetic,
    Comparison,
    Equality,
    Logic,
    Undefined
};

struct Symbol {
    TokenType type;
    bool initialised;
};

struct Scope {
    std::map<std::string, Symbol> symbols;
};

class SemanticAnalyzer {
    Program program;
    void process();
    std::map<std::string, TokenType> variables;
    std::unordered_set<std::string> assignments;
    Diagnostics &diagnostics;
    std::vector<Scope> scopes;
    //void update(const std::string &name, const Symbol &symbol);
    void enterScope();
    void leaveScope();
    bool declare(const std::string &name, const Symbol &symbol);
    const Symbol *lookup(const std::string &name) const;
    Symbol *lookup(const std::string &name);
    void processBlock(const Block &block);
    void processStatement(const Statement &statement);
    void processIfStatement(const IfStatement &statement);
    void processVariableDeclaration(const VariableDeclaration &declaration);
    void processAssignment(const Assignment &assignment);
    void processExit(const Exit &exitCall);
    TokenType getExpressionType(const Expression &expression);
    static OpGeneral getGeneralType(TokenType opType);
    void checkArithmetic(TokenType opType, const BinaryExpression &binary);
public:
    explicit SemanticAnalyzer(Program program, Diagnostics &diagnostics);
    Program analyze();
};

#endif //COMPILERV2_SEMANTICANALYZER_H
