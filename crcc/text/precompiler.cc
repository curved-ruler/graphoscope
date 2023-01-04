
#include "precompiler.hh"


namespace cr {


precompiler::precompiler () {
    debug = false;
}
precompiler::~precompiler () {
    loaded.clear();
    repmap.clear();
}

bool precompiler::is_debug_mode() {
    return debug;
}



int precompiler::addrep (std::string name, std::string val)
{
    repmap[name] = val;
    return (int)(repmap.size());
}
int precompiler::remrep (std::string name)
{
    repmap.erase(name);
    return (int)(repmap.size());
}
bool precompiler::getval (std::string name, std::string& ret)
{
    auto i = repmap.find(name);
    if(i != repmap.end())
    {
        ret = i->second;
        return true;
    }
    else
    {
        return false;
    }
}

std::string precompiler::replace (std::string inp)
{
    std::ostringstream ss("");
    std::ostringstream namess("");
    std::string replace_val;
    bool gather_repname = false;
    for (size_t i=0 ; i<inp.length() ; ++i)
    {
        char curr = inp[i];
        
        if (curr == REPCHAR)
        {
            if (gather_repname)
            {
                if (getval(namess.str(),replace_val))
                {
                    ss << replace_val;
                }
                else
                {
                    ss << REPCHAR << namess.str() << REPCHAR;
                }
                namess.str("");
                gather_repname = false;
            }
            else
            {
                gather_repname = true;
            }
        }
        else // curr != REPCHAR
        {
            if (gather_repname)
            {
                namess << curr;
            }
            else
            {
                ss << curr;
            }
        }
    } // endfor
    
    if (gather_repname)
    {
        ss << REPCHAR << namess.str();
    }
    return ss.str();
}



std::string precompiler::process (std::string path, std::string file) {
    loaded.clear();
    return process(path, file, 0);
}

std::string precompiler::process (std::string pathin, std::string file, int depth) {
    if (depth > MAXDEPTH) {
        std::cout << "ERROR: precompiler: max depth reached" << std::endl;
        return "";
    }
    
    std::string path = spath(pathin);
    std::ifstream filestream(path + file);
    if (!filestream) {
        std::cout << "ERROR: precompiler: file not found: " << path + file << std::endl;
        return "";
    }
    
    bool prevprocessed = true;
    if (std::find(loaded.cbegin(), loaded.cend(), path + file) == loaded.cend()) {
        if (debug) { std::cout << "precompiler: processing file: " << path + file << std::endl; }
        loaded.push_back(path + file);
        prevprocessed = false;
    }
    
    std::stringstream ss("");
    if (debug) { ss << "// --------------- " << path + file << " ---------------" << std::endl; }
    std::string line;
    std::string linetr;
    int linenum = 0;
    bool more = true;
    while (more) {
        more = getline_winlin(filestream, line);
        ++linenum;
        linetr = trim(line);
        
        if (linetr == "#pragma once") {
            if (prevprocessed) {
                if (debug) { ss << "// --------------once--------------" << std::endl; }
                return ss.str();
            } else {
                continue;
            }
        }
        
        if (linetr.length() > 10 && linetr.substr(0,8) == "#include") {
            size_t pos1 = linetr.find_first_of('\"');
            size_t pos2 = linetr.find_last_of('/');
            size_t pos3 = linetr.find_last_of('\"');
            if (pos2 == std::string::npos) pos2 = pos1;
            if (pos1 == std::string::npos || pos3 == std::string::npos || pos3 < pos1 + 2) {
                std::cout << "ERROR: precompiler: #include syntax in file: " << path + file << std::endl;
                std::cout << "         line: " << linenum << std::endl;
                std::cout << "          err: " << line << std::endl;
                return "";
            }
            
            std::string p2 = linetr.substr(pos1+1, pos2-pos1);
            std::string f2 = linetr.substr(pos2+1, pos3-pos2-1);
            ss << process(combine_path(path, p2), f2, depth+1) << std::endl;
            continue;
        }
        
        line = replace(line);
        ss << line << std::endl;
    }
    if (debug) { ss << "// ---------------eof---------------" << std::endl; }
    return ss.str();
}

}
