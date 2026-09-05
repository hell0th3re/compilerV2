#include <iostream>
#include <algorithm>
#include <map>
#include "CFGBuilder.h"

using std::cout;

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

        for (const auto &successor : block.successors) {
            blocks.at(successor).predecessors.push_back(block.id);
        }
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
            bBlock.instructions = std::move(collected);

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
        endBlock.instructions = std::move(collected);

        blocks.push_back(endBlock);
    }
}

void CFGBuilder::printGraph() {
    cout << "\n";
    for (const auto &block : blocks) {
        cout << "B" + std::to_string(block.id) << ": \n";
        for (int successor : block.successors) {
            cout << "   | B" + std::to_string(successor) << "\n";
        }
    }
}

std::vector<int> CFGBuilder::findUnreachable() const {
    std::vector<int> unreachable;
    for (const auto &block : blocks) {
        if (!visited.contains(block.id)) {
            unreachable.push_back(block.id);
        }
    }
    return unreachable;
}

using InitSet = std::set<std::string>;

InitSet CFGBuilder::intersect(const InitSet &a, const InitSet &b) {
    InitSet result;

    std::set_intersection(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );

    return result;
}


InitSet CFGBuilder::getIncomingState(const BasicBlock &block) {
    // Entry block.
    if (block.id == 0) {
        return {};
    }

    if (block.predecessors.empty()) {
        return {};
    }

    // Start with the state from the first predecessor.
    InitSet result = out.at(block.predecessors.at(0));

    // A variable is definitely initialized only if it is
    // initialized on all incoming paths.
    for (size_t i = 1; i < block.predecessors.size(); i++) {
        result = intersect(result, out.at(block.predecessors.at(i)));
    }

    return result;
}

void CFGBuilder::checkRead(const IRValue &value, const InitSet &state) {
    if (std::holds_alternative<std::string>(value.value)) {
        const auto &name = std::get<std::string>(value.value);

        if (!state.contains(name)) {
            uninitialised.push_back(name);
        }
    }
}

InitSet CFGBuilder::transfer(const BasicBlock &block, InitSet state) {
    for (const auto &instruction : block.instructions) {
        switch (instruction.op) {
            case IROp::Move:
                checkRead(instruction.left, state);
                state.insert(instruction.destination);
                break;

            case IROp::Add:
            case IROp::Subtract:
            case IROp::Multiply:
            case IROp::Divide:
            case IROp::CompareEqual:
            case IROp::CompareNotEqual:
            case IROp::CompareLess:
            case IROp::CompareGreater:
            case IROp::And:
            case IROp::Or:
                checkRead(instruction.left, state);
                if (instruction.right.has_value()) {
                    checkRead(*instruction.right, state);
                }
                state.insert(instruction.destination);
                break;

            case IROp::Not:
                checkRead(instruction.left, state);
                state.insert(instruction.destination);
                break;

            case IROp::JumpIfFalse:
                checkRead(instruction.left, state);
                break;

            case IROp::Jump:
            case IROp::Label:
                break;
            case IROp::Exit:
                checkRead(instruction.left, state);
                break;
        }
    }

    return state;
}

void CFGBuilder::analyze() {
    for (const auto& block : blocks) {
        in.insert({block.id, InitSet{}}); //maybe emplace? not sure
        out.insert({block.id, InitSet{}});
    }

    bool changed = true;

    while (changed) {
        changed = false;

        for (const auto &block : blocks) {

            InitSet newIn = getIncomingState(block);
            InitSet newOut = transfer(block, newIn);

            if (newIn != in.at(block.id) ||
                newOut != out.at(block.id)) {

                in.at(block.id) = std::move(newIn);
                out.at(block.id) = std::move(newOut);

                changed = true;
            }
        }
    }
}

void CFGBuilder::visit(const int blockId) {
    if (visited.contains(blockId)) {
        return;
    }

    visited.insert(blockId);
    BasicBlock &block = blocks.at(blockId);

    for (const auto &successor : block.successors) {
        visit(successor);
    }
}

//public
CFGBuilder::CFGBuilder(const IRProgram &ir){
    this->ir = ir;
}

std::vector<BasicBlock> CFGBuilder::build() {
    blocks.clear();
    visited.clear();
    in.clear();
    out.clear();
    uninitialised.clear();
    makeBlocks();
    setSuccessors();

    printGraph();

    cout << "\n";

    if (!blocks.empty()) {
        visit(blocks.at(0).id);
    }

    analyze();
    vector<int> unreachable = findUnreachable();

    if (!unreachable.empty()) {
        cout << "WARNING: Unreachable Code" << '\n';
    }

    sort(uninitialised.begin(), uninitialised.end());
    auto it = std::unique(uninitialised.begin(), uninitialised.end());
    uninitialised.erase(it, uninitialised.end());

    if (!uninitialised.empty()) {
        for (auto &var : uninitialised) {
            cout << "WARNING: Variable: " << var << " might not be initialised\n";
        }
    }

    return blocks;
}
