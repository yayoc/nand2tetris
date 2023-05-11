#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

#include "jack_tokenizer.h"
#include "compilation_engine.h"
#include "token.h"

std::string get_output_file_path(const std::string &input_path)
{
    std::filesystem::path file_path(input_path);

    if (file_path.extension() == ".jack")
    {
        file_path.replace_extension(".vm");
        return file_path.string();
    }

    return "";
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

        VMWriter writer = VMWriter(output);
        CompilationEngine engine = CompilationEngine(tokens, writer);
        engine.compileClass();
    }
}