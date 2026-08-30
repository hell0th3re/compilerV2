#include <iostream>
#include "CFGBuilder.h"

static bool isTerminator(const IROp &operation) {
    switch (operation) {
        case IROp::JumpIfFalse:
        case IROp::Jump:
        case IROp::Exit:
            return true;
        default:
            return false;
    }
}

void CFGBuilder::makeBlocks() {
    int blockCount = 0;
    vector <IRInstruction> collected;

    for (const auto &instruction : ir.instructions) {
        BasicBlock bBlock;
        bBlock.id = blockCount;
        if (!isTerminator(instruction.op)) {
            std::cout << "collected non term instruction " << std::endl;
            collected.push_back(instruction);
        }
        else {
            std::cout << "found terminator" << std::endl;
            //push the collecetd
            for (const auto &instr : collected) {
                bBlock.instructions.push_back(instr);
                std::cout << "pushed non term instruction " << std::endl;
            }
            bBlock.instructions.push_back(instruction); //push the terminator
            std::cout << "pushing the block" << std::endl;
            blocks.push_back(bBlock);
            blockCount++;
            collected.clear();
        }
    }

    std::cout << "no more instructions, creating a new block, pushing collected ones" << std::endl;
    BasicBlock endBlock;
    endBlock.id = blockCount;
    for (const auto &instruction : collected) {
        endBlock.instructions.push_back(instruction);
    }
    blocks.push_back(endBlock);
    std::cout << "Pushed";
}

// void CFGBuilder::makeBlocks() {
//     int blockCount = 0;
//     std::vector<IRInstruction>instrBuffer;
//     for (const auto &instruction : ir.instructions) {
//
//         if (isTerminator(instruction.op)) {
//             BasicBlock bBlock;
//             bBlock.id = blockCount;
//             for (const auto &instr : instrBuffer) {
//                 bBlock.instructions.push_back(instr);
//             }
//             bBlock.instructions.push_back(instruction); //add the terminator to the end of the block
//
//             blocks.push_back(bBlock);
//             blockCount++;
//             instrBuffer.clear();
//         }
//         else if (instruction.op == IROp::Label) {
//             //push back the buffer before the label
//             if (instrBuffer.empty()) {
//                 continue;
//             }
//             std::cout << "frgthyjui ";
//             BasicBlock bBlock;
//             bBlock.id = blockCount;
//             for (const auto &instr : instrBuffer) {
//                 bBlock.instructions.push_back(instr);
//             }
//
//             //push the block
//             blocks.push_back(bBlock);
//             blockCount++;
//             instrBuffer.clear();
//
//             instrBuffer.push_back(instruction);
//         }
//         else {
//             instrBuffer.push_back(instruction);
//         }
//     }
//     if (!instrBuffer.empty()) {
//         BasicBlock bBlock;
//         bBlock.id = blockCount;
//         for (const auto &instr : instrBuffer) {
//             bBlock.instructions.push_back(instr);
//         }
//         blocks.push_back(bBlock);
//         instrBuffer.clear();
//     }
// }

// void CFGBuilder::printBlocks() {
//     for (const auto &block : blocks) {
//         std::cout << "B" << block.id << ": \n";
//         for (const auto &instruction : block.instructions) {
//             std::cout << "\t";
//             if (instruction.op == IROp::Label) {
//                 std::cout << "Label " << instruction.destination << " ";
//             }
//             else if (instruction.op == IROp::JumpIfFalse) {
//                 std::cout << "JumpIfFalse " << irValueToString(instruction.left) << ", " << instruction.destination << " ";
//             }
//             else if (instruction.op == IROp::Jump) {
//                 std::cout << "Jump " << instruction.destination << " ";
//             }
//             else {
//                 std::cout << instruction.destination;
//             }
//
//             if (instruction.op == IROp::Move || instruction.op == IROp::Exit) {
//                 std::cout << " <- ";
//             }
//             else if (instruction.op == IROp::JumpIfFalse || instruction.op == IROp::Label || instruction.op == IROp::Jump) {
//                 std::cout << std::endl;
//                 continue;
//             }
//             else {
//                 std::cout << " = ";
//             }
//
//             if (instruction.op == IROp::Not) {
//                 std::cout << opToString(instruction.op);
//                 std::cout << irValueToString(instruction.left) << " ";
//                 std::cout << std::endl;
//                 continue;
//             }
//
//             std::cout << irValueToString(instruction.left) << " ";
//
//             if (instruction.op != IROp::Move && instruction.op != IROp::Exit) {
//                 std::cout << opToString(instruction.op) << " ";
//                 if (!instruction.right.has_value()) {
//                     std::cerr << "Instruction.right is empty (debug error)" << std::endl;
//                 }
//                 std::cout << irValueToString(instruction.right.value()) << " ";
//             }
//             std::cout << std::endl;
//         }
//     }
// }

//public
CFGBuilder::CFGBuilder(const IRProgram &ir){
    this->ir = ir;
}

std::vector<BasicBlock> CFGBuilder::build() {
    makeBlocks();
    return blocks;
}
