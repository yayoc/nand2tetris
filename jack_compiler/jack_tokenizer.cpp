#include "jack_tokenizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <string>

std::string JackTokenizer::consumeRedundantChars()
{
    std::string res;
    res += consumeSpace();
    res += consumeInlineComment();
    res += consumeBlockComment();
    if (res.length() > 0)
    {
        res += consumeRedundantChars();
    }
    return res;
}

std::string JackTokenizer::consumeSpace()
{
    std::string str;
    auto isSpace = [](char a)
    {
        return a == ' ' || a == '\n' || a == '\r' || a == '\t';
    };
    if (isSpace(peek()))
    {

        str += consumeWhile(isSpace);
    }
    return str;
}

std::string JackTokenizer::consumeInlineComment()
{
    std::string str;
    if (peek() == '/' && peek(1) == '/')
    {
        str += consumeChar('/');
        str += consumeChar('/');
        auto isNotNewLine = [](char c)
        {
            return c != '\n';
        };
        str += consumeWhile(isNotNewLine);
        str += consumeChar('\n');
    }
    return str;
}

std::string JackTokenizer::consumeBlockComment()
{
    std::string str;
    if (peek() == '/' && peek(1) == '*' && peek(2) == '*')
    {
        str += consumeChar('/');
        str += consumeChar('*');
        str += consumeChar('*');
        auto isNotEnd = [](char a, char b)
        {
            return a != '*' || b != '/';
        };
        str += consumeWhile(isNotEnd);
        str += consumeChar('*');
        str += consumeChar('/');
    }
    return str;
}

bool JackTokenizer::isEOF()
{
    return pos_ == input_.length() - 1;
}

std::string JackTokenizer::consumeWhile(bool (*func)(char))
{
    std::string str;
    do
    {
        char a = peek();
        if (func(a))
        {
            str += a;
            pos_++;
        }
        else
        {
            break;
        }
    } while (!isEOF());

    return str;
}

std::string JackTokenizer::consumeWhile(bool (*func)(char, char))
{
    std::string str;
    do
    {
        char a = peek();
        char b = peek(1);
        if (func(a, b))
        {
            str += a;
            pos_++;
        }
        else
        {
            break;
        }
    } while (!isEOF());

    return str;
}

char JackTokenizer::peek(int n)
{
    return input_[pos_ + n];
}

char JackTokenizer::consumeChar(char c)
{
    char curChar = peek();
    if (curChar == c)
    {
        pos_++;
    }
    else
    {
        throw "unexpected char:" + std::string(1, curChar) + "should be " + std::string(1, c);
    }
    return curChar;
}

char JackTokenizer::consumeChar()
{
    char curChar = peek();
    pos_++;
    return curChar;
}

JackTokenizer::JackTokenizer(std::string input_file_path)
{
    pos_ = 0;
    tokenType_ = eTokenType::TokenTypeUnknown;
    tokenVal_ = "";
    std::ifstream f(input_file_path);
    if (f)
    {
        std::ostringstream ss;
        ss << f.rdbuf();
        input_ = ss.str();
    }
}

bool JackTokenizer::hasMoreTokens()
{
    return tokenType_ != eTokenType::TokenTypeUnknown;
}

bool isSymbol(char c)
{
    static const std::unordered_set<char> symbols = {
        '{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'};
    return symbols.find(c) != symbols.end();
}

// Gets the next token from the input, and makes it the current token.
void JackTokenizer::advance()
{
    std::string ignored = consumeRedundantChars();
    char c = peek();
    if (isSymbol(c))
    {
        tokenVal_ = consumeChar();
        tokenType_ = eTokenType::Symbol;
    }
    else if (isdigit(c))
    {
        auto isNotDigit = [](char a)
        {
            return isdigit(a) != 0;
        };
        tokenVal_ = consumeWhile(isNotDigit);
        tokenType_ = eTokenType::IntConst;
    }
    else if (c == '"')
    {
        consumeChar('"');
        auto isNotDoubleQuote = [](char a)
        {
            return a != '"';
        };
        tokenVal_ = consumeWhile(isNotDoubleQuote);
        tokenType_ = eTokenType::StringConst;
        consumeChar('"');
    }
    else if (isalpha(c))
    {
        std::string str;
        auto letters = [](char a)
        {
            return isalnum(a) || a == '_';
        };
        str = consumeWhile(letters);
        static const std::unordered_set<std::string> keywords = {
            "class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return"};
        if (keywords.find(str) != keywords.end())
        {
            tokenType_ = eTokenType::Keyword;
        }
        else
        {
            tokenType_ = eTokenType::Identifier;
        }

        tokenVal_ = str;
    }
    else
    {
        tokenType_ = eTokenType::TokenTypeUnknown;
    }
}

eTokenType JackTokenizer::tokenType()
{
    return tokenType_;
}

std::string JackTokenizer::keyword()
{
    return tokenVal_;
}

std::string JackTokenizer::symbol()
{
    return tokenVal_;
}

std::string JackTokenizer::identifier()
{
    return tokenVal_;
}

int JackTokenizer::intVal()
{
    return std::stoi(tokenVal_);
}

std::string JackTokenizer::stringVal()
{
    return tokenVal_;
}
