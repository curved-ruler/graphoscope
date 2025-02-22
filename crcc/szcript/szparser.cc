
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
void szparser::print_ast(const ast_elem* e, const std::string& prefix, bool left)
{
    std::cout << prefix + (left ? "├─ " : "└─ " ) << e->name;
    std::cout << (e->type == types::FLOAT ? "  float(" : "  int(") << (e->type == types::FLOAT ? floats[e->value] : ints[e->value]) <<")\n";
    
    for (size_t i=0 ; i<e->children.size() ; ++i)
    {
        print_ast(e->children[i], prefix + (left ? std::string("│  ") : std::string("   ")), i != e->children.size()-1 );
    }
}

int szparser::lexer()
{
    lex.add(source);
    status = lex.lexer();
    return status;
}

int szparser::parse_expr(size_t begin, size_t end)
{
    size_t m1 = -1;
    for (size_t j=0 ; j<lex.ops.table.size() ; ++j)
    {
        if (lex.ops.table[j].name == "-" && lex.ops.table[j].arity == 1)
        {
            m1 = j;
        }
    }
    
    for (size_t i = begin ; i<end ; ++i)
    {
        if (lex.tokens[i].name == "-")
        {
            if (i == begin || lex.tokens[i-1].type == token_t::OP ||
                lex.tokens[i-1].name == "(" || lex.tokens[i-1].name == "[" || lex.tokens[i-1].name == "{")
            {
                lex.tokens[i].id = m1;
            }
        }
    }
    
    status = expression(AST,begin,end,0);
    
    floats.clear();
    ints.clear();
    
    status = eval(AST);
    
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
    bool  rootassoc = true;
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
            if (rootassoc ? lex.ops.table[lex.tokens[i].id].prec >= rootprec : lex.ops.table[lex.tokens[i].id].prec > rootprec)
            {
                rootop    = i;
                rootprec  = lex.ops.table[lex.tokens[i].id].prec;
                rootassoc = lex.ops.table[lex.tokens[i].id].assoc;
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
        
        if (lex.ops.table[lex.tokens[rootop].id].arity == 1)
        {
            aste->name = lex.ops.table[lex.tokens[rootop].id].name;
            ast_elem* right = new ast_elem;
            aste->children.push_back(right);
            expression(right, rootop+1,end,level+1);
            
            if (size_t(rootop) >= begin+1)
            {
                std::cout << "ERROR" << std::endl;
            }
        }
        else
        {
            aste->name = lex.ops.table[lex.tokens[rootop].id].name;
            ast_elem* left  = new ast_elem;
            ast_elem* right = new ast_elem;
            aste->children.push_back(left);
            aste->children.push_back(right);
            expression(left, begin,rootop,level+1);
            expression(right, rootop+1,end,level+1);
        }
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

int szparser::eval(ast_elem*& e)
{
    if (e->name == "+")
    {
        eval(e->children[0]);
        eval(e->children[1]);
        
        if (e->children[0]->type == types::FLOAT || e->children[1]->type == types::FLOAT)
        {
            e->type = types::FLOAT;
            float v = getfval(e->children[0]) + getfval(e->children[1]);
            floats.push_back(v);
            e->value = floats.size()-1;
        }
        else
        {
            e->type = types::INT;
            int v = getival(e->children[0]) + getival(e->children[1]);
            ints.push_back(v);
            e->value = ints.size()-1;
        }
    }
    else if (e->name == "-")
    {
        eval(e->children[0]);
        eval(e->children[1]);
        
        if (e->children[0]->type == types::FLOAT || e->children[1]->type == types::FLOAT)
        {
            e->type = types::FLOAT;
            float v = getfval(e->children[0]) - getfval(e->children[1]);
            floats.push_back(v);
            e->value = floats.size()-1;
        }
        else
        {
            e->type = types::INT;
            int v = getival(e->children[0]) - getival(e->children[1]);
            ints.push_back(v);
            e->value = ints.size()-1;
        }
    }
    
    else if (e->name.find('.') != std::string::npos)
    {
        e->type = types::FLOAT;
        float v = std::stof(e->name);
        floats.push_back(v);
        e->value = floats.size()-1;
    }
    else
    {
        e->type = types::INT;
        int v = std::stoi(e->name);
        ints.push_back(v);
        e->value = ints.size()-1;
    }
    
    return 0;
}

int szparser::getival (const ast_elem* e)
{
    if      (e->type == types::FLOAT) return int(floats[e->value]);
    else if (e->type == types::INT)   return ints[e->value];
    
    return 0;
}
float szparser::getfval (const ast_elem* e)
{
    if      (e->type == types::FLOAT) return floats[e->value];
    else if (e->type == types::INT)   return float(ints[e->value]);
    
    return 0;
}



}
