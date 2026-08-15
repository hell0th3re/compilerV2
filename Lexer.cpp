#include "Lexer.h"
#include <iostream>
#include <fstream>
#include <ostream>

using namespace std;

void Lexer::readWords() {
    char ch;
    string buffer;

    while (file.get(ch)) {

        if (ch == ' ' || ch == ';' || ch == ':') {

            // Save whatever word we've built so far
            if (!buffer.empty()) {
                words.push_back(buffer);
                buffer.clear();
            }

            // If it's punctuation, save it as its own word
            if (ch == ';' || ch == ':' || ch == ' ') {
                words.push_back(string(1, ch));
            }
        }
        else {
            buffer += ch;
        }
    }

    // Save the final word
    if (!buffer.empty()) {
        words.push_back(buffer);
    }
}

bool Lexer::isIdentifier(string &word) {

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

bool Lexer::isInteger(string &word) {

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

bool Lexer::isChar(string &word) {

    if (word.empty()) {
        return false;
    }


    if (word.length() != 3) {
        return false;
    }

    if (!(word[0] == '\'' && word[2] == '\'')) {
        return false;
    }

    return true;
}

void Lexer::tokenize() {
    Token token;

    for (int i = 0; i < words.size(); i++) {

        if (keywords.contains(words[i])) {
            TokenType type = keywords[words[i]];
            token ={
                .type = type,
                .value = words[i]
            };
            tokens.push_back(token);
        }

        else if (isIdentifier(words[i])) {
            token = {
                .type = TokenType::Identifier,
                .value = words[i]
            };
            tokens.push_back(token);
        }

        else if (isInteger(words[i])) {
            token = {
                .type = TokenType::Integer,
                .value = words[i]
            };
            tokens.push_back(token);
        }

        else if (isChar(words[i])) {
            token = {
                .type = TokenType::Character,
                .value = words[i]
            };
            tokens.push_back(token);
        }
    }

}

//public
Lexer::Lexer(ifstream &file) {
    this -> file = std::move(file);
    readWords();
    tokenize();
}

vector <Token> Lexer::getTokens() {
    return tokens;
}