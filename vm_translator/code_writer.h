#include <iostream>
#include <fstream>
#include <string>

class CodeWriter
{
private:
    static std::string ADD;
    static std::string SUB;
    static std::string AND;
    static std::string OR;
    static std::string NEG;
    static std::string NOT;
    static std::string PUSH;
    static std::string POP;

    std::ofstream &output_;
    std::string filename_;
    int i_;

    void writeXMinusYToD();
    void writeTrueI();
    void writeFalseI();
    std::string symbol(std::string segment, int index);
    void translatePush(std::string command, std::string segment, int index);
    void translatePop(std::string command, std::string segment, int index);

public:
    CodeWriter(std::ofstream &output, std::string filename);
    void writeArithmetic(std::string command);
    void writePushPop(std::string command, std::string segment, int index);
    void close();
};
