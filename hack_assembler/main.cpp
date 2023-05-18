#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "hack_assembler.h"

int main(int argc, char* argv[])
{
    if (argc == 1) {
        std::cout << "please pass asm path arg" << std::endl;
        return 0;
    }
    // read file content
    // https://www.tutorialspoint.com/Read-whole-ASCII-file-into-Cplusplus-std-string
    std::ifstream f(argv[1]);
    std::string str;
    if (f) {
      std::ostringstream ss;
      ss << f.rdbuf();
      str = ss.str();
    }


    std::vector<std::string> output = HackAssembler::assemble(str);

    for (std::string str : output)
    {
        std::cout << str << std::endl;
    }

    return 0;
}
