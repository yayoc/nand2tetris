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

std::vector<std::string> get_files_with_extension(const std::string &directory, const std::string &extension)
{
    std::vector<std::string> result;

    for (const auto &entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == extension)
        {
            result.push_back(entry.path().string());
        }
    }

    return result;
}

std::string get_output_file_path(const std::string &input_path)
{
    if (std::filesystem::is_directory(input_path))
    {
        std::string dir_name = filename_without_extension(input_path);
        return input_path + "/" + dir_name + ".asm";
    }
    else if (std::filesystem::is_regular_file(input_path))
    {
        return convert_extension(input_path);
    }

    return "";
}

std::vector<std::string> get_vm_files(const std::string &input_path)
{
    if (std::filesystem::is_directory(input_path))
    {
        return get_files_with_extension(input_path, ".vm");
    }
    else if (std::filesystem::is_regular_file(input_path))
    {
        return {input_path};
    }
    return {};
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_file_path = get_output_file_path(input_path);
    std::vector<std::string> vm_files = get_vm_files(input_path);

    if (vm_files.empty())
    {
        std::cerr << "Error: no available .vm file." << std::endl;
        return 1;
    }

    std::cout << "output_file_path: " << output_file_path << std::endl;
    std::ofstream output_file(output_file_path);
    CodeWriter writer = CodeWriter(output_file);

    for (const auto &file : vm_files)
    {
        Parser parser = Parser(file);
        std::string filename = filename_without_extension(file);
        writer.setFileName(filename);
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
    }

    writer.close();
}
