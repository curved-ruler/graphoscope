
#pragma once

#include <string>
#include <vector>

#include "../base.hh"

namespace sz {

namespace token_t {
enum tt
{
    UNKNOWN,
    COMMENT,
    SEMICOLON,
    PARENTH, SQBRACKET, BLOCK,
    KEYWORD, OP, STRING, NUMBER, IDENTIFIER
};
}
std::string tokentype_str (token_t::tt tt);

struct token
{
    token(token_t::tt t, std::string n);
    token(token_t::tt t, char c);
    
    
    token_t::tt type;
    std::string name;
    int id;
};

struct keywords
{
    std::vector<std::string> table;
    
    keywords();
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
