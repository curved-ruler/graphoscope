
#include "parser.hh"

#include <cstring>
#include <iostream>
#include <cmath>

namespace cr {

parser::parser (const std::string& src)
{
    source = src;
    AST = new ast_elem;
    AST->name = "program";
}
parser::~parser ()
{
    delete_ast(AST);
}
void parser::delete_ast(ast_elem*& aste)
{
    for (size_t i=0 ; i<aste->children.size() ; ++i) delete_ast(aste->children[i]);
    delete aste;
}
void parser::print_ast(const ast_elem* e, const std::string& prefix, bool left)
{
    std::cout << prefix + (left ? "├─ " : "└─ " ) << e->name << "\n";
    
    for (size_t i=0 ; i<e->children.size() ; ++i)
    {
        print_ast(e->children[i], prefix + (left ? std::string("│  ") : std::string("   ")), i != e->children.size()-1 );
    }
}
void parser::print_ast_eval(const ast_elem* e, const std::string& prefix, bool left)
{
    std::cout << prefix + (left ? "├─ " : "└─ " ) << e->name;
    std::cout << (e->type == types::FLOAT ? "  float(" : "  int(") << (e->type == types::FLOAT ? getfval(e) : getival(e)) <<")\n";
    
    for (size_t i=0 ; i<e->children.size() ; ++i)
    {
        print_ast_eval(e->children[i], prefix + (left ? std::string("│  ") : std::string("   ")), i != e->children.size()-1 );
    }
}

int parser::tokenize()
{
    lex.add(source);
    status = lex.process();
    return status;
}

int parser::parse_expr(size_t begin, size_t end)
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
    return status;
}
int parser::eval_expr(ast_elem*& e)
{
    status = eval(e);
    return status;
}

int parser::expression(ast_elem*& aste, size_t begin, size_t end, int level)
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
    
    int rootop   = -1;
    int rootprec = 0;
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
            aste->toktyp = token_t::OP;
            
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
            aste->toktyp = token_t::OP;
            
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
        aste->toktyp = token_t::FUNCTION;
        
        //std::cout << lex.tokens[begin].name << " ( ";
        expression_list(aste, begin+2,end-1,level+1);
        //std::cout << " )   ";
        return 3;
    }
        
    
    return -1;
}

int parser::expression_list(ast_elem*& aste, size_t begin, size_t end, int level)
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

int parser::eval(ast_elem*& e)
{
    if (e->toktyp == token_t::FUNCTION)
    {
        evalf(e);
    }
    else if (e->toktyp == token_t::OP)
    {
        if      (e->children.size() == 1) evalop1(e);
        else if (e->children.size() == 2) evalop2(e);
    }
    
    else if (e->name.find('.') != std::string::npos)
    {
        e->type = types::FLOAT;
        float v = std::stof(e->name);
        pushfloat(v, e);
    }
    else
    {
        e->type = types::INT;
        int v = std::stoi(e->name);
        pushint(v, e);
    }
    
    return 0;
}
int parser::evalf(ast_elem*& e)
{
    if (e->name == "sqrt")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::sqrt( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "sin")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::sin( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "cos")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::cos( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "tan")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::tan( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "log")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::log( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "log2")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::log2( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "log10")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::log10( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "exp")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::exp( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "exp2")
    {
        eval(e->children[0]);
        
        e->type = types::FLOAT;
        float v = std::exp2( getfval(e->children[0]) );
        pushfloat(v, e);
    }
    else if (e->name == "pow")
    {
        eval(e->children[0]);
        eval(e->children[1]);
        
        e->type = types::FLOAT;
        float v = std::pow( getfval(e->children[0]), getfval(e->children[1]) );
        pushfloat(v, e);
    }
    
    return 1;
}
int parser::evalop1(ast_elem*& e)
{
    eval(e->children[0]);
    e->type = e->children[0]->type;
    
    if (e->name == "-")
    {
        if (e->type == types::FLOAT)
        {
            float v = - getfval(e->children[0]);
            pushfloat(v, e);
        }
        else
        {
            int v = - getival(e->children[0]);
            pushint(v, e);
        }
    }
    
    return 1;
}
int parser::evalop2(ast_elem*& e)
{
    eval(e->children[0]);
    eval(e->children[1]);
    e->type = gettype2(e);
    
    if (e->name == "+")
    {
        if (e->type == types::FLOAT)
        {
            float v = getfval(e->children[0]) + getfval(e->children[1]);
            pushfloat(v, e);
        }
        else
        {
            int v = getival(e->children[0]) + getival(e->children[1]);
            pushint(v, e);
        }
    }
    else if (e->name == "-")
    {
        if (e->type == types::FLOAT)
        {
            float v = getfval(e->children[0]) - getfval(e->children[1]);
            pushfloat(v, e);
        }
        else
        {
            int v = getival(e->children[0]) - getival(e->children[1]);
            pushint(v, e);
        }
    }
    else if (e->name == "*")
    {
        if (e->type == types::FLOAT)
        {
            float v = getfval(e->children[0]) * getfval(e->children[1]);
            pushfloat(v, e);
        }
        else
        {
            int v = getival(e->children[0]) * getival(e->children[1]);
            pushint(v, e);
        }
    }
    else if (e->name == "/")
    {
        if (e->type == types::FLOAT)
        {
            float v = getfval(e->children[0]) / getfval(e->children[1]);
            pushfloat(v, e);
        }
        else
        {
            int v = getival(e->children[0]) / getival(e->children[1]);
            pushint(v, e);
        }
    }
    
    return 1;
}

types::typ parser::gettype2(const ast_elem* e)
{
    if (e->children[0]->type == types::FLOAT || e->children[1]->type == types::FLOAT) { return types::FLOAT; }
    else { return types::INT; }
}

int parser::getival (const ast_elem* e)
{
    if (e->type == types::FLOAT)
    {
        float f = 0.0;
        memcpy(&f, &(e->value[0]), sizeof(float));
        return int(f);
    }
    else if (e->type == types::INT)
    {
        int i = 0;
        memcpy(&i, &(e->value[0]), sizeof(int));
        return i;
    }
    
    return 0;
}
float parser::getfval (const ast_elem* e)
{
    if (e->type == types::FLOAT)
    {
        float f = 0.0;
        memcpy(&f, &(e->value[0]), sizeof(float));
        return f;
    }
    else if (e->type == types::INT)
    {
        int i = 0;
        memcpy(&i, &(e->value[0]), sizeof(int));
        return float(i);
    }
    
    return 0;
}

void parser::pushfloat(float f, ast_elem* e)
{
    uint8 vbyte[4];
    memcpy(vbyte, &f, 4);
    e->value.push_back(vbyte[0]);
    e->value.push_back(vbyte[1]);
    e->value.push_back(vbyte[2]);
    e->value.push_back(vbyte[3]);
}
void parser::pushint(int i, ast_elem* e)
{
    uint8 vbyte[4];
    memcpy(vbyte, &i, 4);
    e->value.push_back(vbyte[0]);
    e->value.push_back(vbyte[1]);
    e->value.push_back(vbyte[2]);
    e->value.push_back(vbyte[3]);
}



}
