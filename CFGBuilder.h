#ifndef COMPILERV2_CFGBUILDER_H
#define COMPILERV2_CFGBUILDER_H

#include <unordered_map>
#include <unordered_set>
#include <set>
#include "IR.h"

struct BasicBlock {
    int id;
    std::vector<IRInstruction> instructions;
    std::vector<int> successors;
    std::vector<int> predecessors;
};

class CFGBuilder {
    std::vector<std::string> uninitialised;
    void analyze();
    std::map<int, std::set<std::string>> in;
    std::map<int, std::set<std::string>> out;
    void checkRead(const IRValue &value, const std::set<std::string> &state);
    std::set<std::string> getIncomingState(const BasicBlock &block);
    std::set<std::string> intersect(const std::set<std::string> &a, const std::set<std::string> &b);
    std::set<std::string> transfer(const BasicBlock &block, std::set<std::string> state);
    std::unordered_set<int> visited;
    std::unordered_map<std::string, int> labelToBlock;
    IRProgram ir;
    std::vector<BasicBlock> blocks;
    void makeBlocks();
    std::vector<int> findSuccessors(const BasicBlock &block);
    void setSuccessors();
    void printGraph();
    void visit(int blockId);
    std::vector<int> findUnreachable() const;
public:
    explicit CFGBuilder(const IRProgram &ir);
    std::vector<BasicBlock> build();
};

#endif //COMPILERV2_CFGBUILDER_H
