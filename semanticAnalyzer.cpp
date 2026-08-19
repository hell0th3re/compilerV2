#include "semanticAnalyzer.h"
#include <iostream>
#include <ostream>
#include <utility>

using namespace std;

void SemanticAnalyzer::process() {
    for (const Statement &statement : program.statements) {

        if (holds_alternative<VariableDeclaration>(statement.value)) {

            const VariableDeclaration &declaration = std::get<VariableDeclaration>(statement.value);
            if (variables.contains(declaration.name)) {
                cerr << "Variable " << declaration.name << " is already defined"<< endl;
                exit(1);
            }

            variables.insert({declaration.name, declaration.type});
        }
        else if (holds_alternative<Assignment>(statement.value)) {
            const Assignment &assignment = std::get<Assignment>(statement.value);

            if (!variables.contains(assignment.name)) {
                cerr << assignment.name << " was not declared" << endl;
                exit(1);
            }

            const TokenType variableType = variables.at(assignment.name);
            const TokenType expressionType = getExpressionType(assignment.value);
            if (variableType != expressionType) {
                cerr << "Type error. Expected " <<  tokenTypeToString(variableType)
                << ", got " << tokenTypeToString(expressionType) << endl;
                exit(1);
            }
        }
    }
}

TokenType SemanticAnalyzer::getExpressionType(const Expression &expression) {
    if (holds_alternative<int>(expression.value)) {
        return TokenType::IntType;
    }
    if (holds_alternative<char>(expression.value)) {
        return TokenType::CharType;
    }
    if (holds_alternative<string>(expression.value)) {
        //returns the type of the variable in the variables map
        const string &name = std::get<string>(expression.value);
        return variables.at(name);
    }
    if (holds_alternative<std::unique_ptr<BinaryExpression>>(expression.value)) {
        const BinaryExpression &binary = *std::get<std::unique_ptr<BinaryExpression>>(expression.value);

        TokenType leftType = getExpressionType(*binary.left);
        TokenType rightType = getExpressionType(*binary.right);

        if (leftType == TokenType::CharType && rightType == TokenType::CharType) {
            cerr << "CharType + CharType not supported" << endl;
            exit(1);
        }

        if (leftType != rightType) {
            cerr << "Expression error: incompatible types" << endl;
            exit(1);
        }
        return leftType; //since they're the same type
    }
    cerr << "Unknown expression type" <<endl;
    exit(1);
}

//public
SemanticAnalyzer::SemanticAnalyzer(Program program) {
    this->program = std::move(program);
}

Program SemanticAnalyzer::analyze() {
    process();
    return std::move(this->programAnalysed);
}
