#include <iostream>
#include <unordered_set>

#include "compilation_engine.h"

void CompilationEngine::print(std::string str)
{
    output_ << str << std::endl;
}

std::string escape(std::string str)
{
    if (str == "<")
    {
        return "&lt;";
    }
    if (str == ">")
    {
        return "&gt;";
    }
    if (str == "\"")
    {
        return "&quot;";
    }
    if (str == "&")
    {
        return "&amp;";
    }

    return str;
}

void CompilationEngine::printXML(Token token)
{
    std::string value = escape(token.value);
    switch (token.type)
    {
    case eTokenType::Keyword:
        output_ << "<keyword> " + value + " </keyword>" << std::endl;
        break;
    case eTokenType::Symbol:
        output_ << "<symbol> " + value + " </symbol>" << std::endl;
        break;
    case eTokenType::Identifier:
        output_ << "<identifier> " + value + " </identifier>" << std::endl;
        break;
    case eTokenType::IntConst:
        output_ << "<integerConstant> " + value + " </integerConstant>" << std::endl;
        break;
    case eTokenType::StringConst:
        output_ << "<stringConstant> " + value + " </stringConstant>" << std::endl;
        break;
    default:
        break;
    }
}

void CompilationEngine::process()
{
    Token token = tokens_[pos_];
    printXML(token);
    pos_++;
}

void CompilationEngine::process(std::string str)
{
    Token token = tokens_[pos_];
    if (str == token.value)
    {
        printXML(token);
    }
    else
    {
        std::cout << "unexpected syntax:" + token.value + " should be " + str << std::endl;
        throw "unexpected syntax:" + token.value + " should be " + str;
    }
    pos_++;
}

void CompilationEngine::process(eTokenType type)
{
    Token token = tokens_[pos_];
    if (type == token.type)
    {
        printXML(token);
    }
    else
    {
        throw "unexpected syntax:" + std::to_string(type) + " should be " + std::to_string(type);
    }
    pos_++;
}

void CompilationEngine::processWhile(bool (*func)(Token))
{
    do
    {
        Token t = peek();
        if (func(t))
        {
            printXML(t);
            pos_++;
        }
        else
        {
            break;
        }

    } while (pos_ < tokens_.size());
}

void CompilationEngine::compileWhile(bool (*condition)(Token), void (CompilationEngine::*method)())
{
    while (true)
    {
        Token t = peek();
        if (condition(t))
        {
            (this->*method)();
        }
        else
        {
            break;
        }
    }
}

void CompilationEngine::compileStatement()
{
    Token t = peek();
    if (t.value == "let")
    {
        compileLet();
    }
    else if (t.value == "if")
    {
        compileIf();
    }
    else if (t.value == "while")
    {
        compileWhile();
    }
    else if (t.value == "do")
    {
        compileDo();
    }
    else if (t.value == "return")
    {
        compileReturn();
    }
}

void CompilationEngine::compileExpressionWithComma()
{
    process(",");
    compileExpression();
}

void CompilationEngine::compileTermWithOP()
{
    process();
    compileTerm();
}

void CompilationEngine::compileSubroutineCall()
{
    Token t = peek(1);
    if (t.value == "(")
    {
        process(); // subroutineName;
        process("(");
        compileExpressionList();
        process(")");
    }
    else
    {
        process(); // (className|varName);
        process(".");
        process(); // subroutineName;
        process("(");
        compileExpressionList();
        process(")");
    }
}

Token CompilationEngine::peek(int n)
{
    return tokens_[pos_ + n];
}

auto isNotSemiColon = [](Token t)
{
    return t.value != ";";
};

auto isNotClosing = [](Token t)
{
    return t.value != ")";
};

void CompilationEngine::compileClass()
{
    print("<class>");
    process("class");
    process(eTokenType::Identifier);
    process("{");

    auto isClassVarDec = [](Token t)
    {
        return t.value == "static" || t.value == "field";
    };
    compileWhile(isClassVarDec, &CompilationEngine::compileClassVarDec);

    auto isSubroutine = [](Token t)
    {
        return t.value == "constructor" || t.value == "function" || t.value == "method";
    };
    compileWhile(isSubroutine, &CompilationEngine::compileSubroutine);
    process("}");
    print("</class>");
}

void CompilationEngine::compileClassVarDec()
{
    print("<classVarDec>");
    process(eTokenType::Keyword);    // (static|field)
    process();                       // (type)
    process(eTokenType::Identifier); // varName
    processWhile(isNotSemiColon);    // (',' varName)*
    process(";");
    print("</classVarDec>");
}

void CompilationEngine::compileSubroutine()
{
    print("<subroutineDec>");
    process(eTokenType::Keyword); // (constructor|function|method)
    process();                    // (type)
    process();                    // subroutineName
    process("(");
    compileParameterList();
    process(")");
    compileSubroutineBody();
    print("</subroutineDec>");
}

void CompilationEngine::compileParameterList()
{
    print("<parameterList>");
    processWhile(isNotClosing);
    print("</parameterList>");
}

void CompilationEngine::compileSubroutineBody()
{
    print("<subroutineBody>");
    process("{");
    auto isVarDec = [](Token t)
    {
        return t.value == "var";
    };
    compileWhile(isVarDec, &CompilationEngine::compileVarDec);
    compileStatements();
    process("}");
    print("</subroutineBody>");
}

void CompilationEngine::compileVarDec()
{
    print("<varDec>");
    process("var");               // var
    process();                    // (type)
    processWhile(isNotSemiColon); // varName(',', varName)*
    process(";");                 // ;
    print("</varDec>");
}

void CompilationEngine::compileStatements()
{
    print("<statements>");
    auto isStatement = [](Token t)
    {
        return t.value == "let" || t.value == "if" || t.value == "while" || t.value == "do" || t.value == "return";
    };
    compileWhile(isStatement, &CompilationEngine::compileStatement);
    print("</statements>");
}

void CompilationEngine::compileLet()
{
    print("<letStatement>");
    process("let"); // let
    process();      // varName
    Token t = peek();
    if (t.value == "[")
    {
        process("[");        // [
        compileExpression(); // expression
        process("]");        // ]
    }
    process("="); // =
    compileExpression();
    process(";"); // ;
    print("</letStatement>");
}

void CompilationEngine::compileIf()
{
    print("<ifStatement>");
    process("if");       // if
    process("(");        // (
    compileExpression(); // expression
    process(")");        // )
    process("{");        // {
    compileStatements(); // statements
    process("}");        // }
    Token t = peek();
    if (t.value == "else")
    {
        process("else");
        process("{");        // {
        compileStatements(); // statements
        process("}");        // }
    }
    print("</ifStatement>");
}

void CompilationEngine::compileWhile()
{
    print("<whileStatement>");
    process("while");
    process("(");
    compileExpression();
    process(")");
    process("{");
    compileStatements();
    process("}");
    print("</whileStatement>");
}

void CompilationEngine::compileDo()
{
    print("<doStatement>");
    process("do");
    compileSubroutineCall();
    process(";");
    print("</doStatement>");
}

void CompilationEngine::compileReturn()
{
    print("<returnStatement>");
    process("return");
    Token t = peek();
    if (t.value != ";")
    {
        compileExpression();
    }
    process(";");
    print("</returnStatement>");
}

void CompilationEngine::compileExpression()
{
    print("<expression>");
    compileTerm();
    auto isOP = [](Token t)
    {
        static const std::unordered_set<std::string> operators = {"+", "-", "*", "/", "&", "|", "<", ">", "="};
        return operators.find(t.value) != operators.end();
    };
    compileWhile(isOP, &CompilationEngine::compileTermWithOP);
    print("</expression>");
}

void CompilationEngine::compileTerm()
{
    print("<term>");
    Token t = peek();
    if (t.value == "(") // ( expression )
    {
        process("(");
        compileExpression();
        process(")");
    }
    else if (t.value == "-" || t.value == "~") // (unaryOp term)
    {
        process();
        compileTerm();
    }
    else if (peek(1).value == "(" || peek(1).value == ".") // subroutineCall
    {
        compileSubroutineCall();
    }
    else
    {
        process();
        Token s = peek();
        if (s.value == "[")
        {
            process("[");
            compileExpression();
            process("]");
        }
    }
    print("</term>");
}

int CompilationEngine::compileExpressionList()
{
    print("<expressionList>");
    auto isExpression = [](Token t)
    {
        return t.type == eTokenType::IntConst || t.type == eTokenType::StringConst || t.type == eTokenType::Keyword || t.type == eTokenType::Identifier || t.value == "(" || t.value == "-" || t.value == "~";
    };
    if (isExpression(peek()))
    {
        compileExpression();
        auto isComma = [](Token t)
        {
            return t.value == ",";
        };

        compileWhile(isComma, &CompilationEngine::compileExpressionWithComma);
    }

    print("</expressionList>");

    return 0;
}
