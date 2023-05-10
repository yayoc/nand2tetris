#include "vm_writer.h"

std::string eSegementToString(eSegment segment)
{
    switch (segment)
    {
    case eSegment::ARGUMENT:
        return "argument";
    case eSegment::CONSTANT:
        return "constant";
    case eSegment::LOCAL:
        return "local";
    case eSegment::kSTATIC:
        return "static";
    case eSegment::THIS:
        return "this";
    case eSegment::THAT:
        return "that";
    case eSegment::POINTER:
        return "pointer";
    case eSegment::TEMP:
        return "temp";
    default:
        throw std::invalid_argument("invalid segment argument");
    }
};

std::string eCommandToString(eCommand command)
{
    switch (command)
    {
    case eCommand::ADD:
        return "add";
    case eCommand::AND:
        return "and";
    case eCommand::EQ:
        return "eq";
    case eCommand::GT:
        return "gt";
    case eCommand::LT:
        return "lt";
    case eCommand::NEG:
        return "neg";
    case eCommand::NOT:
        return "not";
    case eCommand::OR:
        return "or";
    case eCommand::SUB:
        return "sub";
    default:
        throw std::invalid_argument("invalid command argument");
    }
}

void VMWriter::writePush(eSegment segment, int index)
{
    output_ << "push " + eSegementToString(segment) + " " + std::to_string(index) << std::endl;
}

void VMWriter::writePop(eSegment segment, int index)
{
    output_ << "pop " + eSegementToString(segment) + " " + std::to_string(index) << std::endl;
}

void VMWriter::writeArithmetic(eCommand command)
{
    output_ << eCommandToString(command) << std::endl;
}

void VMWriter::writeLabel(std::string label)
{
    output_ << "label " << label << std::endl;
}

void VMWriter::writeGoto(std::string label)
{
    output_ << "goto " << label << std::endl;
}

void VMWriter::writeIf(std::string label)
{
    output_ << "if-goto " << label << std::endl;
}

void VMWriter::writeCall(std::string name, int nArgs)
{
    output_ << "call " << name << " " << std::to_string(nArgs) << std::endl;
}

void VMWriter::writeFunction(std::string name, int nArgs)
{
    output_ << "function " << name << " " << std::to_string(nArgs) << std::endl;
}

void VMWriter::writeReturn()
{
    output_ << "return" << std::endl;
}

void VMWriter::close()
{
    output_.close();
}

eSegment VMWriter::kindToSegment(eKind kind)
{
    switch (kind)
    {
    case eKind::ARG:
        return eSegment::ARGUMENT;
    case eKind::FIELD:
        return eSegment::THIS;
    case eKind::STATIC:
        return eSegment::kSTATIC;
    case eKind::VAR:
        return eSegment::LOCAL;
    }
    return eSegment();
}
