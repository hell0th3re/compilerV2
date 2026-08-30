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

void CFGBuilder::setSuccessors() {
    //Go over each instruction in each block, get the associated label, and fill the map.
    for (const auto &block : blocks) {
        for (const auto &instruction : block.instructions) {
            if (instruction.op == IROp::Label) {
                labelToBlock.insert({instruction.destination, block.id});
            }
        }
    }

    //Get the values from the map and set the successor field in the blocks
    for (auto &block : blocks) {
        block.successors = findSuccessors(block);
    }
}

std::vector<int> CFGBuilder::findSuccessors(const BasicBlock &block) {
    std::vector<int> result;

    if (block.instructions.back().op == IROp::Jump) {
        //1 successor block
        int succBlockId = labelToBlock.at(block.instructions.back().destination);
        result.push_back(succBlockId);
    }
    else if (block.instructions.back().op == IROp::JumpIfFalse) {
        //2 successor blocks
        int succBlockId = labelToBlock.at(block.instructions.back().destination); //optional label successor
        //next block default successor
        result.push_back(succBlockId);
        if (block.id + 1 < blocks.size()) {
            result.push_back(block.id+1);
        }
    }
    else if (block.instructions.back().op == IROp::Exit) {
        return result;
    }
    else {
        if (block.id + 1 < blocks.size()) {
            result.push_back(block.id+1);
        }
    }

    return result;
}

void CFGBuilder::makeBlocks() {
    int blockCount = 0;
    vector <IRInstruction> collected;

    for (const auto &instruction : ir.instructions) {

        if (!isTerminator(instruction.op) && instruction.op != IROp::Label) {
            //collect the instruction
            collected.push_back(instruction);
        }
        else if (instruction.op == IROp::Label) {
            if (!collected.empty()) {
                BasicBlock bBlock;
                bBlock.id = blockCount;
                bBlock.instructions = std::move(collected);
                blocks.push_back(bBlock); //push the block
                blockCount++;
            }

            collected.clear();
            collected.push_back(instruction); //sneak the label in as the first instruction
        }
        else {
            //push the collected
            BasicBlock bBlock;
            bBlock.id = blockCount;
            for (const auto &instr : collected) {
                bBlock.instructions.push_back(instr);
            }

            bBlock.instructions.push_back(instruction); //push the terminator
            blocks.push_back(bBlock); //push the block
            blockCount++;
            collected.clear();
        }
    }

    //process the collected unpushed instructions
    if (!collected.empty()) {
        BasicBlock endBlock;
        endBlock.id = blockCount;
        for (const auto &instruction : collected) {
            endBlock.instructions.push_back(instruction);
        }
        blocks.push_back(endBlock);
    }
}

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
    blocks.clear();
    makeBlocks();
    setSuccessors();
    return blocks;
}
