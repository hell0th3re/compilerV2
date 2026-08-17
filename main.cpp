#include <iostream>
#include <fstream>
#include "Lexer.h"

using namespace std;

int main() {
    ifstream file;
    file.open("../file.hi");

    Lexer lexer(file);
    vector <Token> tokens = lexer.getTokens();

    cout << endl;

    file.close();
    return 0;
}
