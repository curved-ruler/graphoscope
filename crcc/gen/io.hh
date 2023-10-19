
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "../base.hh"
#include "string.hh"

namespace cr {

std::string localize_path  (const std::string& p);
bool        is_abspath     (const std::string& path);
std::string spath          (const std::string& path);  // strong path: 'a/b/', weak path: 'a/b'
std::string combine_path   (const std::string& p1, const std::string& p2);
std::string get_folder     (const std::string& file);

std::string read_file      (const std::string& fname);
int         read_file_b    (const char* filename, std::vector<char>& res);
// Reads UTF-8 file to str32
void        read_file_u32  (const char* filename, str32& res);
// Writes str32 to UTF-8 file
void        write_file_u32 (const char* filename, const str32& s);
bool        getline_winlin (std::ifstream& file, std::string& s);

}
