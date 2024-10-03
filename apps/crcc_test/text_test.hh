#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "../../crcc/text/text.hh"



int precomp_test ()
{
    cr::precompiler pc;
    pc.addrep("i", "1");
    pc.addrep("j", "2");
    pc.addrep("k", "3");
    std::string got = pc.process("test/unit_tests/precomp", "tst.in");
    
    std::string expfile;
    if (pc.is_debug_mode()) expfile = "test/unit_tests/precomp/expected_debug";
    else expfile = "test/unit_tests/precomp/expected";
    
    std::string exp = cr::read_file(expfile);
    exp = exp.substr(0,exp.length()-1);
    if (got == exp) {
        return 1;
    }
    
    std::ofstream pcf("test/unit_tests/precomp/tst.out");
    pcf << got << std::endl;
    pcf.close();
    std::cout << "Failed: Precompiler: Check file: unit_tests/precomp/tst.out" << std::endl;
    return 0;
}
/*
int scripter_test () {

    cr::scripter scr("cliffs.conf");
    return 1;
}
*/

void run_text_tests(int& sum, int& succ)
{
    ++sum;
    succ += precomp_test();
    //succ += scripter_test();
}
