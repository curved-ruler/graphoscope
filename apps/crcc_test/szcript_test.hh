
#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "../../crcc/szcript/szcript.hh"


void sztest_1(int& sum, int& succ)
{
    sz::szparse sz;
    std::string szcr = cr::read_file("../../apps/crcc_test/szcript/sz_tst_1_ok.txt");
    sz.add(szcr);
    
    std::cout << "KEYS: ";
    for (size_t i=0 ; i<sz.keys.table.size() ; ++i) { std::cout << sz.keys.table[i] << "   "; }
    std::cout << std::endl;
    
    std::cout << "OPS : ";
    for (size_t i=0 ; i<sz.ops.table.size() ; ++i) { std::cout << sz.ops.table[i].name << "   "; }
    std::cout << std::endl;
    
    sz.lexer();
    
    for (size_t i=0 ; i<sz.tokens.size() ; ++i)
    {
        std::cout << i << "   line: ";
        std::cout << sz.tokens[i].line << "   ";
        std::string s = sz::tokentype_str(sz.tokens[i].type);
        std::cout << s << ": ";
        for (size_t i = 0 ; i<9-s.size() ; ++i) { std::cout << " "; }
        std::cout << sz.tokens[i].name << "   ";
        std::cout << sz.tokens[i].id << std::endl;
        
        //std::cout << sz.tokens[i].name << "  ";
    }
    std::cout << "\n\n";
    
    sum+=1; succ+=1;
}

void run_szcript_tests(int& sum, int& succ)
{
    sztest_1(sum, succ);
}
