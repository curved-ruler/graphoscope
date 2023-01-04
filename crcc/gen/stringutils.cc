
#include "stringutils.hh"


namespace cr {

std::string show (const std::string& s)
{
    std::ostringstream ss;
    
    ss << '\"';
    for (size_t i=0 ; i<s.length() ; ++i)
    {
        char c = s[i];
        if      (c == '\t') ss << "\\t";
        else if (c == '\n') ss << "\\n";
        else if (c == '\r') ss << "\\r";
        else                ss << c;
    }
    ss << '\"';
    
    return ss.str();
}

std::string trim (const std::string& s)
{
    size_t first = s.find_first_not_of(" \t\n\r");
    size_t last  = s.find_last_not_of(" \t\n\r");
    
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
