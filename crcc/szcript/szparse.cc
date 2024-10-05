
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

int szparse::add_name (std::string& s)
{
    int found = -1;
    
    for (size_t j=0 ; j<keys.table.size() ; ++j)
    {
        if (s == keys.table[j])
        {
            token t = token(token_t::KEYWORD, keys.table[j]);
            t.id = j;
            tokens.push_back(t);
            found = j;
            s = s.substr(keys.table[j].size());
            break;
        }
    }
        
    if (found == -1) { tokens.push_back(token{token_t::IDENTIFIER, s}); }
        
    return 0;
}

int szparse::add_op (std::string& s)
{
    int found = -1;
    
    while (s.size() > 0)
    {
        found = -1;
        for (size_t j=0 ; j<ops.table.size() ; ++j)
        {
            if (cr::starts_w(s, ops.table[j].name))
            {
                token t = token(token_t::OP, ops.table[j].name);
                t.id = j;
                tokens.push_back(t);
                found = j;
                s = s.substr(ops.table[j].name.size());
                break;
            }
        }
        
        if (found == -1)
        {
            tokens.push_back(token{token_t::UNKNOWN, s});
            return -1;
        }
    }
    
    return 0;
}

int szparse::lexer ()
{
    std::string s;
    token_t::tt collecting = token_t::UNKNOWN;
    
    for (size_t i=0 ; i<script.size() ; ++i)
    {
        char c = script[i];
        
        if (collecting == token_t::UNKNOWN)
        {
            while (cr::isspace(c) && i<script.size()-1) { i+=1; c = script[i]; }
            if    (cr::isspace(c)) { return 0; }
            
            if (c == '/' && (i<script.size()-1 && script[i+1] == '/')) { collecting = token_t::COMMENT; continue; }
            
            if (c == '"') { collecting = token_t::STRING; continue; }
            
            if (ops.opchars.find(c) != std::string::npos) { collecting = token_t::OP; s+=c; continue; }
            
            if (c == ';')         { tokens.push_back(token{token_t::SEMICOLON, ";"}); continue; }
            if (c=='(' || c==')') { tokens.push_back(token{token_t::PARENTH,   c});   continue; }
            if (c=='[' || c==']') { tokens.push_back(token{token_t::SQBRACKET, c});   continue; }
            if (c=='{' || c=='}') { tokens.push_back(token{token_t::BLOCK,     c});   continue; }
            
            if (c <= '9' && c >= '0') { collecting = token_t::NUMBER; s+=c; continue; }
            
            s+=c;
            collecting = token_t::IDENTIFIER;
            continue;
        }
        
        switch (collecting)
        {
            case token_t::COMMENT:
                if (c == '\n') { collecting = token_t::UNKNOWN; s=""; }
                else { s+=c; }
                break;
                
            case token_t::STRING:
                if (c == '"') { tokens.push_back(token{token_t::STRING, s}); collecting = token_t::UNKNOWN; s=""; }
                else if (c == '\\' && i<script.size()-1)
                {
                    switch (script[i+1])
                    {
                        case 'n':
                            s += '\n';
                            break;
                            
                        case 'r':
                            s += '\r';
                            break;
                            
                        case 't':
                            s += '\t';
                            break;
                            
                        case '\\':
                            s += '\\';
                            break;
                            
                        case '"':
                            s += '"';
                            break;
                            
                        default:
                            s+=script[i+1];
                            break;
                    }
                    i+=1;
                }
                else { s+=c; }
                break;
                
            case token_t::OP:
                if (ops.opchars.find(c) == std::string::npos)
                {
                    add_op(s);
                    collecting = token_t::UNKNOWN;
                    s="";
                    i-=1;
                }
                else
                {
                    s+=c;
                }
                break;
                
            case token_t::NUMBER:
                if ((c <= '9' && c >= '0') || c == '.' || c == '_') { s+=c; }
                else
                {
                    tokens.push_back(token{token_t::NUMBER, s});
                    collecting = token_t::UNKNOWN;
                    s="";
                    i-=1;
                }
                break;
                
            case token_t::IDENTIFIER:
                if ( (ops.opchars.find(c) != std::string::npos) ||
                     (cr::isspace(c)) ||
                     (c == '"') || ((c == ';')) ||
                     (c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}') )
                {
                    add_name(s);
                    collecting = token_t::UNKNOWN;
                    s="";
                    i-=1;
                }
                else
                {
                    s+=c;
                }
                break;
                
            default:
                std::cout << "Error: Unhandled collecting: " << collecting << std::endl;
                return -1;
        }
    }
    
    if (s.size() > 0)
    {
        tokens.push_back(token{collecting, s});
    }
    
    return 0;
}

}
