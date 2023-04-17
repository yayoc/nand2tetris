#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "parser.h"
#include "code_writer.h"

std::string convert_extension(const std::string &input_file_path)
{
    std::filesystem::path file_path(input_file_path);

    if (file_path.extension() == ".vm")
    {
        file_path.replace_extension(".asm");
        return file_path.string();
    }

    return "";
}

std::string filename_without_extension(const std::string &input_file_path)
{
    std::filesystem::path file_path(input_file_path);
    return file_path.stem().string();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string input_file_path = argv[1];
    std::string output_file_path = convert_extension(input_file_path);
    std::string fn = filename_without_extension(input_file_path);

    if (output_file_path.empty())
    {
        std::cerr << "Error: Input file must have a .vm extension." << std::endl;
        return 1;
    }

    // read content
    std::ifstream f(input_file_path);
    std::string input;
    if (f)
    {
        std::ostringstream ss;
        ss << f.rdbuf();
        input = ss.str();
    }

    Parser parser = Parser(input);

    std::ofstream output_file(output_file_path);
    CodeWriter writer = CodeWriter(output_file);
    writer.setFileName(fn);

    while (parser.hasMoreLines())
    {

        std::string line = parser.currentLine();
        writer.writeComment(line);
        CommandType type = parser.commandType();
        if (type == C_ARITHMETIC)
        {
            writer.writeArithmetic(parser.arg1());
        }
        else if (type == C_PUSH)
        {
            writer.writePushPop("push", parser.arg1(), parser.arg2());
        }
        else if (type == C_POP)
        {
            writer.writePushPop("pop", parser.arg1(), parser.arg2());
        }
        else if (type == C_LABEL)
        {
            writer.writeLabel(parser.arg1());
        }
        else if (type == C_GOTO)
        {
            writer.writeGoto(parser.arg1());
        }
        else if (type == C_IF)
        {
            writer.writeIf(parser.arg1());
        }

        parser.advance();
    }
    writer.close();
}
