#pragma once

#include <iostream>
#include <string>

#include "../../crcc/gen/gu.hh"


int trimt (std::string s, std::string expected)
{
    std::string ts = cr::trim(s);
    if (ts == expected) return 1;
    
    std::cout << "Failed: cr::trim(" << cr::show(s) << ")" << std::endl;
    std::cout << "        expected: " << cr::show(expected) << ", got: " << cr::show(ts) << std::endl;
    return 0;
}

void trim_tests(int& sum, int& succ)
{
    int sumv=0, succv=0;
    
    ++sumv;
    succv += trimt("", "");
    
    ++sumv;
    succv += trimt("  \n \r  \t", "");
    
    ++sumv;
    succv += trimt(" a b    ", "a b");
    
    ++sumv;
    succv += trimt("\n\ta b\r ", "a b");
    
    sum += sumv; succ+=succv;
}






int pct (std::string p1, std::string p2, std::string expected)
{
    std::string p = cr::combine_path(p1,p2);
    if (p == expected) return 1;
    
    std::cout << "Failed: combine_path(" << cr::show(p1) << ", " << cr::show(p2) << ")" << std::endl;
    std::cout << "        expected: " << cr::show(expected) << ", got: " << cr::show(p) << std::endl;
    return 0;
}

void path_tests(int& sum, int& succ)
{
    int sumv=0, succv=0;
    
    ++sumv;
    succv += pct("", "", "");
    
    ++sumv;
    succv += pct("/", "abc", "/abc/");
    
    ++sumv;
    succv += pct("a", "", "a/");
    
    ++sumv;
    succv += pct("a", "b", "a/b/");
    
    ++sumv;
    succv += pct("abc", "/x", "/x/");
    
    ++sumv;
    succv += pct("a/b/c/", "../", "a/b/");
    
    ++sumv;
    succv += pct("a/b/c", "..", "a/b/");
    
    ++sumv;
    succv += pct("a/b/c/d", "../../x1/../x2", "a/b/x2/");
    
    
    sum += sumv; succ+=succv;
}

void run_gu_tests(int& sum, int& succ)
{
    int sumi=0, succi=0;
    trim_tests(sumi,succi);
    path_tests(sumi,succi);
    //std::cout << "Tests: gu: " << succi << "/" << sumi << std::endl;
    sum += sumi; succ += succi;
}
