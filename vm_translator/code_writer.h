#include <iostream>
#include <fstream>
#include <string>

class CodeWriter
{
private:
    std::ofstream &output_;
    std::string filename_;

public:
    CodeWriter(std::ofstream &output, std::string filename);
    void writeArithmetic(std::string command);
    std::string symbol(std::string segment, int index);
    void writePushPop(std::string command, std::string segment, int index);
    void close();
};
