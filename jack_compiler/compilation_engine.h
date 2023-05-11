#ifndef HEADER_COMPILATION_ENGINE_H
#define HEADER_COMPILATION_ENGINE_H

#include <fstream>
#include <vector>
#include "jack_tokenizer.h"
#include "token.h"
#include "symbol_table.h"
#include "vm_writer.h"
class CompilationEngine
{
private:
    std::vector<Token> tokens_;
    VMWriter writer_;
    SymbolTable classSymbolTable_;
    SymbolTable subroutineSymbolTable_;
    std::string className_;
    std::string subroutineName_;
    int pos_;
    int whileI_;
    int ifI_;
    void print(std::string);
    Token process();
    Token process(std::string);
    Token process(eTokenType);
    std::vector<Token> processWhile(bool (*func)(Token));
    int compileWhile(bool (*condition)(Token), void (CompilationEngine::*method)());
    void compileStatement();
    void compileExpressionWithComma();
    void compileTermWithOP();
    void compileSubroutineCall();
    Token peek(int n = 0);

public:
    CompilationEngine(std::vector<Token> tokens, VMWriter &writer) : tokens_(tokens), writer_(writer)
    {
        pos_ = 0;
        classSymbolTable_ = SymbolTable();
        subroutineSymbolTable_ = SymbolTable();
        whileI_ = 0;
        ifI_ = 0;
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