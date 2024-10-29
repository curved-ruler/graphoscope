
#pragma once

#include <string>
#include <vector>

#include "szlexer.hh"

namespace sz {

struct ast_elem {
    int rule;
    std::string rule_str;
    
    std::vector<ast_elem*> children;
};

class szparser {

    public:
        std::string source;
        szlexer lex;
        
        int status;
        
        ast_elem* AST;
        
    public:
        szparser(const std::string& src);
        ~szparser();
        
        int lexer();
        
        int expression(size_t begin, size_t end, int level);
    
    private:
        void delete_ast(ast_elem*& ast);
};

}
