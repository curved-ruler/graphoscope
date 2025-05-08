
#pragma once

#include <string>
#include <vector>

#include "base.hh"
#include "lexer.hh"

namespace cr {

class parser {

    public:
        std::string source;
        lexer lex;
        
        int status;
        
        std::vector<uint8> variables;
        
        ast_elem* AST;
        
    public:
        parser(const std::string& src);
        ~parser();
        
        int tokenize();
        
        void print_ast     (const ast_elem* e, const std::string& prefix, bool left);
        void print_ast_eval(const ast_elem* e, const std::string& prefix, bool left);
        
        int parse_expr(size_t begin, size_t end);
        int eval_expr (ast_elem*& e);
    
    private:
        void delete_ast(ast_elem*& ast);
        
        int expression     (ast_elem*& e, size_t begin, size_t end, int level);
        int expression_list(ast_elem*& e, size_t begin, size_t end, int level);
        
        int eval   (ast_elem*& e);
        int evalf  (ast_elem*& e);
        int evalop1(ast_elem*& e);
        int evalop2(ast_elem*& e);
        
        types::typ gettype2(const ast_elem* e);
        
        int   getival (const ast_elem* e);
        float getfval (const ast_elem* e);
        
        void pushfloat(float f, ast_elem* e);
        void pushint(int i, ast_elem* e);
};

}
