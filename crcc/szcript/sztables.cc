
#include "sztables.hh"

#include <algorithm>

namespace sz {


keywords::keywords()
{
    table.push_back("return");
    table.push_back("if");
    table.push_back("for");
    table.push_back("do");
    table.push_back("while");
    table.push_back("true");
    table.push_back("false");
    table.push_back("null");
    table.push_back("nan");
    
    std::sort(table.begin(), table.end(), [](const std::string& a, const std::string& b)
    {
        if (a.size() == b.size())
        {
            for (size_t i=0 ; i<a.size() ; ++i)
            {
                if (a[i] == b[i]) continue;
                return a[i] > b[i];
            }
            return true;
        }
        else
        {
            return a.size() > b.size();
        }
    });
}

operators::operators()
{
    table.push_back( op{ .prec{2},  .arity{2},  .assoc{true},   .name{"."} } );
    table.push_back( op{ .prec{3},  .arity{1},  .assoc{false},  .name{"!"} } );
    table.push_back( op{ .prec{3},  .arity{1},  .assoc{false},  .name{"~"} } );
    table.push_back( op{ .prec{3},  .arity{1},  .assoc{false},  .name{"-"} } );
    table.push_back( op{ .prec{5},  .arity{2},  .assoc{true},   .name{"*"} } );
    table.push_back( op{ .prec{5},  .arity{2},  .assoc{true},   .name{"/"} } );
    table.push_back( op{ .prec{5},  .arity{2},  .assoc{true},   .name{"&"} } );
    table.push_back( op{ .prec{5},  .arity{2},  .assoc{true},   .name{"%"} } );
    table.push_back( op{ .prec{6},  .arity{2},  .assoc{true},   .name{"+"} } );
    table.push_back( op{ .prec{6},  .arity{2},  .assoc{true},   .name{"-"} } );
    table.push_back( op{ .prec{6},  .arity{2},  .assoc{true},   .name{"^"} } );
    table.push_back( op{ .prec{9},  .arity{2},  .assoc{true},   .name{"<"} } );
    table.push_back( op{ .prec{9},  .arity{2},  .assoc{true},   .name{">"} } );
    table.push_back( op{ .prec{9},  .arity{2},  .assoc{true},   .name{"<="} } );
    table.push_back( op{ .prec{9},  .arity{2},  .assoc{true},   .name{">="} } );
    table.push_back( op{ .prec{10}, .arity{2},  .assoc{true},   .name{"=="} } );
    table.push_back( op{ .prec{10}, .arity{2},  .assoc{true},   .name{"!="} } );
    table.push_back( op{ .prec{14}, .arity{2},  .assoc{true},   .name{"&&"} } );
    table.push_back( op{ .prec{15}, .arity{2},  .assoc{true},   .name{"||"} } );
    table.push_back( op{ .prec{16}, .arity{2},  .assoc{false},  .name{"="} } );
    table.push_back( op{ .prec{17}, .arity{2},  .assoc{false},  .name{","} } );
    
    std::sort(table.begin(), table.end(), [](const op& a, const op& b)
    {
        if (a.name.size() == b.name.size())
        {
            for (size_t i=0 ; i<a.name.size() ; ++i)
            {
                if (a.name[i] == b.name[i]) continue;
                return a.name[i] > b.name[i];
            }
            return a.arity > b.arity;
        }
        else
        {
            return a.name.size() > b.name.size();
        }
    });
    
    for (size_t i=0 ; i<table.size() ; ++i) opchars += table[i].name;
}

token::token(token_t::tt t, std::string n, int l) : type(t), name(n), id(-1), line(l) {}
token::token(token_t::tt t, char c, int l) : type(t), line(l) { std::string n=""; n+=c; name = n; id=-1; }

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
                
            case sz::token_t::IDENTIFIER:
                return "identif";
                
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
