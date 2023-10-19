
#include "io.hh"

#include <algorithm>

namespace cr {

const char wsep = '\\';
const char lsep = '/';

#ifdef PLATFORM_WIN
const char separator = wsep;
#else
const char separator = lsep;
#endif

//-----------------

std::string localize_path(const std::string& p)
{
    std::string ret = p;
#ifdef PLATFORM_WIN
    std::replace(ret.begin(), ret.end(), lsep, wsep);
#else
    std::replace(ret.begin(), ret.end(), wsep, lsep);
#endif
    return ret;
}

//-----------------

bool is_abspath (const std::string& path)
{
#ifdef PLATFORM_WIN
    return (path.length() > 1 && path[1] == ':') ? true : false;
#else
    return (path.front() == '/') ? true : false;
#endif
}

//-----------------

std::string spath (const std::string& path)
{
    return (path.length() > 0 && path.back() != separator) ? (path + separator) : path;
}

std::string combine_path (const std::string& pp1, const std::string& pp2)
{
    std::string p1 = spath(pp1);
    std::string p2 = pp2;
    std::string ssep(1, separator);
    if (p2.length() == 0)  return p1;
    if (is_abspath(p2))    return spath(p2);
    
    std::string p2first;
    do {
        size_t pos = p2.find_first_of(separator);
        if (pos != std::string::npos) {
            p2first = p2.substr(0,pos);
            p2 = p2.substr(pos+1);
        } else {
            p2first = p2;
            p2 = "";
        }
        
        if (p2first == "..") {
            if (p1 == "" || p1 == ssep) {
                std::cout << "ERROR: combinePath(" << pp1 << ", " << pp2 << ")" << std::endl;
                return "";
            }
            p1 = p1.substr(0,p1.size()-1);
            size_t pos2 = p1.find_last_of(separator);
            p1 = spath(p1.substr(0,pos2));
        } else {
            p1 += spath(p2first);
        }
    } while (p2 != "");
    
    return p1;
}

std::string get_folder (const std::string& filepath)
{
    std::string pers("..");
    pers[0] = lsep;
    pers[1] = wsep;
    size_t last = filepath.find_last_of(pers);
    
    if (last == std::string::npos || last == filepath.length()-1) return filepath;
    else return filepath.substr(0,last);
}




std::string read_file(const std::string& fname)
{
    std::ifstream in(fname, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    } else {
        std::cout << "ERROR: readfile: file not found: " << fname << std::endl;
        return "";
    }
}

int read_file_b (const char* filename, std::vector<char>& res)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    if (!ifs) { std::cout << "File not found: " << filename << std::endl; return -1; }
    
    std::ifstream::pos_type pos = ifs.tellg();
    res.resize(pos);
    ifs.seekg(0, std::ios::beg);
    ifs.read(&res[0], pos);
    ifs.close();
    
    return 0;
}

// Reads UTF-8 file to str32
void read_file_u32 (const char* filename, str32& res)
{
    std::vector<char> data;
    read_file_b(filename, data);
    if (data.size() == 0) return;
    
    data.push_back(0);
    data.push_back(0);
    data.push_back(0);
    //std::cout << "E: " << (int)(data[0]) << "   " << (int)(data[1]) << "   " << (int)(data[2]) << std::endl;
    uint32 ch = 0;
    int type;
    for (size_t i = 0 ; i<data.size()-3 ; )
    {
        type = unicode_8_to_32(data[i], data[i+1], data[i+2], data[i+3], ch);
        if (type <= 0) { std::cout << "UTF8 error, file: " << filename << " at byte: " << i << std::endl; break; }
        else           { res.data.push_back(ch); i += type; }
    }
}
// Writes str32 to UTF-8 file
void write_file_u32 (const char* filename, const str32& s)
{
    str8 s8;
    char ch[4];
    int type;
    for (size_t i = 0 ; i<s.size() ; ++i)
    {
        type = unicode_32_to_8(s.data[i], ch[0], ch[1], ch[2], ch[3]);
        if (type <= 0) { std::cout << "U32 error, file: " << filename << " at: " << i << std::endl; break; }
        for (int j = 0 ; j<type && j<4 ; ++j)
        {
            s8.data.push_back(ch[j]);
        }
    }
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(&(s8.data[0]), s8.data.size());
    ofs.close();
}

bool getline_winlin(std::ifstream& file, std::string& s)
{
    std::ostringstream ss("");
    char ch;
    while (file >> std::noskipws >> ch)
    {
        if (ch == '\n')
        {
            s = ss.str();
            return true;
        }
        if (ch == '\r') continue;
        ss << ch;
    }
    s = ss.str();
    return false;
}

}
