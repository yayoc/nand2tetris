#include "code_writer.h"

CodeWriter::CodeWriter(std::ofstream &output, std::string filename) : output_(output), filename_(filename)
{
    i_ = 0;
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
        output_ << "(RET_ADDRESS_CALL" + i_string + ")" << std::endl;

        i_++;
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
        output_ << "(RET_ADDRESS_CALL" + i_string + ")" << std::endl;

        i_++;
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
    if (segment == "static")
    {
        return "@" + filename_ + "." + std::to_string(index);
    }

    if (segment == "constant")
    {
        return "@" + std::to_string(index);
    }

    if (segment == "local")
    {
        return "@LCL";
    }

    if (segment == "argument")
    {
        return "@ARG";
    }

    if (segment == "this")
    {
        return "@THIS";
    }

    if (segment == "that")
    {
        return "@THAT";
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

void CodeWriter::translatePush(std::string command, std::string segment, int index)
{
    std::string s = symbol(segment, index);
    if (segment == "constant")
    {
        output_ << s << std::endl;
        output_ << "D=A" << std::endl;
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
        output_ << R"(
            @SP
            M=M-1
            A=M
            D=M
        )";
        output_ << s << std::endl;
        output_ << "M=D" << std::endl;
    }
    if (segment == "temp")
    {
        output_ << R"(
            @SP
            M=M-1
            A=M
            D=M
        )";
        output_ << s << std::endl;
        output_ << "M=D" << std::endl;
    }
    if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
    {
        output_ << R"(
            @SP
            M=M-1
            A=M
            D=M
        )";
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
        output_ << R"(
            @SP
            M=M-1
            A=M
            D=M
        )";
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
