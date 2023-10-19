
#include "stringutils.hh"


namespace cr {


stringlines::stringlines(const std::string& str)
{
    s = str;
    
    lin.push_back(0);
    for (size_t i=0 ; i<str.size() ; ++i)
    {
        if (str[i] == '\n') lin.push_back(i+1);
    }
    lin.push_back(str.size()+1);
}
size_t stringlines::line_cnt() const
{
    return lin.size() - 1;
}
std::string stringlines::line (size_t i) const
{
    if (i<line_cnt()) { return s.substr(lin[i], lin[i+1]-lin[i]-1); }
    return "";
}




std::string show (const std::string& s)
{
    std::ostringstream ss;
    
    ss << '\"';
    for (size_t i=0 ; i<s.length() ; ++i)
    {
        char c = s[i];
        
        if      (c == '\f') ss << "\\f";
        else if (c == '\n') ss << "\\n";
        else if (c == '\r') ss << "\\r";
        else if (c == '\t') ss << "\\t";
        else if (c == '\v') ss << "\\v";
        else                ss << c;
    }
    ss << '\"';
    
    return ss.str();
}

bool isspace (char c)
{
    if (c == ' '  ||
        c == '\f' ||
        c == '\n' ||
        c == '\r' ||
        c == '\t' ||
        c == '\v')
    {
        return true;
    }
    
    return false;
}

std::string trim (const std::string& s)
{
    size_t first = s.find_first_not_of(" \f\n\r\t\v");
    size_t last  = s.find_last_not_of(" \f\n\r\t\v");
    
    if (first == std::string::npos) return "";
    
    if (last == std::string::npos) return s.substr(first);
    else return s.substr(first, (last-first+1));
}

bool parenthesis_check(const std::string& s)
{
    // not proper: immediate negative values check also
    
    int ord    = 0;
    int curly  = 0;
    int square = 0;
    for (size_t i = 0 ; i<s.size() ; ++i)
    {
        switch (s.at(i))
        {
            case '(': ++ord; break;
            case ')': --ord; break;
            case '{': ++curly; break;
            case '}': --curly; break;
            case '[': ++square; break;
            case ']': --square; break;
            default:  break;
        }
    }
    
    return (ord == 0 && curly == 0 && square == 0);
}


}
