#include <iostream>
#include <fstream>

#include "CodeGenerator.h"
#include "IR.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

using namespace std;

class Compiler {
    vector <Token> tokens;
    Program prog;
    Program progChecked;
    IRProgram irProg;
    string code;

    public:
    void compile(const string &fileInName, const string &fileOutName) {
        ifstream file;
        file.open("../" + fileInName);

        if (!file) {
            cerr << "Error opening file " << fileInName << endl;
            exit(1);
        }

        Lexer lexer(file);
        tokens = lexer.lex();

        Parser parser(tokens);
        prog = parser.parse();

        SemanticAnalyzer semanticAnalyzer(std::move(prog));
        progChecked = semanticAnalyzer.analyze();

        IRGenerator generator(std::move(progChecked));
        irProg = generator.generateIR();
        std::cout << std::endl;
        CodeGenerator codeGen(std::move(irProg));
        code = codeGen.generate();

        file.close();

        ofstream fileCompiled;
        fileCompiled.open("../" + fileOutName);

        fileCompiled << code;

        fileCompiled.close();
    }
};

int main() {
    Compiler compiler;
    compiler.compile("file.hi", "compiled.asm");

    return 0;
}
