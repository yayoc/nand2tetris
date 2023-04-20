#include <map>
#include "code_writer.h"

CodeWriter::CodeWriter(std::ofstream &output) : output_(output)
{
    i_ = 0;
}

void CodeWriter::setFileName(std::string filename)
{
    filename_ = filename;
}

std::string CodeWriter::ADD = R"(
@SP
AM=M-1
D=M
A=A-1
M=D+M
)";

std::string CodeWriter::SUB = R"(
@SP
AM=M-1
D=M
A=A-1
M=M-D
)";

std::string CodeWriter::AND = R"(
@SP
AM=M-1
D=M
A=A-1
M=D&M
)";

std::string CodeWriter::OR = R"(
@SP
AM=M-1
D=M
A=A-1
M=D|M
)";

std::string CodeWriter::NEG = R"(
@SP
A=M-1
M=-M
)";

std::string CodeWriter::NOT = R"(
@SP
A=M-1
M=!M
)";

// *SP=D, SP++
std::string CodeWriter::PUSH = R"(
@SP
A=M
M=D
@SP
M=M+1
)";

// SP--, D=*SP
std::string CodeWriter::POP = R"(
@SP
M=M-1
A=M
D=M
)";

void CodeWriter::writeArithmetic(std::string command)
{
    std::string a;
    if (command == "add")
    {
        output_ << ADD;
    }
    if (command == "sub")
    {
        output_ << SUB;
    }

    if (command == "neg")
    {
        output_ << NEG;
    }

    if (command == "and")
    {
        output_ << AND;
    }

    if (command == "or")
    {
        output_ << OR;
    }

    if (command == "not")
    {
        output_ << NOT;
    }

    if (command == "eq")
    {
        writeXMinusYToD();
        std::string i_string = std::to_string(i_);
        output_ << "@TRUE_" + i_string << std::endl;
        output_ << "D;JEQ" << std::endl;
        output_ << "@FALSE_" + i_string << std::endl;
        output_ << "D;JNE" << std::endl;
        writeTrueI();
        writeFalseI();
        output_ << "(RET_ADDRESS_CALL" + i_string + ")" << std::endl;

        i_++;
    }

    if (command == "lt")
    {
        writeXMinusYToD();
        std::string i_string = std::to_string(i_);
        output_ << "@TRUE_" + i_string << std::endl;
        output_ << "D;JLT" << std::endl;
        output_ << "@FALSE_" + i_string << std::endl;
        output_ << "D;JGE" << std::endl;
        writeTrueI();
        writeFalseI();
        writeReturnAddr();
    }
    if (command == "gt")
    {
        writeXMinusYToD();
        std::string i_string = std::to_string(i_);
        output_ << "@TRUE_" + i_string << std::endl;
        output_ << "D;JGT" << std::endl;
        output_ << "@FALSE_" + i_string << std::endl;
        output_ << "D;JLE" << std::endl;
        writeTrueI();
        writeFalseI();
        writeReturnAddr();
    }
}

void CodeWriter::writeTrueI()
{
    std::string i_string = std::to_string(i_);
    output_ << "(TRUE_" + i_string + ")" << std::endl;
    output_ << R"(
@SP
A=M-1
M=-1
)";
    output_ << "@RET_ADDRESS_CALL" + i_string << std::endl;
    output_ << "0;JMP" << std::endl;
}

void CodeWriter::writeFalseI()
{
    std::string i_string = std::to_string(i_);
    output_ << "(FALSE_" + i_string + ")" << std::endl;
    output_ << R"(
@SP
A=M-1
M=0
)";
    output_ << "@RET_ADDRESS_CALL" + i_string << std::endl;
    output_ << "0;JMP" << std::endl;
}

void CodeWriter::writeReturnAddr()
{
    output_ << "(RET_ADDRESS_CALL" + std::to_string(i_) + ")" << std::endl;
    i_++;
}

void CodeWriter::writeXMinusYToD()
{
    output_ << R"(
// D=y
@SP
M=M-1
A=M
D=M
// D=x-y
@SP
M=M-1
A=M
D=M-D
@SP
M=M+1
)";
}

std::string CodeWriter::symbol(std::string segment, int index)
{
    static const std::map<std::string, std::string> baseSymbols = {
        {"local", "@LCL"},
        {"argument", "@ARG"},
        {"this", "@THIS"},
        {"that", "@THAT"}};

    if (baseSymbols.find(segment) != baseSymbols.end())
    {
        return baseSymbols.at(segment);
    }

    if (segment == "static")
    {
        return "@" + filename_ + "." + std::to_string(index);
    }

    if (segment == "constant")
    {
        return "@" + std::to_string(index);
    }

    if (segment == "temp")
    {
        return "@R" + std::to_string(5 + index);
    }

    return "";
}

void CodeWriter::writePushPop(std::string command, std::string segment, int index)
{
    if (command == "push")
    {
        translatePush(command, segment, index);
    }
    if (command == "pop")
    {
        translatePop(command, segment, index);
    }
}

void CodeWriter::writeLabel(std::string label)
{
    output_ << "(" << label << ")" << std::endl;
}

void CodeWriter::writeGoto(std::string label)
{
    output_ << "@" << label << std::endl;
    output_ << "0;JMP" << std::endl;
}

void CodeWriter::writeIf(std::string label)
{
    output_ << POP;
    output_ << "@" << label << std::endl;
    output_ << "D;JGT" << std::endl;
}

void CodeWriter::writeFunction(std::string functionName, int nVars)
{
    output_ << "(" + filename_ + "." + functionName + ")" << std::endl;
    for (int i = 0; i < nVars; i++)
    {
        output_ << "@LCL" << std::endl;
        output_ << "D=M" << std::endl;
        output_ << "@" + std::to_string(i) << std::endl;
        output_ << "A=A+D" << std::endl;
        output_ << "M=0" << std::endl;
    }
}

void CodeWriter::writeCall(std::string functionName, int nArgs)
{
    output_ << "@RET_ADDRESS_CALL" + std::to_string(i_) << std::endl;
    output_ << "D=A" << std::endl;
    output_ << PUSH;

    output_ << "@LCL" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << PUSH;

    output_ << "@ARG" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << PUSH;

    output_ << "@THIS" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << PUSH;

    output_ << "@THAT" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << PUSH;

    // ARG=SP-5-nArgs
    output_ << "@5" << std::endl;
    output_ << "D=A" << std::endl;
    output_ << "@" + std::to_string(nArgs) << std::endl;
    output_ << "D=D-A" << std::endl;
    output_ << "@SP" << std::endl;
    output_ << "D=A-D" << std::endl;
    output_ << "@ARG" << std::endl;
    output_ << "M=D" << std::endl;

    // LCL=SP
    output_ << "@SP" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@LCL" << std::endl;
    output_ << "M=D" << std::endl;

    output_ << "@" + filename_ + "." + functionName << std::endl;
    output_ << "0;JMP" << std::endl;
    writeReturnAddr();
}

void CodeWriter::writeReturn()
{
    // R13 = *(LCL - 5)
    output_ << "@LCL" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@5" << std::endl;
    output_ << "A=D-A" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@R13" << std::endl;
    output_ << "M=D" << std::endl;

    // *ARG = pop()
    output_ << POP;
    output_ << "@ARG" << std::endl;
    output_ << "A=M" << std::endl;
    output_ << "M=D" << std::endl;

    // SP = ARG + 1
    output_ << "@ARG" << std::endl;
    output_ << "D=M+1" << std::endl;
    output_ << "@SP" << std::endl;
    output_ << "M=D" << std::endl;

    // THAT = *(LCL - 1), LCL--
    output_ << "@LCL" << std::endl;
    output_ << "AM=M-1" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@THAT" << std::endl;
    output_ << "M=D" << std::endl;

    // THIS = *(LCL - 1), LCL--
    output_ << "@LCL" << std::endl;
    output_ << "AM=M-1" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@THIS" << std::endl;
    output_ << "M=D" << std::endl;

    // ARG = *(LCL - 1), LCL--
    output_ << "@LCL" << std::endl;
    output_ << "AM=M-1" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@ARG" << std::endl;
    output_ << "M=D" << std::endl;

    // LCL = *(LCL - 1)
    output_ << "@LCL" << std::endl;
    output_ << "A=M-1" << std::endl;
    output_ << "D=M" << std::endl;
    output_ << "@LCL" << std::endl;
    output_ << "M=D" << std::endl;

    output_ << "@R13" << std::endl;
    output_ << "A=M" << std::endl;
    output_ << "0;JMP" << std::endl;
}

void CodeWriter::translatePush(std::string command, std::string segment, int index)
{
    std::string s = symbol(segment, index);
    if (segment == "constant")
    {
        output_ << s << std::endl;
        output_ << "D=A" << std::endl;
        output_ << PUSH << std::endl;
    }
    if (segment == "static")
    {
        output_ << s << std::endl;
        output_ << "D=M" << std::endl;
        output_ << PUSH << std::endl;
    }
    if (segment == "pointer")
    {
        // THIS
        if (index == 0)
        {
            output_ << R"(
@THIS
D=M
)";
            output_ << PUSH;
        }
        // THAT
        else if (index == 1)
        {
            output_ << R"(
@THAT
D=M
)";
            output_ << PUSH;
        }
    }
    if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
    {
        output_ << s << std::endl;
        output_ << "A=M" << std::endl;
        for (int i = 0; i < index; i++)
        {
            output_ << "A=A+1" << std::endl;
        }
        output_ << "D=M" << std::endl;
        output_ << PUSH << std::endl;
    }
    if (segment == "temp")
    {
        output_ << s << std::endl;
        output_ << "D=M" << std::endl;
        output_ << PUSH << std::endl;
    }
}

void CodeWriter::translatePop(std::string command, std::string segment, int index)
{
    std::string s = symbol(segment, index);
    if (segment == "constant")
    {
        output_ << POP;
        output_ << s << std::endl;
        output_ << "M=D" << std::endl;
    }
    if (segment == "static")
    {
        output_ << POP;
        output_ << s << std::endl;
        output_ << "M=D" << std::endl;
    }
    if (segment == "temp")
    {
        output_ << POP;
        output_ << s << std::endl;
        output_ << "M=D" << std::endl;
    }
    if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
    {
        output_ << POP;
        output_ << s << std::endl;
        output_ << "A=M" << std::endl;
        for (int i = 0; i < index; i++)
        {
            output_ << "A=A+1" << std::endl;
        }
        output_ << "M=D" << std::endl;
    }
    if (segment == "pointer")
    {
        output_ << POP;
        if (index == 0)
        {
            output_ << R"(
@THIS
M=D
)";
        }
        else if (index == 1)
        {
            output_ << R"(
@THAT
M=D
)";
        }
    }
}

void CodeWriter::close()
{
    output_.close();
}

void CodeWriter::writeComment(std::string comment)
{
    output_ << "// " << comment << std::endl;
}
