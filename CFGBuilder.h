#ifndef COMPILERV2_CFGBUILDER_H
#define COMPILERV2_CFGBUILDER_H

#include <unordered_map>
#include "IR.h"

struct BasicBlock {
    int id;
    std::vector<IRInstruction> instructions;
    std::vector<int> successors;
};

class CFGBuilder {
    std::unordered_map<std::string, BasicBlock> labelToBlock;
    IRProgram ir;
    std::vector<BasicBlock> blocks;
    void makeBlocks();
    std::vector<int> findSuccessors(const BasicBlock &block);
    int findIDByLabel(const std::string &label);
    void setSuccessors();
    //void printBlocks();
public:
    explicit CFGBuilder(const IRProgram &ir);
    std::vector<BasicBlock> build();
};

#endif //COMPILERV2_CFGBUILDER_H
