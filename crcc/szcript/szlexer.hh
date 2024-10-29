
#pragma once

#include <string>
#include <vector>

#include "sztables.hh"

namespace sz {

class szlexer {

    public:
        std::string script;
        
        operators ops;
        keywords keys;
        std::vector<token> tokens;
        
        struct ps_elem
        {
            size_t toki; int typ;
            ps_elem(size_t ti, int ty) : toki(ti), typ(ty) {}
        };
        std::vector<ps_elem> parenthesis_stack;
        
    public:
        szlexer();
        ~szlexer();
        
        void add(const std::string& scr);
        void clear();
        
        int lexer();
        
    private:
        int  add_name (std::string& s, int line);
        int  add_op   (std::string& s, int line);
        int  add_par  (char c, int line);
        void new_char (char c, char peek, token_t::tt& collecting, std::string& s, int line);

};

}
