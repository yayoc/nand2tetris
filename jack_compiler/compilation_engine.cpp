#include <iostream>
#include <unordered_set>
#include <vector>

#include "compilation_engine.h"

void CompilationEngine::print(std::string str)
{
    std::cout << str << std::endl;
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
        std::cout << "<keyword> " + value + " </keyword>" << std::endl;
        break;
    case eTokenType::Symbol:
        std::cout << "<symbol> " + value + " </symbol>" << std::endl;
        break;
    case eTokenType::Identifier:
        std::cout << "<identifier> " + value + " </identifier>" << std::endl;
        break;
    case eTokenType::IntConst:
        std::cout << "<integerConstant> " + value + " </integerConstant>" << std::endl;
        break;
    case eTokenType::StringConst:
        std::cout << "<stringConstant> " + value + " </stringConstant>" << std::endl;
        break;
    default:
        break;
    }
}

Token CompilationEngine::process()
{
    Token token = tokens_[pos_];
    printXML(token);
    pos_++;
    return token;
}

Token CompilationEngine::process(std::string str)
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
    return token;
}

Token CompilationEngine::process(eTokenType type)
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
    return token;
}

std::vector<Token> CompilationEngine::processWhile(bool (*func)(Token))
{
    std::vector<Token> tokens = {};
    do
    {
        Token t = peek();
        if (func(t))
        {
            printXML(t);
            pos_++;
            tokens.push_back(t);
        }
        else
        {
            break;
        }

    } while (pos_ < tokens_.size());
    return tokens;
}

int CompilationEngine::compileWhile(bool (*condition)(Token), void (CompilationEngine::*method)())
{
    int c = 0;
    while (true)
    {
        Token t = peek();
        if (condition(t))
        {
            (this->*method)();
            c++;
        }
        else
        {
            break;
        }
    }
    return c;
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

static const std::unordered_map<std::string, eCommand> commands = {{"+", eCommand::ADD}, {"-", eCommand::NEG}, {"&", eCommand::AND}, {"|", eCommand::OR}, {"<", eCommand::LT}, {">", eCommand::GT}, {"=", eCommand::EQ}};

void CompilationEngine::compileTermWithOP()
{
    Token opToken = process(); // op
    compileTerm();
    if (opToken.value == "*")
    {
        writer_.writeCall("Math.multiply", 2);
    }
    else if (opToken.value == "/")
    {
        writer_.writeCall("Math.divide", 2);
    }
    else
    {
        writer_.writeArithmetic(commands.at(opToken.value));
    }
}

void CompilationEngine::compileSubroutineCall()
{
    Token t = peek(1);
    std::string subroutineName;
    int c;
    if (t.value == "(")
    {
        Token subroutineNameToken = process(); // subroutineName;
        subroutineName = subroutineNameToken.value;
        process("(");
        c = compileExpressionList();
        process(")");
    }
    else
    {
        Token objNameToken = process(); // (className|varName);
        process(".");
        Token subroutineNameToken = process(); // subroutineName;
        subroutineName = objNameToken.value + "." + subroutineNameToken.value;
        process("(");
        c = compileExpressionList();
        process(")");
    }
    writer_.writeCall(subroutineName, c);
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
    process("class");
    Token classNameToken = process(eTokenType::Identifier);
    className_ = classNameToken.value;
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
}

void CompilationEngine::compileClassVarDec()
{
    print("<classVarDec>");
    Token kindToken = process(eTokenType::Keyword);               // (static|field)
    Token typeToken = process();                                  // (type)
    Token nameToken = process(eTokenType::Identifier);            // varName
    std::vector<Token> nameTokens = processWhile(isNotSemiColon); // (',' varName)*
    process(";");
    print("</classVarDec>");

    classSymbolTable_.define(nameToken.value, typeToken.value, SymbolTable::fromString(kindToken.value));
    for (Token t : nameTokens)
    {
        if (t.type == eTokenType::Identifier)
        {
            classSymbolTable_.define(t.value, typeToken.value, SymbolTable::fromString(kindToken.value));
        }
    }
}

void CompilationEngine::compileSubroutine()
{
    print("<subroutineDec>");
    Token keywordToken = process(eTokenType::Keyword); // (constructor|function|method)
    if (keywordToken.value == "method")
    {
        subroutineSymbolTable_.define("this", className_, eKind::ARG);
    }
    Token typeToken = process(); // (type)
    Token nameToken = process(); // subroutineName
    process("(");
    compileParameterList(); // update symbol table
    process(")");

    writer_.writeFunction(className_ + "." + nameToken.value, subroutineSymbolTable_.varCount(eKind::ARG));

    compileSubroutineBody();
    print("</subroutineDec>");
    subroutineSymbolTable_.reset();
}

void CompilationEngine::compileParameterList()
{
    print("<parameterList>");
    std::vector<Token> tokens = processWhile(isNotClosing);
    print("</parameterList>");

    std::string type;
    for (Token token : tokens)
    {
        if (token.type == eTokenType::Keyword)
        {
            type = token.value;
        }
        if (token.type == eTokenType::Identifier)
        {
            subroutineSymbolTable_.define(token.value, type, eKind::ARG);
        }
    }
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
    process("var");                                               // var
    Token typeToken = process();                                  // (type)
    std::vector<Token> nameTokens = processWhile(isNotSemiColon); // varName(',', varName)*
    process(";");                                                 // ;
    print("</varDec>");

    for (Token t : nameTokens)
    {
        if (t.type == eTokenType::Identifier)
        {
            subroutineSymbolTable_.define(t.value, typeToken.value, eKind::VAR);
        }
    }
}

void CompilationEngine::compileStatements()
{
    auto isStatement = [](Token t)
    {
        return t.value == "let" || t.value == "if" || t.value == "while" || t.value == "do" || t.value == "return";
    };
    compileWhile(isStatement, &CompilationEngine::compileStatement);
}

void CompilationEngine::compileLet()
{
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
}

void CompilationEngine::compileIf()
{
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
}

void CompilationEngine::compileWhile()
{
    process("while");
    process("(");
    compileExpression();
    process(")");
    process("{");
    compileStatements();
    process("}");
}

void CompilationEngine::compileDo()
{
    process("do");
    compileSubroutineCall();
    process(";");
    writer_.writePop(eSegment::TEMP, 0);
}

void CompilationEngine::compileReturn()
{
    process("return");
    Token t = peek();
    if (t.value != ";")
    {
        compileExpression();
    }
    else
    {
        writer_.writePush(eSegment::CONSTANT, 0); // push dummy value for void function
    }
    process(";");
    writer_.writeReturn();
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
    Token t = peek();
    if (t.value == "(") // ( expression )
    {
        process("(");
        compileExpression();
        process(")");
    }
    else if (t.value == "-" || t.value == "~") // (unaryOp term)
    {
        process(); // op
        compileTerm();
        if (t.value == "-")
        {
            writer_.writeArithmetic(eCommand::NEG);
        }
        else if (t.value == "~")
        {
            writer_.writeArithmetic(eCommand::NOT);
        }
    }
    else if (peek(1).value == "(" || peek(1).value == ".") // subroutineCall
    {
        compileSubroutineCall();
    }
    else
    {
        Token t = process();
        Token s = peek();
        if (s.value == "[")
        {
            process("[");
            compileExpression();
            process("]");
        }

        switch (t.type)
        {
        case eTokenType::Identifier:
            break;
        case eTokenType::IntConst:
            writer_.writePush(eSegment::CONSTANT, std::stoi(t.value));
            break;
        default:
            break;
        }
    }
}

int CompilationEngine::compileExpressionList()
{
    int count = 0;
    auto isExpression = [](Token t)
    {
        return t.type == eTokenType::IntConst || t.type == eTokenType::StringConst || t.type == eTokenType::Keyword || t.type == eTokenType::Identifier || t.value == "(" || t.value == "-" || t.value == "~";
    };
    if (isExpression(peek()))
    {
        compileExpression();
        count++;
        auto isComma = [](Token t)
        {
            return t.value == ",";
        };

        count += compileWhile(isComma, &CompilationEngine::compileExpressionWithComma);
    }

    return count;
}
