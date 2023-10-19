
#pragma once

#include <string>
#include <vector>

#include "sztables.hh"

namespace sz {

class szparse {

    public:
        std::vector<token> tokens;
        
    public:
        szparse();
        ~szparse();
        
        void add(const std::string& script);

};

}
