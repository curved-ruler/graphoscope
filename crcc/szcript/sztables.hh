
#pragma once

#include <string>

#include "../base.hh"

namespace sz {

struct op
{
    uint8 prec;
    char  name[3];
};

struct token
{
    static const uint8 UNKNOWN = 0;
    static const uint8 KEYWORD = 1;
    static const uint8 PARENTH = 2;
    static const uint8 BLOCK   = 3;
    static const uint8 OP      = 4;
    static const uint8 STRING  = 5;
    static const uint8 NUMBER  = 6;
    static const uint8 VARIAB  = 7;
    
    uint8       type;
    std::string name;
};

}
