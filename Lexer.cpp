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

    if (!isalpha(static_cast<unsigned char>(word[0])) && word[0] != '_') {
        return false;
    }

    for (int i = 0; i < word.length(); i++) {
        if (!static_cast<unsigned char>(isalnum(word[i])) && word[i] != '_') {
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
        if (!static_cast<unsigned char>(isdigit(word[i]))) {
            return false;
        }
    }
    return true;
}

TokenType Lexer::getTokenType(const string &tokenVal) {
    if (tokenMap.contains(tokenVal)) {
        TokenType tokenT = tokenMap.at(tokenVal);
        return tokenT;
    }
    return TokenType::Undefined;
}

TokenType Lexer::processWord(const string &word) {
    Token token;
    if (tokenMap.contains(word)) {
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

void Lexer::updateLoc(char ch) {
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

    while (file.get(ch)) {
        //test
        if (ch == '\'') {
            TokenStart = {line, column};
            updateLoc(ch); //update from '

            if (!file.get(ch)) {
                diagnostics.error("Unterminated char literal", TokenStart);
                break;
            }

            updateLoc(ch); //update from character

            std::string value;

            value += ch;

            if (!file.get(ch) || ch != '\'') {
                diagnostics.error("Invalid char literal", TokenStart);

                while (file.get(ch)) {
                    updateLoc(ch);
                    if (ch == '\'')
                        break;
                }

                continue;
            }

            updateLoc(ch);

            token.type = TokenType::Character;
            token.value = value;
            token.location = TokenStart;
            tokens.push_back(token);

            continue;
        }
        //test

        if (static_cast<unsigned char>(isspace(ch))) {
            if (!buffer.empty()) {

                token.type = processWord(buffer);
                if (token.type == TokenType::Undefined) {
                    diagnostics.error(
                        "Unknown token type: " + buffer,
                        TokenStart
                    );
                }

                token.value = buffer;
                token.location = TokenStart;
                tokens.push_back(token);
                buffer.clear();
            }
            updateLoc(ch); //if we want to count whitespace as a column
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
            ch == '{' ||
            ch == '}' ||
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
                updateLoc(ch);
                file.get();
                updateLoc('=');
                continue;
            }
            if (ch == '!' && file.peek() == '=') {
                string op = "!=";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                updateLoc(ch);
                file.get();
                updateLoc('=');
                continue;
            }
            if (ch == '&' && file.peek() == '&') {
                string op = "&&";
                token.value = op;
                token.type = getTokenType(op);
                tokens.push_back(token);
                updateLoc(ch);
                file.get();
                updateLoc('&');
                continue;
            }
            if (ch == '|' && file.peek() == '|') {
                string op = "||";
                token.value = op;
                token.type = getTokenType(op);
                //token.location = TokenStart;
                tokens.push_back(token);
                updateLoc(ch);
                file.get();
                updateLoc('|');
                continue;
            }
            TokenStart = {line, column};

            // Process the seperator token

            string singleCharStr(1, ch);
            token.type = getTokenType(singleCharStr);
            if (token.type == TokenType::Undefined) {
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
        updateLoc(ch);
    }

    // Process the last word
    if (!buffer.empty()) {
        token.type = processWord(buffer);
        if (token.type == TokenType::Undefined) {
            diagnostics.error(
                    "Unknown token type: " + buffer,
                    TokenStart
                );
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
    column(1) {}

vector <Token> Lexer::lex() {
    tokenize();
    return tokens;
}
