#pragma once

#include <iostream>
#include <string>

#include "../../crcc/gen/gu.hh"

using namespace cr;


int trimt (std::string s, std::string expected)
{
    std::string ts = cr::trim(s);
    if (ts == expected) return 1;
    
    std::cout << "Failed: cr::trim("  << show_whitespace(s) << ")" << std::endl;
    std::cout << "        expected: " << show_whitespace(expected) << ", got: " << show_whitespace(ts) << std::endl;
    return 0;
}

void trim_tests(int& sum, int& succ)
{
    ++sum;
    succ += trimt("", "");
    
    ++sum;
    succ += trimt("  \n \r  \t", "");
    
    ++sum;
    succ += trimt(" a b    ", "a b");
    
    ++sum;
    succ += trimt("\n\ta b\r ", "a b");
}






int pct (std::string p1, std::string p2, std::string expected)
{
    std::string p = cr::combine_path(p1,p2);
    if (p == expected) return 1;
    
    std::cout << "Failed: combine_path(" << show_whitespace(p1) << ", " << show_whitespace(p2) << ")" << std::endl;
    std::cout << "        expected: " << show_whitespace(expected) << ", got: " << show_whitespace(p) << std::endl;
    return 0;
}

void path_tests(int& sum, int& succ)
{
    ++sum;
    succ += pct("", "", "");
    
    ++sum;
    succ += pct("/", "abc", "/abc/");
    
    ++sum;
    succ += pct("a", "", "a/");
    
    ++sum;
    succ += pct("a", "b", "a/b/");
    
    ++sum;
    succ += pct("abc", "/x", "/x/");
    
    ++sum;
    succ += pct("a/b/c/", "../", "a/b/");
    
    ++sum;
    succ += pct("a/b/c", "..", "a/b/");
    
    ++sum;
    succ += pct("a/b/c/d", "../../x1/../x2", "a/b/x2/");
}

void run_gu_tests(int& sum, int& succ)
{
    trim_tests(sum,succ);
    path_tests(sum,succ);
    //std::cout << "Tests: gu: " << succi << "/" << sumi << std::endl;
    
    std::string s("lin1\n\nlin2\nlin3\n\n\n");
    cr::stringlines ss(s);
    for (size_t i=0 ; i<ss.line_cnt() ; ++i)
    {
        std::string l = ss.line(i);
        std::cout << "L" << i+1 << ": " << l << std::endl;
    }
}
