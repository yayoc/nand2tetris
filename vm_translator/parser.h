#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

enum CommandType
{
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_UNKNOWN
};

class Parser
{
private:
    int n_;
    std::vector<std::string> lines_;
    std::string currentLine();

public:
    Parser(std::string input);
    bool hasMoreLines();
    void advance();
    CommandType commandType();
    std::string arg1();
    int arg2();
};

#endif