
#pragma once

#include <string>
#include <vector>

#include "../base.hh"

namespace sz {

namespace token_t {
enum tt
{
    UNKNOWN,
    PARENTH, SQBRACKET, BLOCK,
    KEYWORD, OP, STRING, CONST, VARIABLE
};
}

struct token
{
    token_t::tt type;
    std::string name;
};

struct keywords
{
    std::vector<std::string> table;
    
    keywords();
    bool is_keyword() const;
};

struct op
{
    uint8 prec;
    std::string name;
};

struct operators
{
    std::string opchars;
    std::vector<op> table;
    
    operators();
};

}
