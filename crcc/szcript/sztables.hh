
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
    token(token_t::tt t, std::string n, int l);
    token(token_t::tt t, char c, int l);
    
    
    token_t::tt type;
    std::string name;
    int id;
    int line;
};

struct keywords
{
    std::vector<std::string> table;
    
    keywords();
};

struct op
{
    uint8 prec;
    uint8 arity;
    std::string name;
};

struct operators
{
    std::string opchars;
    std::vector<op> table;
    
    operators();
};

struct ast_elem
{
    int rule;
    std::string name;
    
    int    type;
    size_t value;
    
    std::vector<ast_elem*> children;
};

}
