#ifndef HEADER_VM_WRITER_H
#define HEADER_VM_WRITER_H

#include <fstream>

enum eSegment
{
    CONSTANT,
    ARGUMENT,
    LOCAL,
    kSTATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

enum eCommand
{
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
};

class VMWriter
{
public:
    VMWriter(std::ofstream &output) : output_(output){};
    void writePush(eSegment segment, int index);
    void writePop(eSegment segment, int index);
    void writeArithmetic(eCommand command);
    void writeLabel(std::string label);
    void writeGoto(std::string label);
    void writeIf(std::string label);
    void writeCall(std::string name, int nArgs);
    void writeFunction(std::string name, int nArgs);
    void writeReturn();
    void close();

private:
    std::ofstream &output_;
};

#endif