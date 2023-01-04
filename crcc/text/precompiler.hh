
#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "../gen/gu.hh"


namespace cr {


class precompiler {
    //public:
        // status masks
        //static const unsigned int OK = 0x0;
        //static const unsigned int FOUND_NONEXISTING = 0x1;
        //static const unsigned int FOUND_PAIRLESS = 0x2;
        //static const unsigned int MAXDEPTH_REACHED = 0x4;
        //static const unsigned int FILE_NOT_FOUND = 0x8;
        //static const unsigned int INCLUDE_SYNTAX = 0x10;
        
    private:
        static const int  MAXDEPTH = 20;
        static const char REPCHAR  = '$';
        std::map<std::string, std::string> repmap; // $name$ -> value
        std::vector<std::string> loaded;
        //unsigned int status;
        bool debug;
        
        bool getval (std::string name, std::string& ret);
        std::string process (std::string path, std::string file, int depth);
        
    public:
        precompiler();
        ~precompiler();
        
        bool is_debug_mode();
        
        int addrep (std::string name, std::string val);
        int remrep (std::string name);
        std::string replace (std::string inp);
        
        std::string process (std::string path, std::string file);
};

}
