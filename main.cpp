#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

using namespace std;

int main() {
    ifstream file;
    file.open("../file.hi");

    Lexer lexer(file);
    vector <Token> tokens = lexer.lex();
    cout << " ";
    Parser parser(tokens);
    Program prog = parser.parse();
    cout << " ";
    SemanticAnalyzer semanticAnalyzer(std::move(prog));
    semanticAnalyzer.analyze();
    cout << endl;

    file.close();
    return 0;
}
