#include <iostream>
#include <fstream>
#include <algorithm>

#include "CodeGenerator.h"
#include "Diagnostics.h"
#include "IR.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "CFGBuilder.h"

using namespace std;

class Compiler {
    vector <Token> tokens;
    Program prog;
    Program progChecked;
    IRProgram irProg;
    string code;
    Diagnostics diagnostics;

    void printDiag() {
        vector<Diagnostic> sorted = diagnostics.getAll();

        std::sort(sorted.begin(), sorted.end(),[](const Diagnostic& a, const Diagnostic& b) {
            if (a.location.line != b.location.line) {
                return a.location.line < b.location.line;
            }
            return a.location.column < b.location.column;
        });

        for (const auto& el : sorted) {
            string severity;

            if (el.severity == Severity::Error) {
                severity = "Error ";
            }
            else if (el.severity == Severity::Warning) {
                severity = "Warning ";
            }

            cerr << severity << "on line " << el.location.line << ", column " << el.location.column << ":" << endl;
            cerr << el.message << endl;
            cerr << endl;
        }
    }

    public:
    void compile(const string &fileInName, const string &fileOutName) {
        ifstream file;
        file.open("../" + fileInName);

        if (!file) {
            cerr << "Error opening file " << fileInName << endl;
            exit(1);
        }

        Lexer lexer(file, diagnostics);
        tokens = lexer.lex();

        Parser parser(tokens, diagnostics);
        prog = parser.parse();

        SemanticAnalyzer semanticAnalyzer(std::move(prog), diagnostics);
        progChecked = semanticAnalyzer.analyze();

        if (!diagnostics.getAll().empty()) {
            printDiag();
            exit(1);
        }

        IRGenerator generator(std::move(progChecked));
        irProg = generator.generateIR();

        CFGBuilder builder(irProg);
        vector<BasicBlock> blocks = builder.build();

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
