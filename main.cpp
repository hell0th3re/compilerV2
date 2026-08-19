#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"

using namespace std;

int main() {
    ifstream file;
    file.open("../file.hi");

    Lexer lexer(file);
    lexer.lex();
    vector <Token> tokens = lexer.getTokens();
    Parser parser(tokens);
    parser.parse();
    vector <Variable> vars = parser.getVariables();
    //vector <Assignment> assignments = parser.getAssignments();
    cout << endl;

    file.close();
    return 0;
}
