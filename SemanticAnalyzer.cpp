#include "SemanticAnalyzer.h"
#include <iostream>

using namespace std;

void SemanticAnalyzer::process() {
    for (const Statement &statement : program.statements) {

        if (holds_alternative<VariableDeclaration>(statement.value)) {

            const VariableDeclaration &declaration = std::get<VariableDeclaration>(statement.value);
            if (variables.contains(declaration.name)) {
                diagnostics.error(
                    "Variable \'" + declaration.name + "\' is already defined",
                    declaration.location
                );
            }
            else {
                variables.insert({declaration.name, declaration.type});
            }
        }

        else if (holds_alternative<Assignment>(statement.value)) {
            const Assignment &assignment = std::get<Assignment>(statement.value);

            if (variables.contains(assignment.name)) {
                assignments.insert(assignment.name);

                const TokenType variableType = variables.at(assignment.name);
                const TokenType expressionType = getExpressionType(assignment.value);


                if (variableType != expressionType) {
                    diagnostics.error(
                        "Type error: Expected " +  tokenTypeToString(variableType)
                        + ", got " + tokenTypeToString(expressionType),
                        assignment.value.location //changed this
                    );
                }
            }
            else {
                diagnostics.error(
                    "Variable \'" + assignment.name + "\' was not declared",
                    assignment.location
                );
            }
        }

        else if (holds_alternative<Exit>(statement.value)) {
            const Exit &exitCall = std::get<Exit>(statement.value);
            TokenType exp = getExpressionType(exitCall.value); //see if the expression is valid
            if (exp != TokenType::IntType && exp != TokenType::Identifier) {
                cerr << "Exit code must be an integer literal, or an int type variable" << endl;
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
    if (holds_alternative<bool>(expression.value)) {
        return TokenType::BoolType;
    }
    if (holds_alternative<string>(expression.value)) {
        //returns the type of the variable in the variables map
        const string &name = std::get<string>(expression.value);

        if (!assignments.contains(name)) {
            diagnostics.error(
            "Operation on variable " + name + " before assignment",
                expression.location
            );
        }
        if (!variables.contains(name)) {
            diagnostics.error(
                name + " was not declared",
                expression.location
            );
        }
        else {
            return variables.at(name);
        }
    }

    if (holds_alternative<std::unique_ptr<UnaryExpression>>(expression.value)) {

        const UnaryExpression &unary = *std::get<std::unique_ptr<UnaryExpression>>(expression.value);
        TokenType operandType = getExpressionType(*unary.operand);

        if (unary.op == TokenType::Not && operandType == TokenType::BoolType) {
            return TokenType::BoolType;
        }
        diagnostics.error(
        "Expression error: negation of a non-bool value",
            unary.operand->location
        );
        return TokenType::Undefined;
    }

    if (holds_alternative<std::unique_ptr<BinaryExpression>>(expression.value)) {

        const BinaryExpression &binary = *std::get<std::unique_ptr<BinaryExpression>>(expression.value);

        TokenType leftType = getExpressionType(*binary.left);
        TokenType rightType = getExpressionType(*binary.right);


        if (leftType != rightType) {
            diagnostics.error(
                "Expression error: incompatible types",
                binary.right->location
            );
            return TokenType::Undefined;
        }

        //const Expression &leftValue = *binary.left;
        const Expression &rightValue = *binary.right;
        TokenType opType = binary.op;
        opGeneral opGeneralType =  getGeneralType(opType);

        switch (opGeneralType) {
            case opGeneral::Arithmetic:
                if (leftType == TokenType::IntType) {
                    checkArithmetic(opType, binary);
                    return TokenType::IntType;
                }
                cerr << "Arithmetic on a non-int type" << endl;
                exit(1);
            case opGeneral::Logic:
                if (leftType == TokenType::BoolType) return TokenType::BoolType;
                cerr << "Logical operation on a non-boolean type" << endl;
                exit(1);
            case opGeneral::Comparison:
                if (leftType == TokenType::IntType) return TokenType::BoolType;
                cerr << "'<' and '>' operators are not supported on non-int types" << endl;
                exit(1);
            case opGeneral::Equality:
                return TokenType::BoolType; // "==" and "!=" are supported for all types
            default:
                cerr << "Unknown operation" << endl;
                exit(1);
        }
    }


    cerr << "Unknown expression type" <<endl;
    exit(1);
}

opGeneral SemanticAnalyzer::getGeneralType(TokenType opType) {
    switch (opType) {
        case TokenType::And:
            return opGeneral::Logic;
        case TokenType::Or:
            return opGeneral::Logic;
        case TokenType::Not:
            return opGeneral::Logic;
        case TokenType::Equals:
            return opGeneral::Equality;
        case TokenType::NotEquals:
            return opGeneral::Equality;
        case TokenType::GreaterThan:
            return opGeneral::Comparison;
        case TokenType::LessThan:
            return opGeneral::Comparison;
        case TokenType::Add:
            return opGeneral::Arithmetic;
        case TokenType::Subtract:
            return opGeneral::Arithmetic;
        case TokenType::Multiply:
            return opGeneral::Arithmetic;
        case TokenType::Divide:
            return opGeneral::Arithmetic;
        default:
            cerr << "Unknown general expression type" << endl;
            exit(1);
    }
}

//For now just checks division by 0
void SemanticAnalyzer::checkArithmetic(TokenType opType, const BinaryExpression &binary) {
    const Expression &rightValue = *binary.right;
    if (opType == TokenType::Divide) {
        if (holds_alternative<int>(rightValue.value) && std::get<int>(rightValue.value) == 0) {
            cerr << "Division by 0" << endl;
            exit(1);
        }
    }
}

//public
SemanticAnalyzer::SemanticAnalyzer(Program program, Diagnostics &diagnostics) :
    program(std::move(program)),
    diagnostics(diagnostics) {}

Program SemanticAnalyzer::analyze() {
    process();
    return std::move(program);
}
