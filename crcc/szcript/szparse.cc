
#include "szparse.hh"

#include "../gen/stringutils.hh"

namespace sz {

szparse::szparse () {}
szparse::~szparse () {}

void szparse::add (const std::string& scr)
{
    script += "\n" + scr;
}
void szparse::clear ()
{
    script = "";
    tokens.clear();
}

int szparse::lexer ()
{
    std::string s;
    int ret = 0;
    int collecting = -1; // 0-alphanumeric, 1-op
    
    for (size_t i=0 ; i<script.size() ; ++i)
    {
        char c = script[i];
        
        if (cr::isspace(c) || c==';' || c==',')
        {
            if (s.size() > 0) { ret = addtoken(s); }
            s = "";
            collecting = -1;
        }
        else if (c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}')
        {
            if (s.size() > 0) { ret = addtoken(s); }
            std::string s2 = "";
            s2 += c;
            addtoken(s2);
            s = "";
            collecting = -1;
        }
        else if (ops.opchars.find(c) != std::string::npos)
        {
            if (collecting == 1 || collecting == -1)
            {
                s += c;
                collecting = 1;
            }
            else
            {
                ret = addtoken(s);
                s = "";
                collecting = -1;
            }
        }
        else
        {
            s += c;
            collecting = 0;
        }
    }
    
    return ret;
}

int szparse::addtoken (std::string& s)
{
    if (s.size() == 0) return -1;
    
    if (s == "(" || s == ")")
    {
        token t;
        t.type = token_t::PARENTH;
        t.name = s;
        tokens.push_back(t);
        return 0;
    }
    if (s == "[" || s == "]")
    {
        token t;
        t.type = token_t::SQBRACKET;
        t.name = s;
        tokens.push_back(t);
        return 0;
    }
    if (s == "{" || s == "}")
    {
        token t;
        t.type = token_t::BLOCK;
        t.name = s;
        tokens.push_back(t);
        return 0;
    }
    
    for (size_t i=0 ; i<ops.table.size() ; ++i)
    {
        if (ops.table[i].name == s)
        {
            token t;
            t.type = token_t::OP;
            t.name = s;
            tokens.push_back(t);
            return 0;
        }
    }
    
    for (size_t i=0 ; i<keys.table.size() ; ++i)
    {
        if (keys.table[i] == s)
        {
            token t;
            t.type = token_t::KEYWORD;
            t.name = s;
            tokens.push_back(t);
            return 0;
        }
    }
    
    if (s[0] == '"' && s[s.size()-1] == '"')
    {
        token t;
        t.type = token_t::STRING;
        t.name = s.substr(1,s.size()-2);
        tokens.push_back(t);
        return 0;
    }
    else if (s[0] >= '0' && s[0] <= '9')
    {
        token t;
        t.type = token_t::CONST;
        t.name = s;
        tokens.push_back(t);
        return 0;
    }
    else
    {
        token t;
        t.type = token_t::VARIABLE;
        t.name = s;
        tokens.push_back(t);
        return 0;
    }
}

}
