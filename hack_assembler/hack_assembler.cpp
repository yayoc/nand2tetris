#include <string>
#include <iostream>
#include <vector>

#include "hack_assembler.h"
#include "parser.h"
#include "code.h"
#include "symbol_table.h"

std::vector<std::string> HackAssembler::assemble(std::string input)
{
    std::vector<std::string> output;

    Parser parser(input);
    SymbolTable table;

    // create symbol table
    int i = 0;
    while (parser.hasMoreLines()) 
    {
        if (parser.instructionType() == A_INSTRUCTION) {
            i++;
        }
        else if (parser.instructionType() == C_INSTRUCTION) {
            i++;
        }
        else if (parser.instructionType() == L_INSTRUCTION) {
            std::string symbol = parser.symbol();
            //std::cout << "l instruction: " << symbol << std::endl;
            //std::cout << "i: " << i << std::endl;
            table.addEntry(symbol, i);
        }
        
        parser.advance();
    }

    parser.reset();

    // second path
    int v = 16;
    while (parser.hasMoreLines())
    {
        std::string a;
        std::string c;
        switch (parser.instructionType())
        {
        case A_INSTRUCTION:
            if (Parser::isNumber(parser.symbol())) {
                output.push_back(parser.symbol());
            } else {
                // second path should already know symbol's address
                if (table.contains(parser.symbol())) {
                    int addr = table.getAddress(parser.symbol());
                    //std::cout << "symbol is: " << parser.symbol() << std::endl;
                    //std::cout << "addr is: " << addr << std::endl;
                    // need to convert to binary
                    output.push_back(Parser::convertToBinary(std::to_string(addr)));
                } else {
                    table.addEntry(parser.symbol(), v);
                    output.push_back(Parser::convertToBinary(std::to_string(v)));
                    v++;
                }

            }
            break;
        case C_INSTRUCTION:
            output.push_back("111" + Code::comp(parser.comp()) + Code::dest(parser.dest()) + Code::jump(parser.jump()));
            break;
        case L_INSTRUCTION:
            break;
        }
        parser.advance();
    }

    return output;
}
