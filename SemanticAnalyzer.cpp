#include "SemanticAnalyzer.h"
#include <iostream>

using namespace std;

void SemanticAnalyzer::process() {
    for (const Statement &statement : program.statements) {
        processStatement(statement);
    }
}

void SemanticAnalyzer::processStatement(const Statement &statement) {

        if (holds_alternative<ErrorStatement>(statement.value)) {
            return;
        }

        if (holds_alternative<VariableDeclaration>(statement.value)) {
            const VariableDeclaration &declaration = std::get<VariableDeclaration>(statement.value);
            processVariableDeclaration(declaration);
        }

        else if (holds_alternative<Assignment>(statement.value)) {
            const Assignment &assignment = std::get<Assignment>(statement.value);
            processAssignment(assignment);
        }

        else if (holds_alternative<Exit>(statement.value)) {
            const Exit &exitCall = std::get<Exit>(statement.value);
            processExit(exitCall);
        }

        else if (holds_alternative<IfStatement>(statement.value)) {
            const IfStatement &ifStatement = std::get<IfStatement>(statement.value);
            processIfStatement(ifStatement);
        }
}

void SemanticAnalyzer::processExit(const Exit &exitCall) {
    TokenType exitExprType = getExpressionType(exitCall.value);
    if (exitExprType != TokenType::IntType && exitExprType != TokenType::Undefined) {
        diagnostics.error("Invalid exit code", exitCall.location);
    }
}

void SemanticAnalyzer::processBlock(const Block &block) {
    enterScope();
    for (const auto &blockStatement : block.statements) {
        processStatement(blockStatement);
    }
    leaveScope();
}

void SemanticAnalyzer::processAssignment(const Assignment &assignment) {

    Symbol *assigSym = lookup(assignment.name);

    if (assigSym == nullptr) {
            diagnostics.error(
            "Variable \'" + assignment.name + "\' was not declared",
            assignment.location
        );
        return;
    }


    const TokenType variableType = assigSym->type;
    const TokenType expressionType = getExpressionType(assignment.value);

    if (expressionType != TokenType::Undefined && variableType != expressionType) {
        diagnostics.error(
            "Type error: Expected " +  tokenTypeToString(variableType)
            + ", got " + tokenTypeToString(expressionType),
            assignment.value.location //changed this
        );
    }
    else {
        assigSym->initialised = true;
    }
}

void SemanticAnalyzer::processVariableDeclaration(const VariableDeclaration &declaration) {
    Symbol declarationSym{};
    if (declaration.type == TokenType::Undefined) {
        return;
    }
    declarationSym.type = declaration.type;
    declarationSym.initialised = false;
    bool isDeclared = declare(declaration.name, declarationSym);
    if (!isDeclared) {
        diagnostics.error(
          "Variable '" + declaration.name + "' is already defined",
          declaration.location
        );
    }
}

void SemanticAnalyzer::enterScope() {
    scopes.push_back(Scope{});
}

void SemanticAnalyzer::leaveScope() {
    scopes.pop_back();
}

bool SemanticAnalyzer::declare(const std::string &name, const Symbol &symbol) {
    //if defined in the current scope
    if (scopes.back().symbols.contains(name)) {
        return false;
    }
    scopes.back().symbols.insert({name, symbol});
    return true;
}

const Symbol *SemanticAnalyzer::lookup(const std::string &name) const {
    for (size_t i = scopes.size(); i > 0; --i) {
        const Scope &scope = scopes[i - 1];

        auto it = scope.symbols.find(name);
        if (it != scope.symbols.end()) {
            return &it->second;
        }
    }

    return nullptr;
}

Symbol *SemanticAnalyzer::lookup(const std::string &name) {
    for (size_t i = scopes.size(); i > 0; --i) {
        Scope &scope = scopes[i - 1];

        auto it = scope.symbols.find(name);
        if (it != scope.symbols.end()) {
            return &it->second;
        }
    }

    return nullptr;
}

void SemanticAnalyzer::processIfStatement(const IfStatement &statement) {
    TokenType conditionType = getExpressionType(statement.condition);

    if (conditionType != TokenType::BoolType && conditionType != TokenType::Undefined) {
        diagnostics.error(
            "Condition must be of bool type",
            statement.condition.location
        );
    }

    processBlock(*statement.thenBlock);

    if (statement.elseBlock != nullptr) {
        processBlock(*statement.elseBlock);
    }

}

TokenType SemanticAnalyzer::getExpressionType(const Expression &expression) {
    if (holds_alternative<ErrorExpression>(expression.value)) {
        return TokenType::Undefined;
    }
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
        const string &name = std::get<string>(expression.value);

        const Symbol *temp = lookup(name);

        if (temp == nullptr) {
            diagnostics.error(
                name + " was not declared",
                expression.location
            );
        }
        else {
            if (!temp->initialised) {
                diagnostics.error(
                "Operation on variable " + name + " before assignment",
                    expression.location
                );
            }
            return temp->type;
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


        if (leftType == TokenType::Undefined || rightType == TokenType::Undefined) {
            return TokenType::Undefined;
        }

        if (leftType != rightType) {
            diagnostics.error(
                "Expression error: incompatible types",
                binary.right->location
            );
            return TokenType::Undefined;
        }

        const Expression &rightValue = *binary.right;
        TokenType opType = binary.op;
        OpGeneral opGeneralType =  getGeneralType(opType);

        switch (opGeneralType) {
            case OpGeneral::Arithmetic:
                if (leftType == TokenType::IntType) {
                    checkArithmetic(opType, binary);
                    return TokenType::IntType;
                }
                diagnostics.error("Arithmetic on a non-int type", expression.location);
                return TokenType::Undefined;
            case OpGeneral::Logic:
                if (leftType == TokenType::BoolType) return TokenType::BoolType;
                diagnostics.error("Logical operation on a non-boolean type", expression.location);
                return TokenType::Undefined;
            case OpGeneral::Comparison:
                if (leftType == TokenType::IntType) return TokenType::BoolType;
                diagnostics.error(
        "'<' and '>' operators are not supported on non-int types",
                    expression.location
                    );
                return TokenType::Undefined;
            case OpGeneral::Equality:
                return TokenType::BoolType; // "==" and "!=" are supported for all types
            default:
                diagnostics.error("Unknown operation", expression.location);
                return TokenType::Undefined;
        }
    }
    diagnostics.error("Unknown expression type", expression.location);
    return TokenType::Undefined;
}

OpGeneral SemanticAnalyzer::getGeneralType(TokenType opType) {
    switch (opType) {
        case TokenType::And:
            return OpGeneral::Logic;
        case TokenType::Or:
            return OpGeneral::Logic;
        case TokenType::Not:
            return OpGeneral::Logic;
        case TokenType::Equals:
            return OpGeneral::Equality;
        case TokenType::NotEquals:
            return OpGeneral::Equality;
        case TokenType::GreaterThan:
            return OpGeneral::Comparison;
        case TokenType::LessThan:
            return OpGeneral::Comparison;
        case TokenType::Add:
            return OpGeneral::Arithmetic;
        case TokenType::Subtract:
            return OpGeneral::Arithmetic;
        case TokenType::Multiply:
            return OpGeneral::Arithmetic;
        case TokenType::Divide:
            return OpGeneral::Arithmetic;
        default:
            return OpGeneral::Undefined;
    }
}

//For now just checks division by 0
void SemanticAnalyzer::checkArithmetic(TokenType opType, const BinaryExpression &binary) {
    const Expression &rightValue = *binary.right;
    if (opType == TokenType::Divide) {
        if (holds_alternative<int>(rightValue.value) && std::get<int>(rightValue.value) == 0) {
            diagnostics.error("Division by zero", binary.right->location);
        }
    }
}

//public
SemanticAnalyzer::SemanticAnalyzer(Program program, Diagnostics &diagnostics) :
    program(std::move(program)),
    diagnostics(diagnostics) {}

Program SemanticAnalyzer::analyze() {
    enterScope();
    process();
    leaveScope();
    return std::move(program);
}
