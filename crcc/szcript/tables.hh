
#pragma once

#include <string>
#include <vector>

#include "base.hh"

namespace cr {

namespace token_t {
enum tt
{
    UNKNOWN,
    COMMENT,
    SEMICOLON,
    PARENTH, SQBRACKET, BLOCK,
    KEYWORD, OP, STRING, NUMBER, IDENTIFIER,
    
    FUNCTION
};
}
namespace types {
enum typ
{
    INT, FLOAT
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
    char prec;
    char arity;
    bool  assoc; // true: +-,  false: =
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
    token_t::tt toktyp;
    std::string name;
    
    types::typ type;
    std::vector<uint8> value;
    
    std::vector<ast_elem*> children;
};

}
