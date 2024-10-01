
#include "sztables.hh"

namespace sz {

keywords::keywords()
{
    table.push_back("return");
    table.push_back("if");
    table.push_back("for");
    table.push_back("do");
    table.push_back("while");
}

operators::operators()
{
    opchars = "+-*/%:=<>!~^|&.,";
    
    table.push_back( op{2,  "++"} );
    table.push_back( op{2,  "--"} );
    table.push_back( op{5,  "*"} );
    table.push_back( op{5,  "/"} );
    table.push_back( op{5,  "%"} );
    table.push_back( op{6,  "+"} );
    table.push_back( op{6,  "-"} );
    table.push_back( op{9,  "<"} );
    table.push_back( op{9,  ">"} );
    table.push_back( op{9,  "<="} );
    table.push_back( op{9,  ">="} );
    table.push_back( op{10, "=="} );
    table.push_back( op{10, "!="} );
    table.push_back( op{14, "&&"} );
    table.push_back( op{15, "||"} );
    table.push_back( op{16, "="} );
    table.push_back( op{16, ":"} );
    table.push_back( op{17, ","} );
}

}
