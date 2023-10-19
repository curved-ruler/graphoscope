
#include "szparse.hh"

#include "../gen/stringutils.hh"

namespace sz {

void szparse::add (const std::string& script)
{
    std::string s;
    for (size_t i=0 ; i<script.size() ; ++i)
    {
        char  c   = script[i];
        uint8 typ = token::UNKNOWN;
        
        if (cr::isspace(c))
        {
            if (s.size() == 0) continue;
            
            token t;
            t.type = typ;
            t.name = s;
            tokens.push_back(t);
            
            typ = token::UNKNOWN;
            s   = "";
        }
        //else if ()
        
        
        
        
    }
}

}
