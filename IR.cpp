#include "IR.h"
#include <iostream>
#include <sstream>

void IRGenerator::process() {
    for (const Statement &statement : parsedProg.statements) {
        //if (exitProg) return;
        generateStatement(statement);
    }
}

void IRGenerator::generateStatement(const Statement &statement) {
        if (std::holds_alternative<VariableDeclaration>(statement.value)) {

            const VariableDeclaration &variableDeclaration = std::get<VariableDeclaration>(statement.value);

            if (variableDeclaration.initializer != nullptr) {
                IRValue initValue = generateExpression(*variableDeclaration.initializer);
                IRInstruction initialiseInstruction;
                initialiseInstruction.op = IROp::Move;
                initialiseInstruction.left = initValue;
                initialiseInstruction.destination = variableDeclaration.name;

                irProg.instructions.push_back(std::move(initialiseInstruction));
            }
        }
        else if (holds_alternative<Assignment>(statement.value)) {
            const Assignment &assignment = std::get<Assignment>(statement.value);
            generateAssignment(assignment);
        }
        else if (std::holds_alternative<Exit>(statement.value)) {
            const Exit &exitCall = std::get<Exit>(statement.value);
            generateExit(exitCall);
        }
        else if (std::holds_alternative<IfStatement>(statement.value)) {
            const IfStatement &ifStatement = std::get<IfStatement>(statement.value);
            generateIf(ifStatement);
        }
        else if (std::holds_alternative<WhileLoop>(statement.value)) {
            const WhileLoop &whileLoop = std::get<WhileLoop>(statement.value);
            generateWhile(whileLoop);
        }
        else if (std::holds_alternative<ForLoop>(statement.value)) {
            const ForLoop &forLoop = std::get<ForLoop>(statement.value);
            generateFor(forLoop);
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
    bool hasElse = ifStatement.elseBlock != nullptr;
    IRInstruction ifInstruction;

    if (hasElse) {
        IRInstruction elseLabel;
        elseLabel.op = IROp::Label;
        elseLabel.destination = newLabel();

        IRInstruction endLabel;
        endLabel.op = IROp::Label;
        endLabel.destination = newLabel();

        ifInstruction.op = IROp::JumpIfFalse;
        ifInstruction.destination = elseLabel.destination;
        ifInstruction.left = generateExpression(ifStatement.condition);

        irProg.instructions.push_back(ifInstruction);

        for (auto &statement : ifStatement.thenBlock->statements) {
            generateStatement(statement);
        }

        IRInstruction jumpInstruction;
        jumpInstruction.op = IROp::Jump;
        jumpInstruction.destination = endLabel.destination;

        irProg.instructions.push_back(jumpInstruction);

        irProg.instructions.push_back(elseLabel);

        for (auto &statement : ifStatement.elseBlock->statements) {
            generateStatement(statement);
        }

        irProg.instructions.push_back(endLabel);
    }
    else {
        IRInstruction endLabel;
        endLabel.op = IROp::Label;
        endLabel.destination = newLabel();

        ifInstruction.op = IROp::JumpIfFalse;
        ifInstruction.destination = endLabel.destination;
        ifInstruction.left = generateExpression(ifStatement.condition);

        irProg.instructions.push_back(ifInstruction);

        for (auto &statement : ifStatement.thenBlock->statements) {
            generateStatement(statement);
        }

        irProg.instructions.push_back(endLabel);
    }

}

void IRGenerator::generateFor(const ForLoop &forLoop) {
    //get a value into a variable
    //place a startLabel
    //check the condition, if false jump to endLabel
    //do stuff
    //place an incrementLabel (will be useful for continues)
    //increment
    //jump to the start
    //place the endLabel

   IRValue initExprVal = generateExpression(*forLoop.declaration.initializer);

    IRInstruction placeVarInstruction;
    placeVarInstruction.op = IROp::Move;
    placeVarInstruction.left = initExprVal;
    placeVarInstruction.destination = forLoop.declaration.name;

    irProg.instructions.push_back(placeVarInstruction);

    IRInstruction entryLabel;
    entryLabel.destination = newLabel();
    entryLabel.op = IROp::Label;
    irProg.instructions.push_back(entryLabel);

    IRInstruction exitLabel;
    exitLabel.op = IROp::Label;
    exitLabel.destination = newLabel();

    IRInstruction conditionCheck;
    conditionCheck.op = IROp::JumpIfFalse;
    conditionCheck.left = generateExpression(forLoop.condition);
    conditionCheck.destination = exitLabel.destination;
    irProg.instructions.push_back(conditionCheck);

    for (auto &statement : forLoop.forBlock->statements) {
        generateStatement(statement);
    }

    IRInstruction incrementLabel;
    incrementLabel.op = IROp::Label;
    incrementLabel.destination = newLabel();

    irProg.instructions.push_back(incrementLabel);

    IRInstruction incrementInstruction;
    generateAssignment(forLoop.action);


    IRInstruction jumpBack;
    jumpBack.op = IROp::Jump;
    jumpBack.destination = entryLabel.destination;
    irProg.instructions.push_back(jumpBack);

    irProg.instructions.push_back(exitLabel);
}

void IRGenerator::generateWhile(const WhileLoop &whileLoop) {
    //place a label
    //check the condition, if true continue, if not jump to a end label
    //do stuff
    //jump back to the start label

    IRInstruction entryLabel;
    entryLabel.op = IROp::Label;
    entryLabel.destination = newLabel();

    IRInstruction exitLabel;
    exitLabel.op = IROp::Label;
    exitLabel.destination = newLabel();

    irProg.instructions.push_back(entryLabel);

    //condition check after the label so it updates after every iteration
    IRInstruction conditionCheck;
    conditionCheck.op = IROp::JumpIfFalse;
    conditionCheck.left = generateExpression(whileLoop.condition);
    conditionCheck.destination = exitLabel.destination;

    irProg.instructions.push_back(conditionCheck);

    // stuff
    for (auto &statement : whileLoop.whileBlock->statements) {
        generateStatement(statement);
    }

    IRInstruction jumpBack;
    jumpBack.op = IROp::Jump;
    jumpBack.destination = entryLabel.destination;

    irProg.instructions.push_back(jumpBack);
    irProg.instructions.push_back(exitLabel);
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

        instruction.destination = newTemporary();
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
        case IROp::Jump:
            return "Jump";
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

std::string IRGenerator::irValueToString(const IRValue& value) {
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
        else if (instruction.op == IROp::Jump) {
            std::cout << "Jump " << instruction.destination << " ";
        }
        else {
            std::cout << instruction.destination;
        }

        if (instruction.op == IROp::Move || instruction.op == IROp::Exit) {
            std::cout << " <- ";
        }
        else if (instruction.op == IROp::JumpIfFalse || instruction.op == IROp::Label || instruction.op == IROp::Jump) {
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
    irProg.instructions.clear();
    tempVarCounter = 0;
    labelCounter = 0;
    this->parsedProg = std::move(parsedProg);
}

IRProgram IRGenerator::generateIR() {
    process();
    printIRCode();
    return irProg;
}
