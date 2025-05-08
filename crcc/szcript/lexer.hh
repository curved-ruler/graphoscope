
#pragma once

#include <string>
#include <vector>

#include "tables.hh"

namespace cr {

class lexer {

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
        lexer();
        ~lexer();
        
        void add(const std::string& scr);
        void clear();
        
        int process();
        
        bool isspace (char c);
        bool starts_w (const std::string& s, const std::string& w);
        
    private:
        int  add_name (std::string& s, int line);
        int  add_op   (std::string& s, int line);
        int  add_par  (char c, int line);
        void new_char (char c, char peek, token_t::tt& collecting, std::string& s, int line);

};

}
