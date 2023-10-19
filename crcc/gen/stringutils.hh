#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

namespace cr {

struct stringlines
{
    std::string s;
    std::vector<size_t> lin;
    
    stringlines(const std::string& str);
    
    size_t      line_cnt()         const;
    std::string line    (size_t i) const;
};

std::string show (const std::string& s);

std::string trim (const std::string& s);

bool parenthesis_check(const std::string& s);


bool isspace (char c);

}
