
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
        
        std::vector<float> floats;
        std::vector<int>   ints;
        
        ast_elem* AST;
        
    public:
        szparser(const std::string& src);
        ~szparser();
        
        int lexer();
        
        void print_ast(const ast_elem* e, const std::string& prefix, bool left);
        
        int parse_expr(size_t begin, size_t end);
    
    private:
        void delete_ast(ast_elem*& ast);
        
        int expression     (ast_elem*& e, size_t begin, size_t end, int level);
        int expression_list(ast_elem*& e, size_t begin, size_t end, int level);
        
        int eval(ast_elem*& e);
        
        int   getival (const ast_elem* e);
        float getfval (const ast_elem* e);
};

}
