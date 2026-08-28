#include "IR.h"
#include <iostream>

void IRGenerator::process() {
    for (const Statement &statement : parsedProg.statements) {
        generateStatement(statement);
    }
}

void IRGenerator::generateStatement(const Statement &statement) {
        Statement newStatement;
        if (holds_alternative<Assignment>(statement.value)) {
            const Assignment &assignment = std::get<Assignment>(statement.value);
            generateAssignment(assignment);
        }
        if (std::holds_alternative<Exit>(statement.value)) {
            const Exit &exitCall = std::get<Exit>(statement.value);
            generateExit(exitCall);
            return;
        }
        if (std::holds_alternative<IfStatement>(statement.value)) {
            const IfStatement &ifStatement = std::get<IfStatement>(statement.value);
            generateIf(ifStatement);
        }
}

void IRGenerator::generateAssignment(const Assignment &assignment) {
    IRValue varValue = generateExpression(assignment.value);

    IRInstruction assignInstruction;
    assignInstruction.op = IROp::Move;
    assignInstruction.left = varValue;
    assignInstruction.destination = assignment.name;
    irProg.instructions.push_back(std::move(assignInstruction));
}

void IRGenerator::generateIf(const IfStatement &ifStatement) {
    //the if

    IRInstruction ifInstruction;
    std::string label = newLabel();

    ifInstruction.op = IROp::JumpIfFalse;
    ifInstruction.destination = label;
    ifInstruction.left = generateExpression(ifStatement.condition);

    irProg.instructions.push_back(std::move(ifInstruction));

    for (auto &statement : ifStatement.thenBlock->statements) {
        // if (std::holds_alternative<IfStatement>(statement.value)) {
        //     generateIf(std::get<IfStatement>(statement.value));
        // }
        generateStatement(statement);
    }

    IRInstruction makeLabel;
    makeLabel.destination = label;
    makeLabel.op = IROp::Label;
    irProg.instructions.push_back(makeLabel);
}

void IRGenerator::generateExit(const Exit &exitCall) {
    IRInstruction exitInstruction;
    exitInstruction.op = OpToIROp(TokenType::Exit); // should return Exit
    exitInstruction.left = generateExpression(exitCall.value);
    exitInstruction.destination = "exit";
    irProg.instructions.push_back(std::move(exitInstruction));
}

IRValue IRGenerator::generateExpression(const Expression &expr) {
    IRValue irVal;
    if (holds_alternative<int>(expr.value)) {
        irVal.value = std::get<int>(expr.value);
        return irVal;
    }
    if (holds_alternative<char>(expr.value)) {
        irVal.value = std::get<char>(expr.value);
        return irVal;
    }
    if (holds_alternative<std::string>(expr.value)) {
        irVal.value = std::get<std::string>(expr.value);
        return irVal;
    }
    if (std::holds_alternative<bool>(expr.value)) {
        irVal.value = std::get<bool>(expr.value);
        return irVal;
    }
    if (holds_alternative<std::unique_ptr<UnaryExpression>>(expr.value)) {
        const UnaryExpression &unary = *std::get<std::unique_ptr<UnaryExpression>>(expr.value);

        IROp irOp = OpToIROp(unary.op);
        IRValue operand = generateExpression(*unary.operand);

        IRInstruction instruction;
        instruction.op = irOp;
        instruction.left = operand;
        instruction.destination = newTemporary();

        irVal.value = instruction.destination;
        irProg.instructions.push_back(std::move(instruction));

        return irVal;
    }
    if (holds_alternative<std::unique_ptr<BinaryExpression>>(expr.value)) {
        const BinaryExpression &binary = *std::get<std::unique_ptr<BinaryExpression>>(expr.value);

        TokenType binOp = binary.op;
        IROp irOp = OpToIROp(binOp);

        IRValue left = generateExpression(*binary.left);
        IRValue right = generateExpression(*binary.right);

        IRInstruction instruction;
        instruction.op = irOp;
        instruction.left = left;
        instruction.right = right;
        if (instruction.op != IROp::Label) {
            instruction.destination = newTemporary();
        }
        else {
            instruction.destination = newLabel();
        }

        irVal.value = instruction.destination;

        irProg.instructions.push_back(std::move(instruction));

        return irVal;
    }
    std::cerr << "Unknown expression type" << std::endl;
    exit(1);
}



std::string IRGenerator::newTemporary() {
    return "t" + std::to_string(tempVarCounter++);
}

std::string IRGenerator::newLabel() {
    return "L" + std::to_string(labelCounter++);
}

IROp IRGenerator::OpToIROp(TokenType binOp) {
    switch (binOp) {
        case TokenType::Add:
            return IROp::Add;
        case TokenType::Subtract:
            return IROp::Subtract;
        case TokenType::Multiply:
            return IROp::Multiply;
        case TokenType::Divide:
            return IROp::Divide;
        case TokenType::Equals:
            return IROp::CompareEqual;
        case TokenType::NotEquals:
            return IROp::CompareNotEqual;
        case TokenType::GreaterThan:
            return IROp::CompareGreater;
        case TokenType::LessThan:
            return IROp::CompareLess;
        case TokenType::And:
            return IROp::And;
        case TokenType::Or:
            return IROp::Or;
        case TokenType::Not:
            return IROp::Not;
        case TokenType::If:
            return IROp::JumpIfFalse;
        case TokenType::Exit:
            return IROp::Exit;
        default:
            std::cerr << "Operation not found" << std::endl;
            exit(1);
    }
}

static std::string opToString(IROp op) {
    switch (op) {
        case IROp::Add:
            return "+";
        case IROp::Subtract:
            return "-";
        case IROp::Multiply:
            return "*";
        case IROp::Divide:
            return "/";
        case IROp::And:
            return "&&";
        case IROp::Or:
            return "||";
        case IROp::CompareEqual:
            return "==";
        case IROp::CompareNotEqual:
            return "!=";
        case IROp::CompareLess:
            return "<";
        case IROp::CompareGreater:
            return ">";
        case IROp::JumpIfFalse:
            return "JumpIfFalse";
        case IROp::Label:
            return "Label";
        case IROp::Not:
            return "!";
        case IROp::Exit:
            return "exit";
        default:
            std::cerr << "unknown opType (debug error)" << std::endl;
            exit(1);
    }
}

static std::string irValueToString(const IRValue& value) {
    if (std::holds_alternative<int>(value.value)) {
        return std::to_string(std::get<int>(value.value));
    }
    if (std::holds_alternative<std::string>(value.value)) {
        return std::get<std::string>(value.value);
    }
    if (std::holds_alternative<bool>(value.value)) {
        if (std::get<bool>(value.value)) {
            return "true";
        }
        return "false";
    }
    if (std::holds_alternative<char>(value.value)) {
        return "'" + std::string(1, std::get<char>(value.value)) + "'";
    }
     std::cerr << "Unknown IR value type (debug error)" << std::endl;
    exit(1);
}

void IRGenerator::printIRCode() {
    for (const IRInstruction &instruction : irProg.instructions) {

        if (instruction.op == IROp::Label) {
            std::cout << "Label " << instruction.destination << " ";
        }
        else if (instruction.op == IROp::JumpIfFalse) {
            std::cout << "JumpIfFalse " << irValueToString(instruction.left) << ", " << instruction.destination << " ";
        }
        else {
            std::cout << instruction.destination;
        }

        if (instruction.op == IROp::Move || instruction.op == IROp::Exit) {
            std::cout << " <- ";
        }
        else if (instruction.op == IROp::JumpIfFalse || instruction.op == IROp::Label) {
            std::cout << std::endl;
            continue;
        }
        else {
            std::cout << " = ";
        }

        if (instruction.op == IROp::Not) {
            std::cout << opToString(instruction.op);
            std::cout << irValueToString(instruction.left) << " ";
            std::cout << std::endl;
            continue;
        }

        std::cout << irValueToString(instruction.left) << " ";

        if (instruction.op != IROp::Move && instruction.op != IROp::Exit) {
            std::cout << opToString(instruction.op) << " ";
            if (!instruction.right.has_value()) {
                std::cerr << "Instruction.right is empty (debug error)" << std::endl;
            }
            std::cout << irValueToString(instruction.right.value()) << " ";
        }
        std::cout << std::endl;
    }
}


//public
IRGenerator::IRGenerator(Program parsedProg) {
    this->parsedProg = std::move(parsedProg);
    tempVarCounter = 0;
    labelCounter = 0;
}

IRProgram IRGenerator::generateIR() {
    process();
    printIRCode();
    return irProg;
}
