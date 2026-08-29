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
    Move,
    JumpIfFalse,
    Jump,
    Label,
    Exit
};

struct IRValue {
    std::variant<int, char, bool, std::string> value;
};

struct IRInstruction {
    IROp op;
    std::string destination; //like t0 or L0
    IRValue left;
    std::optional<IRValue> right;
};

/*
    Add:
        destination = result
        left = lhs
        right = rhs

    Move:
        destination = variable
        left = source

    JumpIfFalse:
        destination = label
        left = condition

    Jump:
        destination = label

    Label:
        destination = label

    Exit:
        left = exit value
 */

struct IRProgram {
    std::vector<IRInstruction> instructions;
};

class IRGenerator {
    int tempVarCounter;
    int labelCounter;
    Program parsedProg;
    IRProgram irProg;
    bool exitProg = false;
    void process();
    IRValue generateExpression(const Expression &expr);
    void generateStatement(const Statement &statement);
    void generateAssignment(const Assignment &assignment);
    void generateIf(const IfStatement &ifStatement);
    void generateExit(const Exit &exitCall);
    std::string newTemporary();
    std::string newLabel();
    static IROp OpToIROp(TokenType binOp);
    static std::string irValueToString(const IRValue& value);
    void printIRCode();
public:
    explicit IRGenerator(Program parsedProg);
    IRProgram generateIR();
};

#endif //COMPILERV2_IR_H
