#ifndef HEADER_COMPILATION_ENGINE_H
#define HEADER_COMPILATION_ENGINE_H

#include <fstream>
#include <vector>
#include "jack_tokenizer.h"
#include "token.h"
#include "symbol_table.h"
class CompilationEngine
{
private:
    std::ofstream &output_;
    std::vector<Token> tokens_;
    SymbolTable classSymbolTable_;
    SymbolTable subroutineSymbolTable_;
    std::string className_;
    int pos_;
    void print(std::string);
    void printXML(Token);
    void printSymbol(std::string);
    Token process();
    Token process(std::string);
    Token process(eTokenType);
    std::vector<Token> processWhile(bool (*func)(Token));
    void compileWhile(bool (*condition)(Token), void (CompilationEngine::*method)());
    void compileStatement();
    void compileExpressionWithComma();
    void compileTermWithOP();
    void compileSubroutineCall();
    Token peek(int n = 0);

public:
    CompilationEngine(std::vector<Token> tokens, std::ofstream &output) : tokens_(tokens), output_(output)
    {
        pos_ = 0;
        classSymbolTable_ = SymbolTable();
        subroutineSymbolTable_ = SymbolTable();
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