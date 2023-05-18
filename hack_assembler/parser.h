#ifndef PARSER_H
#define PARSER_H

#include <vector>

enum InstructionType
{
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION,
};

class Parser
{
private:
    int n; // line number
    std::vector<std::string> lines;

public:
    Parser (std::string input);
    bool hasMoreLines();
    void advance();
    InstructionType instructionType();
    std::string symbol();
    std::string dest();
    std::string comp();
    std::string jump();

    static bool isNumber(std::string str);
    static std::string convertToBinary(std::string str);
    void reset();
};

#endif
