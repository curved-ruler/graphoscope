
#include "szparser.hh"

#include "../gen/stringutils.hh"

namespace sz {

szparser::szparser (const std::string& src)
{
    source = src;
    AST = new ast_elem;
    AST->name = "program";
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
void szparser::print_ast(ast_elem* e, std::string prefix, bool left)
{
    std::cout << prefix + (left ? "├─ " : "└─ " ) << e->name << "\n";
    
    for (size_t i=0 ; i<e->children.size() ; ++i)
    {
        print_ast(e->children[i], prefix + (left ? std::string("|  ") : std::string("   ")), i != e->children.size()-1 );
    }
}

int szparser::lexer()
{
    lex.add(source);
    status = lex.lexer();
    return status;
}

int szparser::expression(ast_elem*& aste, size_t begin, size_t end, int level)
{
    if (end == begin+1)
    {
        aste->name = lex.tokens[begin].name;
        //std::cout << lex.tokens[begin].name << "   ";
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
        //std::cout << lex.ops.table[lex.tokens[rootop].id].name << "   ";
        
        aste->name = lex.ops.table[lex.tokens[rootop].id].name;
        ast_elem* left  = new ast_elem;
        ast_elem* right = new ast_elem;
        aste->children.push_back(left);
        aste->children.push_back(right);
        expression(left, begin,rootop,level+1);
        expression(right, rootop+1,end,level+1);
        return 2;
    }
    else if (lex.tokens[begin].type == token_t::PARENTH && lex.tokens[end-1].type == token_t::PARENTH)
    {
        expression(aste, begin+1,end-1,level);
        return 3;
    }
    else if (begin+1 < end && lex.tokens[begin].type == token_t::IDENTIFIER && lex.tokens[begin+1].type == token_t::PARENTH && lex.tokens[end-1].type == token_t::PARENTH)
    {
        aste->name = lex.tokens[begin].name;
        
        //std::cout << lex.tokens[begin].name << " ( ";
        expression_list(aste, begin+2,end-1,level+1);
        //std::cout << " )   ";
        return 3;
    }
        
    
    return -1;
}

int szparser::expression_list(ast_elem*& aste, size_t begin, size_t end, int level)
{
    if (end < begin+1)
    {
        return 0;
    }
    
    //std::cout << "L: " << level << ": ";
    //for (size_t i=begin ; i<end ; ++i) std::cout << lex.tokens[i].name << "   ";
    //std::cout << std::endl;
    
    size_t i   = begin;
    size_t beg = begin;
    while (i<end)
    {
        if (lex.tokens[i].type == token_t::PARENTH)
        {
            //expression(i+1,lex.tokens[i].id, level+1);
            i = lex.tokens[i].id + 1;
        }
        else if (lex.tokens[i].type == token_t::OP && lex.tokens[i].name == ",")
        {
            ast_elem* newch = new ast_elem;
            newch->name = lex.tokens[i].name;
            aste->children.push_back(newch);
            
            expression(newch, beg,i,level+1);
            //std::cout << ",   ";
            beg = i+1;
            i+=1;
        }
        else
        {
            i+=1;
        }
    }
    
    ast_elem* newch = new ast_elem;
    newch->name = lex.tokens[i].name;
    aste->children.push_back(newch);
    expression(newch, beg,end,level+1);
    
    return 0;
}

}
