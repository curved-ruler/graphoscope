
#include "szparser.hh"

#include "../gen/stringutils.hh"

namespace sz {

szparser::szparser (const std::string& src)
{
    source = src;
    AST = new ast_elem;
}
szparser::~szparser ()
{
    delete_ast(AST);
}
void szparser::delete_ast(ast_elem*& aste)
{
    for (size_t i=0 ; i<aste->children.size() ; ++i) delete_ast(aste->children[i]);
    delete aste;
}

int szparser::lexer()
{
    lex.add(source);
    status = lex.lexer();
    return status;
}

int szparser::expression(size_t begin, size_t end, int level)
{
    if (end == begin+1)
    {
        std::cout << lex.tokens[begin].name << "   ";
        return 1;
    }
    if (end < begin+1)
    {
        return 0;
    }
    
    //std::cout << "L: " << level << ": ";
    //for (size_t i=begin ; i<end ; ++i) std::cout << lex.tokens[i].name << "   ";
    //std::cout << std::endl;
    
    int rootop     = -1;
    uint8 rootprec = 0;
    size_t i = begin;
    while (i<end)
    {
        if (lex.tokens[i].type == token_t::PARENTH)
        {
            //expression(i+1,lex.tokens[i].id, level+1);
            i = lex.tokens[i].id + 1;
        }
        else if (lex.tokens[i].type == token_t::OP)
        {
            if (lex.ops.table[lex.tokens[i].id].prec > rootprec)
            {
                rootop = i;
                rootprec = lex.ops.table[lex.tokens[i].id].prec;
            }
            i+=1;
        }
        else
        {
            i+=1;
        }
    }
    
    if (rootop != -1)
    {
        std::cout << lex.ops.table[lex.tokens[rootop].id].name << "   ";
        expression(begin,rootop,level);
        expression(rootop+1,end,level);
        return 2;
    }
    else if (lex.tokens[begin].type == token_t::PARENTH && lex.tokens[end-1].type == token_t::PARENTH)
    {
        expression(begin+1,end-1,level+1);
        return 3;
    }
        
    
    return -1;
}

}
