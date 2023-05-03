#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

#include "jack_tokenizer.h"
#include "compilation_engine.h"
#include "token.h"

class Writer
{
private:
    std::ofstream &output_;
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

public:
    Writer(std::ofstream &output) : output_(output)
    {
    }
    void writeTag(std::string tag)
    {
        output_ << "<" + tag + ">";
    }
    void writeEndTag(std::string tag)
    {
        output_ << "</" + tag + ">" << std::endl;
    }
    void writeToken(std::string type, std::string value)
    {
        writeTag(type);
        output_ << " " + escape(value) + " ";
        writeEndTag(type);
    }
    void writeNewLine()
    {
        output_ << std::endl;
    }
};

std::string apend_t(const std::string &input_file_path)
{
    std::size_t last_dot = input_file_path.find_last_of('.');

    if (last_dot == std::string::npos)
    {
        // No valid dot found for the file extension, return the original path
        return input_file_path;
    }

    std::string new_path = input_file_path.substr(0, last_dot);
    new_path += "TT.xml";
    return new_path;
}

std::string get_output_file_path(const std::string &input_path)
{
    return apend_t(input_path);
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

std::vector<std::string> get_jack_files(const std::string &input_path)
{
    if (std::filesystem::is_directory(input_path))
    {
        return get_files_with_extension(input_path, ".jack");
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

    std::vector<std::string> jack_files = get_jack_files(input_path);

    for (const auto &file : jack_files)
    {
        JackTokenizer tokenizer = JackTokenizer(file);
        tokenizer.advance();

        std::string output_file_path = get_output_file_path(file);
        std::cout << "input file: " << file << std::endl;
        std::cout << "output file: " << output_file_path << std::endl;
        std::ofstream output(output_file_path);
        std::vector<Token> tokens = {};

        while (tokenizer.hasMoreTokens())
        {
            Token token;
            switch (tokenizer.tokenType())
            {
            case eTokenType::Keyword:
                token.type = eTokenType::Keyword;
                token.value = tokenizer.keyword();
                break;
            case eTokenType::Symbol:
                token.type = eTokenType::Symbol;
                token.value = tokenizer.symbol();
                break;
            case eTokenType::Identifier:
                token.type = eTokenType::Identifier;
                token.value = tokenizer.identifier();
                break;
            case eTokenType::IntConst:
                token.type = eTokenType::IntConst;
                token.value = std::to_string(tokenizer.intVal()); // Why compiler didn't error when assining intVal to value?
                break;
            case eTokenType::StringConst:
                token.type = eTokenType::StringConst;
                token.value = tokenizer.stringVal();
                break;
            default:
                break;
            }
            tokens.push_back(token);
            tokenizer.advance();
        }
        CompilationEngine engine = CompilationEngine(tokens, output);
        engine.compileClass();
    }
}