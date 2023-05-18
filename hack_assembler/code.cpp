#include <string>

#include "code.h"

std::string Code::dest(std::string str)
{
    if (str.empty()) return "000";
    if (str == "M") return "001";
    if (str == "D") return "010";
    if (str == "DM") return "011";
    if (str == "MD") return "011";
    if (str == "A") return "100";
    if (str == "AM") return "101";
    if (str == "AD") return "110";
    if (str == "ADM") return "111";

    return "";
}

std::string Code::comp(std::string str)
{
    if (str == "0") return "0101010";
    if (str == "1") return "0111111";
    if (str == "-1") return "0111010";
    if (str == "D") return "0001100";

    if (str == "A") return "0110000";
    if (str == "M") return "1110000";

    if (str == "!D") return "0001101";

    if (str == "!A") return "0110001";
    if (str == "!M") return "1110001";

    if (str == "-D") return "0001111";

    if (str == "-A") return "0110011";
    if (str == "-M") return "1110011";

    if (str == "D+1") return "0011111";

    if (str == "A+1") return "0110111";
    if (str == "M+1") return "1110111";

    if (str == "D-1") return "0001110";

    if (str == "A-1") return "0110010";
    if (str == "M-1") return "1110010";
    
    if (str == "D+A") return "0000010";
    if (str == "D+M") return "1000010";

    if (str == "D-A") return "0010011";
    if (str == "D-M") return "1010011";

    if (str == "A-D") return "0000111";
    if (str == "M-D") return "1000111";

    if (str == "D&A") return "0000000";
    if (str == "D&M") return "1000000";

    if (str == "D|A") return "0010101";
    if (str == "D|M") return "1010101";

    return "";
}

std::string Code::jump(std::string str)
{
    if (str.empty()) return "000";

    if (str == "JGT") return "001";
    if (str == "JEQ") return "010";
    if (str == "JGE") return "011";
    if (str == "JLT") return "100";
    if (str == "JNE") return "101";
    if (str == "JLE") return "110";
    if (str == "JMP") return "111";

    return "000";
}
