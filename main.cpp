#include <iostream>
#include <fstream>

#include "CodeGenerator.h"
#include "IR.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

using namespace std;

int main() {
    ifstream file;
    file.open("../file.hi");

    Lexer lexer(file);
    vector <Token> tokens = lexer.lex();

    Parser parser(tokens);
    Program prog = parser.parse();

    SemanticAnalyzer semanticAnalyzer(std::move(prog));
    Program progChecked = semanticAnalyzer.analyze();

    IRGenerator generator(std::move(progChecked));
    IRProgram irProg = generator.generateIR();
    std::cout << std::endl;
    CodeGenerator codeGen(std::move(irProg));
    string code = codeGen.generate();

    cout << code << endl;

    file.close();
    return 0;
}
