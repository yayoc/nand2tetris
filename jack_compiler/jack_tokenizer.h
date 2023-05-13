#ifndef HEADER_JACK_TOKENIZER_H
#define HEADER_JACK_TOKENIZER_H

#include <string>
#include <map>

enum eTokenType
{
    Keyword,
    Symbol,
    Identifier,
    IntConst,
    StringConst,
    TokenTypeUnknown
};



enum eKeyword
{
    Class,
    Method,
    Function,
    Constructor,
    Int,
    Boolean,
    Char,
    Void,
    Var,
    Static,
    Field,
    Let,
    Do,
    If,
    Else,
    While,
    Return,
    True,
    False,
    Null,
    This,
    KeywordUnknown
};

class JackTokenizer
{
private:
    std::string input_;
    int pos_;
    std::string tokenVal_;
    eTokenType tokenType_;

    std::string consumeWhile(bool (*func)(char));
    std::string consumeWhile(bool (*func)(char, char));
    char peek(int n = 0);
    char consumeChar(char);
    char consumeChar();
    std::string consumeRedundantChars();
    std::string consumeSpace();
    std::string consumeInlineComment();
    std::string consumeBlockComment();
    bool isEOF();

public:
    JackTokenizer(std::string input_file_path);
    bool hasMoreTokens();
    void advance();
    eTokenType tokenType();
    std::string keyword();
    std::string symbol();
    std::string identifier();
    int intVal();
    std::string stringVal();
};

#endif // HEADER_JACK_TOKENIZER_H