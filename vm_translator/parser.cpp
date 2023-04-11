#include "parser.h"
#include <iostream>
#include <sstream>

std::string Parser::currentLine()
{
    return lines_[n_];
}

Parser::Parser(std::string input)
{
    n_ = 0;
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

    if (line == "add")
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
