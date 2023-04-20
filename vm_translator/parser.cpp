#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>

std::string Parser::currentLine()
{
    return lines_[n_];
}

Parser::Parser(std::string input_file_path)
{
    n_ = 0;

    std::ifstream f(input_file_path);
    std::string input;
    if (f)
    {
        std::ostringstream ss;
        ss << f.rdbuf();
        input = ss.str();
    }

    auto ss = std::stringstream(input);
    std::string line;
    while (getline(ss, line))
    {
        // Remove leading and trailing whitespace
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch)
                                              { return !std::isspace(ch); }));
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch)
                                { return !std::isspace(ch); })
                       .base(),
                   line.end());

        // Ignore comments and empty lines
        if (line.empty() || line[0] == '/')
        {
            continue;
        }

        lines_.push_back(line);
    }
}

bool Parser::hasMoreLines()
{
    return n_ < lines_.size();
}

void Parser::advance()
{
    n_++;
}

CommandType Parser::commandType()
{
    std::string line = currentLine();

    static const std::unordered_set<std::string> arithmeticCommands = {
        "add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};

    if (arithmeticCommands.find(line) != arithmeticCommands.end())
    {
        return C_ARITHMETIC;
    }

    if (line.find("push") != std::string::npos)
    {
        return C_PUSH;
    }

    if (line.find("pop") != std::string::npos)
    {
        return C_POP;
    }

    if (line.find("label") != std::string::npos)
    {
        return C_LABEL;
    }

    if (line.find("if-goto") != std::string::npos)
    {
        return C_IF;
    }

    if (line.find("goto") != std::string::npos)
    {
        return C_GOTO;
    }

    if (line.find("function") != std::string::npos)
    {
        return C_FUNCTION;
    }

    if (line.find("return") != std::string::npos)
    {
        return C_RETURN;
    }

    if (line.find("call") != std::string::npos)
    {
        return C_CALL;
    }

    return C_UNKNOWN;
}

std::vector<std::string> split_string(const std::string &input, char delimiter = ' ')
{
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

std::string Parser::arg1()
{
    std::string line = currentLine();
    if (commandType() == C_ARITHMETIC)
    {
        return line;
    }

    std::vector<std::string> tokens = split_string(line);
    return tokens[1];
}

int Parser::arg2()
{
    std::string line = currentLine();
    std::vector<std::string> tokens = split_string(line);
    return std::stoi(tokens[2]);
}
