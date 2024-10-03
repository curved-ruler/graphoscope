
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

token::token(token_t::tt t, std::string n) : type(t), name(n) {}
token::token(token_t::tt t, char c) : type(t) { std::string n; n+=c; name = n; }

std::string tokentype_str (token_t::tt tt)
{
    switch (tt)
        {
            case sz::token_t::COMMENT:
                return "comment";
                
            case sz::token_t::STRING:
                return "string";
                
            case sz::token_t::OP:
                return "op";
                
            case sz::token_t::NUMBER:
                return "number";
                
            case sz::token_t::VARIABLE:
                return "variable";
                
            case sz::token_t::UNKNOWN:
                return "unknown";
                
            case sz::token_t::SEMICOLON:
                return "semicol";
                
            case sz::token_t::PARENTH:
                return "parenth";
                
            case sz::token_t::SQBRACKET:
                return "sqbracket";
                
            case sz::token_t::BLOCK:
                return "block";
                
            case sz::token_t::KEYWORD:
                return "keyword";
        }
        return "?";
}

}
