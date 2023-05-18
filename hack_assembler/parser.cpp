#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <regex>

#include "parser.h"

bool isLineComment(std::string str) {
    return str.at(0) == '/' && str.at(1) == '/';
}

Parser::Parser(std::string input) {
    n = 0;
    // split input by lines
    // ignore line comments
    auto ss = std::stringstream(input);
    for (std::string line; getline(ss, line, '\n');)
        // remove white space
        if (!isLineComment(line)) {
            // not sure but compiler throws an error when putting erase before calling isLineComment 
            std::smatch m;
            if (std::regex_search(line, m, std::regex("^( +)?([^ ]+)"))) {
                std::string s = m[2].str();
                s.erase(std::remove(s.begin(), s.end(), '\n'), s.cend()); // remove line break
                s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
                if (!s.empty()) {
                    lines.push_back(s);
                }
            }
        }
}

bool Parser::hasMoreLines() { 
    return n < lines.size();
};

void Parser::advance() { 
    n++;
};

bool isLabel(std::string str) {
    return str.at(0) == '(' && str.at(str.length() - 1) == ')';
}

InstructionType Parser::instructionType() {
    std::string line = lines[n];
    if (line.at(0) == '@')
    {
        return A_INSTRUCTION;
    }
    if (isLabel(line)) {
        return L_INSTRUCTION;
    }
    return C_INSTRUCTION;
}

bool Parser::isNumber(std::string str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}


std::string Parser::convertToBinary(std::string str) {
    int n = stoi(str);
    return std::bitset<16>(n).to_string();
}

std::string Parser::symbol() {
    InstructionType type = instructionType();
    std::string line = lines[n];
    if (type == A_INSTRUCTION) {
        std::string l = line.substr(1, line.length() - 1); // remove @
        if (Parser::isNumber(l)) {
            return Parser::convertToBinary(l);
        }

        return l;
    }

    return line.substr(1, line.length() - 2); // remove () for L_INSTRUCTION
}

std::string Parser::dest() {
    std::string line = lines[n];
    std::size_t found = line.find('=');
    if (found == -1) {
        return "";
    }

    return line.substr(0, found);
}

std::string Parser::comp() {
    std::string line = lines[n];
    std::size_t eq = line.find('=');
    std::size_t semi = line.find(';');
    return line.substr(eq + 1, semi);
}

std::string Parser::jump() {
    std::string line = lines[n];
    std::size_t found = line.find(';');
    if (found == -1) {
        return "";
    }
    return line.substr(found + 1);
}


void Parser::reset() {
    n = 0;
}

