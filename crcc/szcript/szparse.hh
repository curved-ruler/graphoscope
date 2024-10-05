
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
        
    public:
        szparse();
        ~szparse();
        
        void add(const std::string& scr);
        void clear();
        
        int lexer();
        
    private:
        int add_name (std::string& s);
        int add_op   (std::string& s);

};

}
