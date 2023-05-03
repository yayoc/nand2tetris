#ifndef HEADER_COMPILATION_ENGINE_H
#define HEADER_COMPILATION_ENGINE_H

#include <fstream>
#include "jack_tokenizer.h"
#include "token.h"
class CompilationEngine
{
private:
    std::ofstream &output_;
    std::vector<Token> tokens_;
    int pos_;
    void print(std::string);
    void printXML(Token);
    void process();
    void process(std::string);
    void process(eTokenType);
    void processWhile(bool (*func)(Token));
    void compileWhile(bool (*condition)(Token), void (CompilationEngine::*method)());
    void compileStatement();
    void compileExpressionWithComma();
    void compileTermWithOP();
    void compileSubroutineCall();
    Token peek(int n = 0);

public:
    CompilationEngine(std::vector<Token> tokens, std::ofstream &output) : output_(output), tokens_(tokens)
    {
        pos_ = 0;
    };
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();
};

#endif // HEADER_COMPILATION_ENGINE_H