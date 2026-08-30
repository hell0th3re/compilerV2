#ifndef COMPILERV2_CFGBUILDER_H
#define COMPILERV2_CFGBUILDER_H

#include "IR.h"

struct BasicBlock {
    int id;
    std::vector<IRInstruction> instructions;
    std::vector<int> successors;
};

class CFGBuilder {
    IRProgram ir;
    std::vector<BasicBlock> blocks;
    void makeBlocks();
    //void printBlocks();
public:
    explicit CFGBuilder(const IRProgram &ir);
    std::vector<BasicBlock> build();
};

#endif //COMPILERV2_CFGBUILDER_H
