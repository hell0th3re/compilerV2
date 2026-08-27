#include "Lexer.h"
#include <iostream>
#include <fstream>

using std::string;
using std::ifstream;
using std::vector;

bool Lexer::isIdentifier(const string &word) {

    if (word.empty()) {
        return false;
    }

    if (!isalpha(word[0]) && word[0] != '_') {
        return false;
    }

    for (int i = 0; i < word.length(); i++) {
        if (!isalnum(word[i]) && word[i] != '_') {
            return false;
        }
    }

    return true;
}

bool Lexer::isInteger(const std::string &word) {
    if (word.empty()) {
        return false;
    }
    for (int i = 0; i < word.length(); i++) {
        if (!isdigit(word[i])) {
            return false;
        }
    }
    return true;
}

TokenType Lexer::getTokenType(const string &tokenVal) {
    if (keywords.contains(tokenVal)) {
        TokenType tokenT = keywords.at(tokenVal);
        return tokenT;
    }
    return TokenType::Undefined;
}

TokenType Lexer::processWord(const string &word) {
    Token token;
    if (keywords.contains(word)) {
        return getTokenType(word);
    }
    if (isInteger(word)) {
        return TokenType::Integer;
    }
    if (isIdentifier(word)){
        return TokenType::Identifier;
    }
    return TokenType::Undefined;
}

void Lexer::consume(char ch) {
    if (ch == '\n') {
        line++;
        column = 1;
    }
    else {
        column++;
    }
}

void Lexer::tokenize() {
    tokens.clear();
    Token token;
    char ch;
    string buffer;
    string charBuff;
    bool inChar = false;
    int charIter = 0;

    while (file.get(ch)) {
        if (inChar) {
            charBuff += ch;

            if (ch == '\'') {
                if (charIter != 1) {
                    diagnostics.error(
                        "Invalid char literal",
                        TokenStart
                    );
                    //cerr << "Invalid char literal" << endl;
                }
                token.type = TokenType::Character; //charLit
                token.value = charBuff;
                token.location = TokenStart;
                inChar = false;
                tokens.push_back(token);

                charBuff.clear();
                buffer.clear();
            }
            if (charIter > 1) {
                diagnostics.error(
                    "Char too long",
                    TokenStart
                );
                //cerr << "Char too long" << endl;
            }
            consume(ch);
            charIter++;
            continue;
        }
        if (ch == '\'') {
            TokenStart = {line, column};
            consume(ch);
            charIter = 0;
            inChar = true;
            charBuff += ch;
            continue;
        }

        if (isspace(ch)) {
            if (!buffer.empty()) {

                token.type = processWord(buffer);
                if (token.type == TokenType::Undefined) {
                    diagnostics.error(
                        "Unknown token type: " + buffer,
                        TokenStart
                    );
                    //cerr << "Unknown token type: " << buffer << endl;
                }

                token.value = buffer;
                token.location = TokenStart;
                tokens.push_back(token);
                buffer.clear();
            }
            consume(ch); //if we want to count whitespace as a column
            continue;
        }

        bool isSeparator = (
            ch == ';' ||
            ch == ':' ||
            ch == '=' ||
            ch == '+' ||
            ch == '-' ||
            ch == '*' ||
            ch == '/' ||
            ch == '(' ||
            ch == ')' ||
            ch == '>' ||
            ch == '<' ||
            ch == '!' ||
            ch == '&' ||
            ch == '|'
            );

        if (isSeparator) {
            if (!buffer.empty()) {
                token.type = processWord(buffer);
                if (token.type == TokenType::Undefined) {

                    diagnostics.error(
                        "Unknown token type: " + buffer,
                        TokenStart
                    );
                    //cerr << "Unknown token type: " << buffer << endl;
                }
                token.value = buffer;
                token.location = TokenStart;
                tokens.push_back(token);

                buffer.clear();
            }

            token.location = {line, column};

            if (ch == '=' && file.peek() == '=') {
                string op = "==";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                consume(ch);
                file.get();
                consume('=');
                continue;
            }
            if (ch == '!' && file.peek() == '=') {
                string op = "!=";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                consume(ch);
                file.get();
                consume('=');
                continue;
            }
            if (ch == '&' && file.peek() == '&') {
                string op = "&&";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                consume(ch);
                file.get();
                consume('&');
                continue;
            }
            if (ch == '|' && file.peek() == '|') {
                string op = "||";
                token.value = op;
                token.type = getTokenType(op);
                //token.location = TokenStart;
                tokens.push_back(token);
                consume(ch);
                file.get();
                consume('|');
                continue;
            }
            TokenStart = {line, column};

            // Process the seperator token

            string singleCharStr(1, ch);
            token.type = getTokenType(singleCharStr);
            if (token.type == TokenType::Undefined) {
                //cerr << "Unknown seperator: " << singleCharStr << endl;
                diagnostics.error(
                    "Unknown seperator: " + singleCharStr,
                    TokenStart
                );
            }
            token.value = singleCharStr;
            tokens.push_back(token);
        }
        else {
            if (buffer.empty()) {
                TokenStart = {line,column};
            }
            buffer += ch;
        }
        consume(ch);

    }

    if (inChar) {
        diagnostics.error(
            "Unterminated char literal",
            TokenStart
            );
        //cerr << "Unterminated character literal " << endl;
    }

    // Process the last word
    if (!buffer.empty()) {
        token.type = processWord(buffer);
        if (token.type == TokenType::Undefined) {
            diagnostics.error(
                    "Unknown token type: " + buffer,
                    TokenStart
                );
            //cerr << "Unknown token type: " << buffer << endl;
        }

        token.value = buffer;
        token.location = TokenStart;
        tokens.push_back(token);
    }

    Token eofTok;
    eofTok.type = TokenType::Eof;
    eofTok.location = {line, column};
    tokens.push_back(eofTok);
}

//public

//pro tip: references must be intialised, so thats why this->diagnostics = diagnostics makes a copy,
//a contructor initaliser list doesnt have that problem. So if there are no errors we can just check if
//diagnostics.empty() in the compiler driver class

Lexer::Lexer(ifstream &file, Diagnostics &diagnostics) :
    file(std::move(file)),
    diagnostics(diagnostics),
    line(1),
    column(1),
    TokenStart({}) {}

vector <Token> Lexer::lex() {
    tokenize();
    return tokens;
}
