#ifndef HEADER_TOKEN_H
#define HEADER_TOKEN_H

#include "jack_tokenizer.h"

struct Token
{
    std::string value;
    eTokenType type;
};

#endif // HEADER_TOKEN_H