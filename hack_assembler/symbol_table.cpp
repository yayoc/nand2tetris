#include <map>
#include <iostream>
#include <string>

#include "symbol_table.h"

SymbolTable::SymbolTable() {
    // R0 - R15
    for (int i = 0; i < 16; i++) {
        m.insert(std::make_pair("R" + std::to_string(i), i));
    }

    m.insert(std::make_pair("SP", 0));
    m.insert(std::make_pair("LCL", 1));
    m.insert(std::make_pair("ARG", 2));
    m.insert(std::make_pair("THIS", 3));
    m.insert(std::make_pair("THAT", 4));

    m.insert(std::make_pair("SCREEN", 16384));
    m.insert(std::make_pair("KBD", 24576));
}

void SymbolTable::addEntry(std::string symbol, int address) {
    m.insert(std::make_pair(symbol, address)); 
}

bool SymbolTable::contains(std::string symbol) {
    return m.count(symbol) > 0;
}

int SymbolTable::getAddress(std::string symbol) {
    return m.at(symbol);
}
