#ifndef COMPILERV2_CODEGENERATOR_H
#define COMPILERV2_CODEGENERATOR_H

#include <map>
#include "IR.h"

class CodeGenerator {
    int offset;
    IRProgram ir;
    std::map<std::string, int> locations;
    std::string code;
    void process();
    void generateMove(const IRInstruction& instruction, const std::string &reg);
    void generateUnary(const IRInstruction& instruction, const std::string &reg);
    void generateBinary(const IRInstruction &instruction, const std::string &reg);
    [[nodiscard]] std::string loadValue(const IRValue& value, const std::string &reg) const;
    static std::string irOpToAsm(IROp op);
    static std::string getSetType(IROp op);
    static bool isComparison(IROp op);
    void indent();
public:
    explicit CodeGenerator(IRProgram ir);
    std::string generate();
};

#endif //COMPILERV2_CODEGENERATOR_H
