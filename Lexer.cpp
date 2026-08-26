#include "Lexer.h"
#include <iostream>
#include <fstream>

using namespace std;

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
        token.type = getTokenType(word);
    }
    else if (isInteger(word)) {
        token.type = TokenType::Integer;
    }
    else if (isIdentifier(word)){
        token.type = TokenType::Identifier;
    }
    else {
        return TokenType::Undefined;
    }
    return token.type;
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
                token.type = TokenType::Character; //charLit
                token.value = charBuff;
                inChar = false;
                tokens.push_back(token);

                charBuff.clear();
                buffer.clear();
            }
            if (charIter > 1) {
                cerr << "Char too long" << endl;
                exit(1);
            }
            charIter++;
            continue;
        }
        if (ch == '\'') {
            charIter = 0;
            inChar = true;
            charBuff += ch;
            continue;
        }

        if (isspace(ch)) {
            if (!buffer.empty()) {

                token.type = processWord(buffer);
                if (token.type == TokenType::Undefined) {
                    cerr << "Unknown token type: " << buffer << endl;
                    exit(1);
                    return;
                }

                token.value = buffer;
                tokens.push_back(token);
                buffer.clear();
            }
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
                    cerr << "Unknown token type: " << buffer << endl;
                    exit(1);
                }

                token.value = buffer;
                tokens.push_back(token);

                buffer.clear();
            }
            // End of word by keyword seperator (instead of space)
            if (ch == '=' && file.peek() == '=') {
                string op = "==";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                file.get();
                continue;
            }
            if (ch == '!' && file.peek() == '=') {
                string op = "!=";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                file.get();
                continue;
            }
            if (ch == '&' && file.peek() == '&') {
                string op = "&&";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                file.get();
                continue;
            }
            if (ch == '|' && file.peek() == '|') {
                string op = "||";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                file.get();
                continue;
            }

            // Process the seperator token

            string singleCharStr(1, ch);
            token.type = getTokenType(singleCharStr);
            if (token.type == TokenType::Undefined) {
                cerr << "Unknown seperator: " << singleCharStr << endl;
                exit(1);
            }
            token.value = singleCharStr;
            tokens.push_back(token);
        }
        else {
            buffer += ch;
        }
    }

    if (inChar) {
        cerr << "Unterminated character literal " << endl;
        exit(1);
    }

    // Process the last word
    if (!buffer.empty()) {
        token.type = processWord(buffer);
        if (token.type == TokenType::Undefined) {
            cerr << "Unknown token type: " << buffer << endl;
            exit(1);
        }

        token.value = buffer;
        tokens.push_back(token);
    }
    Token eofTok;
    eofTok.type = TokenType::Eof;
    tokens.push_back(eofTok);
}

//public
Lexer::Lexer(ifstream &file) {
    this -> file = std::move(file);
}

vector <Token> Lexer::lex() {
    tokenize();

    return tokens;
}
