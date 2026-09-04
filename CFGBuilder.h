#ifndef COMPILERV2_CFGBUILDER_H
#define COMPILERV2_CFGBUILDER_H

#include <unordered_map>
#include <unordered_set>
#include "IR.h"

struct BasicBlock {
    int id;
    std::vector<IRInstruction> instructions;
    std::vector<int> successors;
};

class CFGBuilder {
    std::unordered_set<int> visited;
    std::unordered_map<std::string, int> labelToBlock;
    IRProgram ir;
    std::vector<BasicBlock> blocks;
    void makeBlocks();
    std::vector<int> findSuccessors(const BasicBlock &block);
    void setSuccessors();
    void printGraph();
    //BasicBlock *findUnreachable();
    void visit(int blockId);
    std::vector<int> findUnreachable() const;
    //void printBlocks();
public:
    explicit CFGBuilder(const IRProgram &ir);
    std::vector<BasicBlock> build();
};

#endif //COMPILERV2_CFGBUILDER_H
