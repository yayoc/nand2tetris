#include "symbol_table.h"

SymbolTable::SymbolTable()
{
    table_ = {};
    counters_ = {};
}

void SymbolTable::reset()
{
    table_.clear();
    counters_.clear();
}

void SymbolTable::define(std::string name, std::string type, eKind kind)
{
    int index = varCount(kind);
    SymbolInfo s;
    s.type = type;
    s.kind = kind;
    s.index = index;
    table_[name] = s;
    counters_[kind]++;
}

int SymbolTable::varCount(eKind kind)
{
    return counters_[kind];
}

eKind SymbolTable::kindOf(std::string name)
{
    return table_[name].kind;
}

std::string SymbolTable::typeOf(std::string name)
{
    return table_[name].type;
}

int SymbolTable::indexOf(std::string name)
{
    return table_[name].index;
}

eKind SymbolTable::fromString(std::string str)
{
    if (str == "static")
    {
        return eKind::STATIC;
    }

    if (str == "field")
    {
        return eKind::FIELD;
    }

    return eKind();
}

bool SymbolTable::has(std::string name)
{
    return table_.find(name) != table_.end();
}
