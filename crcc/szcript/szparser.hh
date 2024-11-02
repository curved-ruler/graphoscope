
#pragma once

#include <string>
#include <vector>

#include "szlexer.hh"

namespace sz {

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
        
        void print_ast(ast_elem* e, std::string prefix, bool left);
        
        int expression     (ast_elem*& e, size_t begin, size_t end, int level);
        int expression_list(ast_elem*& e, size_t begin, size_t end, int level);
    
    private:
        void delete_ast(ast_elem*& ast);
};

}
