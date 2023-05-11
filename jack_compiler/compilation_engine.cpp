#include <iostream>
#include <unordered_set>
#include <vector>

#include "compilation_engine.h"

void CompilationEngine::print(std::string str)
{
    std::cout << str << std::endl;
}

Token CompilationEngine::process()
{
    Token token = tokens_[pos_];
    pos_++;
    return token;
}

Token CompilationEngine::process(std::string str)
{
    Token token = tokens_[pos_];
    if (str != token.value)
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
    if (type != token.type)
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
    Token kindToken = process(eTokenType::Keyword);               // (static|field)
    Token typeToken = process();                                  // (type)
    Token nameToken = process(eTokenType::Identifier);            // varName
    std::vector<Token> nameTokens = processWhile(isNotSemiColon); // (',' varName)*
    process(";");

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
    subroutineSymbolTable_.reset();
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

    subroutineName_ = nameToken.value;
    compileSubroutineBody();
}

void CompilationEngine::compileParameterList()
{
    std::vector<Token> tokens = processWhile(isNotClosing);

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
    process("{");
    auto isVarDec = [](Token t)
    {
        return t.value == "var";
    };
    compileWhile(isVarDec, &CompilationEngine::compileVarDec);

    // generate function code here
    writer_.writeFunction(className_ + "." + subroutineName_, subroutineSymbolTable_.varCount(eKind::VAR));

    compileStatements();
    process("}");
}

void CompilationEngine::compileVarDec()
{
    process("var");                                               // var
    Token typeToken = process();                                  // (type)
    std::vector<Token> nameTokens = processWhile(isNotSemiColon); // varName(',', varName)*
    process(";");                                                 // ;
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
    process("let");              // let
    Token nameToken = process(); // varName
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

    eKind kind = subroutineSymbolTable_.kindOf(nameToken.value);
    int index = subroutineSymbolTable_.indexOf(nameToken.value);
    writer_.writePop(VMWriter::kindToSegment(kind), index);
}

const std::string IF_TRUE_LABEL = "IF_TRUE";
const std::string IF_FALSE_LABEL = "IF_FALSE";
const std::string IF_END_LABEL = "IF_END";

void CompilationEngine::compileIf()
{
    std::string i = std::to_string(ifI_);
    ifI_++;
    process("if");       // if
    process("(");        // (
    compileExpression(); // expression
    process(")");        // )
    process("{");        // {
    writer_.writeIf(IF_TRUE_LABEL + i);
    writer_.writeGoto(IF_FALSE_LABEL + i);
    writer_.writeLabel(IF_TRUE_LABEL + i);
    compileStatements(); // statements
    writer_.writeGoto(IF_END_LABEL + i);
    process("}"); // }

    Token t = peek();
    writer_.writeLabel(IF_FALSE_LABEL + i);
    if (t.value == "else")
    {
        process("else");
        process("{");        // {
        compileStatements(); // statements
        process("}");        // }
    }
    writer_.writeLabel(IF_END_LABEL + i);
}

const std::string WHILE_EXP_LABEL = "WHILE_EXP";
const std::string WHILE_END_LABEL = "WHILE_END";

void CompilationEngine::compileWhile()
{
    int i = whileI_;
    whileI_++;
    writer_.writeLabel(WHILE_EXP_LABEL + std::to_string(i));
    process("while");
    process("(");
    compileExpression();
    writer_.writeArithmetic(eCommand::NOT);               // not
    writer_.writeIf(WHILE_END_LABEL + std::to_string(i)); // fals e case
    process(")");
    process("{");
    compileStatements();
    writer_.writeGoto(WHILE_EXP_LABEL + std::to_string(i));
    process("}");
    writer_.writeLabel(WHILE_END_LABEL + std::to_string(i));
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
    compileTerm();
    auto isOP = [](Token t)
    {
        static const std::unordered_set<std::string> operators = {"+", "-", "*", "/", "&", "|", "<", ">", "="};
        return operators.find(t.value) != operators.end();
    };
    compileWhile(isOP, &CompilationEngine::compileTermWithOP);
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

        if (t.type == eTokenType::Identifier)
        {
            int index;
            eSegment seg;
            eKind kind = subroutineSymbolTable_.kindOf(t.value);
            switch (kind)
            {
            case eKind::ARG:
                seg = eSegment::ARGUMENT;
                break;
            case eKind::VAR:
                seg = eSegment::LOCAL;
                break;
            default:
                seg = eSegment();
                break;
            }
            index = subroutineSymbolTable_.indexOf(t.value);
            writer_.writePush(seg, index);
        }
        else if (t.type == eTokenType::Keyword)
        {
            writer_.writePush(eSegment::CONSTANT, 0); // true
            if (t.value == "true")
            {
                writer_.writeArithmetic(eCommand::NOT); // true == -1
            }
        }
        else if (t.type == eTokenType::IntConst)
        {
            writer_.writePush(eSegment::CONSTANT, std::stoi(t.value));
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
