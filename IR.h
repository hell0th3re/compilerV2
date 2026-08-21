#ifndef COMPILERV2_IR_H
#define COMPILERV2_IR_H

#include <optional>
#include <vector>
#include <string>
#include <variant>

#include "Parser.h"

enum class IROp {
    Add,
    Subtract,
    Multiply,
    Divide,
    CompareEqual,
    CompareNotEqual,
    CompareLess,
    CompareGreater,
    And,
    Or,
    Not,
    Move
};

struct IRValue {
    std::variant<int, char, bool, std::string> value;
};

struct IRInstruction {
    IROp op;
    std::string destination; //like t0
    IRValue left;
    std::optional<IRValue> right;
};
struct IRProgram {
    std::vector<IRInstruction> instructions;
};

class IRGenerator {
    int tempVarCounter;
    Program parsedProg;
    IRProgram irProg;
    void process();
    IRValue generateExpression(const Expression &expr);
    std::string newTemporary();
    static IROp OpToIROp(TokenType binOp);
    void printIRCode();
public:
    explicit IRGenerator(Program parsedProg);
    IRProgram generateIR();
};

#endif //COMPILERV2_IR_H
