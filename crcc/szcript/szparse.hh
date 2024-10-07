
#pragma once

#include <string>
#include <vector>

#include "sztables.hh"

namespace sz {

class szparse {

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
        szparse();
        ~szparse();
        
        void add(const std::string& scr);
        void clear();
        
        int lexer();
        
    private:
        int add_name (std::string& s, int line);
        int add_op   (std::string& s, int line);
        int add_par  (char c, int line);

};

}
