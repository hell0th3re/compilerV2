#include <iostream>
#include <fstream>
#include "Lexer.h"

using namespace std;

int main() {
    ifstream file;
    file.open("../file.hi");

    Lexer lexer(file);
    vector <Token> tokens = lexer.getTokens();

    for (int i = 0; i < tokens.size(); i++) {
        cout << tokens[i].value << endl;
    }
    cout << endl;

    file.close();
    return 0;
}
