#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"

using namespace std;

int main() {
    ifstream file;
    file.open("../file.hi");

    Lexer lexer(file);
    vector <Token> tokens = lexer.lex();
    Parser parser(tokens);
    Program prog = parser.parse();
    cout << endl;

    file.close();
    return 0;
}
