#include "code_writer.h"

CodeWriter::CodeWriter(std::ofstream &output, std::string filename) : output_(output), filename_(filename) {}

void CodeWriter::writeArithmetic(std::string command)
{
    std::string a;
    if (command == "add")
    {
        a = R"(@SP
M=M-1
// D=RAM[M]
A=M
D=M
@SP
M=M-1
A=M
D=D+M
@SP
A=M
M=D
@SP
M=M+1
)";
    }
    output_ << a;
}

std::string CodeWriter::symbol(std::string segment, int index)
{
    if (segment == "static")
    {
        return filename_ + "." + std::to_string(index);
    }
    return "";
}

void CodeWriter::writePushPop(std::string command, std::string segment, int index)
{
    std::string s = symbol(segment, index);
    if (command == "push")
    {
        if (segment == "constant")
        {
            /*
            // D=i
            @i
            D=A
            // *SP=D
            @SP
            A=M
            M=D
            // SP++
            M=M+1
            */
            output_ << "@" + std::to_string(index) << std::endl;
            output_ << "D=A" << std::endl;
            output_ << "@SP" << std::endl;
            output_ << "A=M" << std::endl;
            output_ << "M=D" << std::endl;
            output_ << "@SP" << std::endl;
            output_ << "M=M+1" << std::endl;
        }
    }
    if (command == "pop")
    {
        /*
            // SP--
            @SP
            M=M-1
            // D=SP
            D=M
            // XX.i=D
            @XX.i
            A=M
            M=D
            */
        output_ << R"(
            @SP
            M=M-1
            D=M
        )";
        output_ << "@" + s << std::endl;
        output_ << R"(
            A=M
            M=D
        )";
    }
}

void CodeWriter::close()
{
    output_.close();
}
