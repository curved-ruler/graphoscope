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

std::string show_whitespace (const std::string& s);
std::string trim (const std::string& s);
void split    (const std::string& s, std::vector<std::string>& tokens, std::string delimiter);
bool starts_w (const std::string& s, const std::string& w);


bool parenthesis_check(const std::string& s);


bool isspace (char c);

}
