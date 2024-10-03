
#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "../../crcc/szcript/szcript.hh"

void sztest_1(int& sum, int& succ)
{
    sz::szparse sz;
    std::string szcr = cr::read_file("../../apps/crcc_test/szcript/sz_tst_0_ok.txt");
    sz.add(szcr);
    sz.lexer();
    
    for (size_t i=0 ; i<sz.tokens.size() ; ++i)
    {
        std::cout << sz.tokens[i].name << "  ";
    }
    std::cout << "\n\n";
    
    sum+=1; succ+=1;
}

void run_szcript_tests(int& sum, int& succ)
{
    sztest_1(sum, succ);
}
