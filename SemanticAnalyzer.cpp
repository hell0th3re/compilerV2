#include "SemanticAnalyzer.h"
#include <iostream>

using namespace std;

void SemanticAnalyzer::process() {
    for (const Statement &statement : program.statements) {
        processStatement(statement);
    }
}

void SemanticAnalyzer::processStatement(const Statement &statement) {
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
    if (getExpressionType(exitCall.value) != TokenType::IntType) {
        cerr << "Exit code must be an integer literal, or an int type variable" << endl;
        exit(1);
    }
}

void SemanticAnalyzer::update(const std::string &name, const Symbol &symbol) {
    if (scopes.back().symbols.contains(name)) {
        scopes.back().symbols.at(name) = symbol;
    }
    else {
        diagnostics.error("Variable not declared", {});
    }

}

void SemanticAnalyzer::processAssignment(const Assignment &assignment) {

    if (lookup(assignment.name) != nullptr) {

        Symbol assigSym = *lookup(assignment.name);

        assigSym.type = lookup(assignment.name)->type;
        assigSym.initialised = true;
        update(assignment.name, assigSym);

        const TokenType variableType = assigSym.type;
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

void SemanticAnalyzer::processVariableDeclaration(const VariableDeclaration &declaration) {
    Symbol declarationSym{};
    declarationSym.type = declaration.type;
    declarationSym.initialised = false;
    declare(declaration.name, declarationSym);
}

void SemanticAnalyzer::enterScope() {
    scopes.push_back(Scope{});
}

void SemanticAnalyzer::leaveScope() {
    scopes.pop_back();
}

void SemanticAnalyzer::declare(const std::string &name, const Symbol &symbol) {
    //if defined in the current scope
    if (scopes.back().symbols.contains(name)) {
        diagnostics.error(
            "Variable \'" + name + "\' is already defined",
            {}
        );
        return;
    }
    scopes.back().symbols.insert({name, symbol});
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

void SemanticAnalyzer::processIfStatement(const IfStatement &statement) {
    if (getExpressionType(statement.condition) != TokenType::BoolType) {
        diagnostics.error(
            "Condition must be of bool type",
            statement.condition.location
        );
    }
    //const Symbol *sym = lookup("x");

    enterScope();
    for (auto &blockStatement : statement.thenBlock->statements) {
        processStatement(blockStatement);
    }
    leaveScope();
    
    if (statement.elseBlock != nullptr) {
        enterScope();
        for (auto &blockStatement : statement.elseBlock->statements) {
            processStatement(blockStatement);
        }
        leaveScope();
    }
    //const Symbol *sym2 = lookup("x");
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
        const string &name = std::get<string>(expression.value);

        const Symbol *temp = lookup(name);

        if (temp != nullptr && temp->initialised == false) {
            diagnostics.error(
            "Operation on variable " + name + " before assignment",
                expression.location
            );
        }
        if (temp == nullptr) {
            diagnostics.error(
                name + " was not declared",
                expression.location
            );
        }
        else {
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


        if (leftType == TokenType::Undefined) {
            diagnostics.error(
                "Expression error: incompatible types",
                binary.right->location
            );
            return TokenType::Undefined;
        }

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
                diagnostics.error("Arithmetic on a non-int type", expression.location);
                return TokenType::Undefined;
            case opGeneral::Logic:
                if (leftType == TokenType::BoolType) return TokenType::BoolType;
                diagnostics.error("Logical operation on a non-boolean type", expression.location);
                return TokenType::Undefined;
            case opGeneral::Comparison:
                if (leftType == TokenType::IntType) return TokenType::BoolType;
                diagnostics.error(
        "'<' and '>' operators are not supported on non-int types",
                    expression.location
                    );
                return TokenType::Undefined;
            case opGeneral::Equality:
                return TokenType::BoolType; // "==" and "!=" are supported for all types
            default:
                diagnostics.error("Unknown operation", expression.location);
                return TokenType::Undefined;
        }
    }
    diagnostics.error("Unknown expression type", expression.location);
    return TokenType::Undefined;
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
    enterScope();
    process();
    leaveScope();
    return std::move(program);
}
