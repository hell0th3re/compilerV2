#include "CodeGenerator.h"

#include <iostream>
#include <sstream>
#include <utility>

void CodeGenerator::process() {
    std::stringstream codeTemp;

    const size_t stackSize =  ir.instructions.size();

    codeTemp << "push rbp" << std::endl;
    codeTemp << "mov rbp, rsp" << std::endl;
    codeTemp << "sub rsp, " << stackSize*8 << std::endl;
    codeTemp << std::endl;
    code += codeTemp.str();

    for (const IRInstruction& instruction : ir.instructions) {

        if (instruction.op == IROp::Exit) {
            generateExit(instruction);
            return;
        }

        //allocate new vars, and allow redeclarations to reuse things
        if (!locations.contains(instruction.destination)) {
            offset += 8;
            locations.insert({instruction.destination, offset});
        }

        if (instruction.op == IROp::Move) {
            generateMove(instruction, "rax");
            continue;
        }
        if (instruction.op == IROp::Not) {
            generateUnary(instruction, "rax");
        }
        else {
            generateBinary(instruction, "rax");
        }
    }
}

void CodeGenerator::generateExit(const IRInstruction &instruction) {
    IRValue extValue = instruction.left;
    std::string exitMove = loadValue(extValue, "rdi");
    code += "\n" + exitMove;
}

void CodeGenerator::generateMove(const IRInstruction &instruction, const std::string &reg) {
    std::stringstream codeTemp;
    codeTemp << loadValue(instruction.left, reg);
    codeTemp << "mov [rbp - " << locations.at(instruction.destination) << "], " << reg << std::endl;
    code += codeTemp.str();
}



void CodeGenerator::generateUnary(const IRInstruction& instruction, const std::string &reg) {
    std::stringstream codeTemp;

    codeTemp << loadValue(instruction.left, reg);
    codeTemp << "xor " << reg << ", 1" << std::endl;
    codeTemp << "mov [rbp - " << locations.at(instruction.destination) << "], " << reg << std::endl;
    code += codeTemp.str();
}

void CodeGenerator::generateBinary(const IRInstruction &instruction, const std::string &reg) {
    std::stringstream codeTemp;

    codeTemp << loadValue(instruction.left, reg);

    if (instruction.right.has_value()) {
        const IRValue &right = *instruction.right;



        if (std::holds_alternative<std::string>(right.value)) {
            std::string valueConv = std::get<std::string>(right.value);

            if (instruction.op == IROp::Divide) {
                codeTemp << "cqo" << std::endl;
                codeTemp << "mov rcx, [rbp - " << locations.at(valueConv) << "]" << std::endl;
                codeTemp << "idiv rcx" << std::endl;
                codeTemp << "mov [rbp - " << locations.at(instruction.destination) << "], " << "rax" << std::endl;
                code += codeTemp.str();
                return;
            }


            codeTemp << irOpToAsm(instruction.op) << " " << reg << ", [rbp - " << locations.at(valueConv) << "]"
            << std::endl;
        }
        else if (std::holds_alternative<int>(right.value)) {
            int valueConv = std::get<int>(right.value);

            if (instruction.op == IROp::Divide) {
                codeTemp << "cqo" << std::endl;
                codeTemp << "mov rcx, " << valueConv << std::endl;
                codeTemp << "idiv rcx" << std::endl;
                codeTemp << "mov [rbp - " << locations.at(instruction.destination) << "], " << "rax" << std::endl;
                code += codeTemp.str();
                return;
            }

            codeTemp << irOpToAsm(instruction.op) << " " << reg << ", " << valueConv << std::endl;
        }
        else if (std::holds_alternative<char>(right.value)) {
            int valueConv = static_cast<unsigned char>(std::get<char>(right.value));
            codeTemp << irOpToAsm(instruction.op) << " " << reg << ", " << valueConv << std::endl;
        }
        else if (std::holds_alternative<bool>(right.value)) {
            int valueConv = std::get<bool>(right.value);
            codeTemp << irOpToAsm(instruction.op) << " " << reg << ", " << valueConv << std::endl;
        }
        else {
            std::cerr << "Failed to load value, unknown variant" << std::endl;
            exit(1);
        }



        if (isComparison(instruction.op)) {
            codeTemp << getSetType(instruction.op) << " al" << std::endl;
            codeTemp << "movzx " << reg << ", al" << std::endl;
            codeTemp << "mov [rbp - " << locations.at(instruction.destination) << "], " << "rax" << std::endl;
            code += codeTemp.str();
            return;
        }
    }

    codeTemp << "mov [rbp - " << locations.at(instruction.destination) << "], " << reg << std::endl;

    code += codeTemp.str();
}

std::string CodeGenerator::loadValue(const IRValue &value, const std::string &reg) const {
    int valueConv = -1;
    std::string identifier;
    if (std::holds_alternative<int>(value.value)) {
        valueConv = std::get<int>(value.value);
    }
    else if (std::holds_alternative<char>(value.value)) {
        valueConv = static_cast<unsigned char>(std::get<char>(value.value));
    }
    else if (std::holds_alternative<bool>(value.value)) {
        valueConv = std::get<bool>(value.value);
    }
    else if (std::holds_alternative<std::string>(value.value)) {
        identifier = std::get<std::string>(value.value);
    }
    else {
        std::cerr << "Failed to load value, unknown variant" << std::endl;
        exit(1);
    }
    if (!identifier.empty()) {
        return "mov " + reg + ", [rbp - " + std::to_string(locations.at(identifier)) + "]\n";
    }

    return "mov " + reg + ", " + std::to_string(valueConv) + "\n";
}

std::string CodeGenerator::getSetType(IROp op) {
    switch (op) {
        case IROp::CompareEqual:
            return "sete";
        case IROp::CompareGreater:
            return "setg";
        case IROp::CompareLess:
            return "setl";
        case IROp::CompareNotEqual:
            return "setne";
        default:
            std::cerr << "Unknown comparison operation" << std::endl;
            exit(1);
    }
}

std::string CodeGenerator::irOpToAsm(IROp op) {
    switch (op) {
        case IROp::Add:
            return "add";
        case IROp::Subtract:
            return "sub";
        case IROp::Multiply:
            return "imul";
        case IROp::CompareEqual:
            return "cmp";
        case IROp::CompareGreater:
            return "cmp";
        case IROp::CompareLess:
            return "cmp";
        case IROp::CompareNotEqual:
            return "cmp";
        case IROp::And:
            return "and";
        case IROp::Or:
            return "or";
        case IROp::Not:
            return "not";
        default:
            std::cerr << "operation not implemented yet" << std::endl;
            exit(1);
    }
}

bool CodeGenerator::isComparison(IROp op) {
    switch (op) {
        case IROp::CompareEqual:
            return true;
        case IROp::CompareGreater:
            return true;
        case IROp::CompareLess:
            return true;
        case IROp::CompareNotEqual:
            return true;
        default:
            return false;
    }
}

void CodeGenerator::indent() {
    std::string start = "global _start\nsection .text\n\n_start:\n";
    std::string end = "mov rsp, rbp\n\tpop rbp\n\tmov rax, 60\n\tsyscall";
    std::string result = "\t";

    for (char c : code) {
        result += c;

        if (c == '\n') {
            result += '\t';
        }
    }

    code = start + result + end;
}

//public
CodeGenerator::CodeGenerator(IRProgram ir) {
    offset = 0;
    this->ir = std::move(ir);
}

std::string CodeGenerator::generate() {
    code.clear();
    process();
    indent();
    return code;
}