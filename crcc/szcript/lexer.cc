
#include "lexer.hh"

#include <iostream>

namespace cr {

lexer::lexer () {}
lexer::~lexer () {}

bool lexer::isspace (char c)
{
    if (c == ' '  ||
        c == '\f' ||
        c == '\n' ||
        c == '\r' ||
        c == '\t' ||
        c == '\v')
    {
        return true;
    }
    
    return false;
}
bool lexer::starts_w (const std::string& s, const std::string& w)
{
    for (size_t i=0 ; i<w.size() ; ++i)
    {
        if (i >= s.size()) return false;
        if (s[i] != w[i])  return false;
    }
    return true;
}

void lexer::add (const std::string& scr)
{
    script += scr + "\n";
}
void lexer::clear ()
{
    script = "";
    tokens.clear();
}

int lexer::add_name (std::string& s, int l)
{
    int found = -1;
    
    for (size_t j=0 ; j<keys.table.size() ; ++j)
    {
        if (s == keys.table[j])
        {
            token t = token(token_t::KEYWORD, keys.table[j], l);
            t.id = j;
            tokens.push_back(t);
            found = j;
            s = s.substr(keys.table[j].size());
            break;
        }
    }
    
    if (found == -1) { tokens.push_back(token{token_t::IDENTIFIER, s, l}); }
    
    return 0;
}

int lexer::add_op (std::string& s, int l)
{
    int found = -1;
    
    while (s.size() > 0)
    {
        found = -1;
        
        //if (starts_w(s, "//")) return 1;
        
        for (size_t j=0 ; j<ops.table.size() ; ++j)
        {
            if (starts_w(s, ops.table[j].name))
            {
                token t = token(token_t::OP, ops.table[j].name, l);
                t.id = j;
                tokens.push_back(t);
                found = j;
                s = s.substr(ops.table[j].name.size());
                break;
            }
        }
        
        if (found == -1)
        {
            tokens.push_back(token{token_t::UNKNOWN, s, l});
            return -1;
        }
    }
    
    return 0;
}

int lexer::add_par (char c, int line)
{
    if (c=='(')
    {
        tokens.push_back(token{token_t::PARENTH, c, line});
        parenthesis_stack.push_back(ps_elem{tokens.size()-1, 0});
        return 0;
    }
    if (c==')')
    {
        tokens.push_back(token{token_t::PARENTH, c, line});
        size_t n = parenthesis_stack.size();
        
        if (n > 0 && parenthesis_stack[n-1].typ == 0)
        {
            tokens[parenthesis_stack[n-1].toki].id = tokens.size()-1;
            tokens[tokens.size()-1].id = parenthesis_stack[n-1].toki;
            parenthesis_stack.pop_back();
            return 0;
        }
        else
        {
            std::cout << "Unexpected ')' on line " << line << std::endl;
            return -1;
        }
    }
                
    if (c=='[')
    {
        tokens.push_back(token{token_t::PARENTH, c, line});
        parenthesis_stack.push_back(ps_elem{tokens.size()-1, 1});
        return 0;
    }
    if (c==']')
    {
        tokens.push_back(token{token_t::PARENTH, c, line});
        size_t n = parenthesis_stack.size();
        
        if (n > 0 && parenthesis_stack[n-1].typ == 1)
        {
            tokens[parenthesis_stack[n-1].toki].id = tokens.size()-1;
            tokens[tokens.size()-1].id = parenthesis_stack[n-1].toki;
            parenthesis_stack.pop_back();
            return 0;
        }
        else
        {
            std::cout << "Unexpected ']' on line " << line << std::endl;
            return -1;
        }
    }
                
    if (c=='{')
    {
        tokens.push_back(token{token_t::PARENTH, c, line});
        parenthesis_stack.push_back(ps_elem{tokens.size()-1, 2});
        return 0;
    }
    if (c=='}')
    {
        tokens.push_back(token{token_t::PARENTH, c, line});
        size_t n = parenthesis_stack.size();
        
        if (n > 0 && parenthesis_stack[n-1].typ == 2)
        {
            tokens[parenthesis_stack[n-1].toki].id = tokens.size()-1;
            tokens[tokens.size()-1].id = parenthesis_stack[n-1].toki;
            parenthesis_stack.pop_back();
            return 0;
        }
        else
        {
            std::cout << "Unexpected '}' on line " << line << std::endl;
            return -1;
        }
    }
    
    return 1;
}

void lexer::new_char (char c, char peek, token_t::tt& collecting, std::string& s, int line)
{
    if (isspace(c)) { return; }
    
    else if (c == '/' && (peek == '/')) { collecting = token_t::COMMENT; }
    
    else if (c == '"') { collecting = token_t::STRING; }
    
    else if (ops.opchars.find(c) != std::string::npos) { collecting = token_t::OP; s+=c; }
    
    else if (c == ';')         { tokens.push_back(token{token_t::SEMICOLON, ";", line}); }
    
    else if (c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}')
    {
        add_par(c, line);
    }
    
    else if (c <= '9' && c >= '0') { collecting = token_t::NUMBER; s+=c; }
    
    else {
        s+=c;
        collecting = token_t::IDENTIFIER;
    }
}

int lexer::process ()
{
    std::string s;
    token_t::tt collecting = token_t::UNKNOWN;
    int line = 1;
    
    for (size_t i=0 ; i<script.size() ; ++i)
    {
        char c    = script[i];
        char peek = i+1<script.size() ? script[i+1] : 0;
        
        
        switch (collecting)
        {
            case token_t::UNKNOWN:
                new_char(c, peek, collecting, s, line);
                break;
            
            case token_t::COMMENT:
                if (c == '\n') { collecting = token_t::UNKNOWN; s=""; }
                break;
                
            case token_t::STRING:
                if (c == '"') { tokens.push_back(token{token_t::STRING, s, line}); collecting = token_t::UNKNOWN; s=""; }
                else if (c == '\\')
                {
                    switch (peek)
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
                            s+=peek;
                            if (peek == '\n') line+=1;
                            break;
                    }
                    i+=1;
                }
                else { s+=c; }
                break;
                
            case token_t::OP:
                if (ops.opchars.find(c) == std::string::npos || (c=='/' && peek == '/'))
                {
                    add_op(s, line);
                    collecting = token_t::UNKNOWN;
                    s = "";
                    new_char(c, peek, collecting, s, line);
                }
                else
                {
                    s+=c;
                }
                break;
                
            case token_t::NUMBER:
                if ((c <= '9' && c >= '0') || c == '.' || c == '_')
                {
                    s+=c;
                }
                else
                {
                    tokens.push_back(token{token_t::NUMBER, s, line});
                    collecting = token_t::UNKNOWN;
                    s="";
                    new_char(c, peek, collecting, s, line);
                }
                break;
                
            case token_t::IDENTIFIER:
                if ( (ops.opchars.find(c) != std::string::npos) ||
                     (isspace(c)) ||
                     (c == '"') || ((c == ';')) ||
                     (c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}') )
                {
                    add_name(s, line);
                    collecting = token_t::UNKNOWN;
                    s="";
                    new_char(c, peek, collecting, s, line);
                }
                else
                {
                    s+=c;
                }
                break;
                
            default:
                std::cout << "Error: Unhandled collecting: " << tokentype_str(collecting) << std::endl;
                return -1;
        } // switch
        
        if (c == '\n') line += 1;
        
    } // for
    
    if (s.size() > 0)
    {
        tokens.push_back(token{collecting, s, line});
    }
    
    size_t n = parenthesis_stack.size();
    if (n > 0)
    {
        std::cout << "Error: Unclosed ";
        if (parenthesis_stack[n-1].typ == 0) std::cout << "(";
        if (parenthesis_stack[n-1].typ == 1) std::cout << "[";
        if (parenthesis_stack[n-1].typ == 2) std::cout << "{";
        std::cout << " on line " << tokens[parenthesis_stack[n-1].toki].line << std::endl;
        return -1;
    }
    
    return 0;
}

}
