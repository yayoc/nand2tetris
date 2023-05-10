#ifndef HEADER_SYMBOL_TABLE_H
#define HEADER_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

enum eKind
{
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
};
struct SymbolInfo
{
    std::string type;
    eKind kind;
    int index;
};
class SymbolTable
{
public:
    SymbolTable();
    void reset();
    void define(std::string name, std::string type, eKind kind);
    int varCount(eKind kind);
    eKind kindOf(std::string name);
    std::string typeOf(std::string name);
    int indexOf(std::string name);
    static eKind fromString(std::string str);
    bool has(std::string name);

private:
    std::unordered_map<std::string, SymbolInfo> table_;
    std::unordered_map<eKind, int> counters_;
};

#endif