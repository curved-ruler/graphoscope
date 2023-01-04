
#include <ft2build.h>
#include FT_FREETYPE_H

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

typedef unsigned int uint;


//std::string inf("../fonts/DejaVuSansMono.ttf");
//std::string ouf("../out/tex_out_djsm.png");
//std::string outcnf("../out/tex_out_djsm.txt");

std::string inf("../fonts/ter-u16n.bdf");
std::string ouf("../out/f_term16.png");
std::string outcnf("../out/f_term16.conf");

bool bdf = true;

std::vector<unsigned int>  chars;
std::vector<unsigned char> tex;
std::vector<unsigned int> tex_tmp;

//int char_width  = 20;
//int char_height = 20;
//int baseline    = 17;
//int tex_cols    = 16;

int char_width  = 8;
int char_height = 16;
int baseline    = 16;
int tex_cols    = 32;



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
std::string trim (const std::string& s)
{
    size_t first = s.find_first_not_of(" \t\n\r");
    size_t last  = s.find_last_not_of(" \t\n\r");
    
    if (first == std::string::npos) return "";
    
    if (last == std::string::npos) return s.substr(first);
    else return s.substr(first, (last-first+1));
}


void process_bdf(const std::string& f)
{
    std::ifstream ff(f);
    if (!ff) { std::cout << "Error opening: " << f << std::endl; return; }
    
    std::string line;
    std::string line1;
    std::string line2;
    bool rawdata = false;
    while (getline_winlin(ff, line))
    {
        if (line.size() == 0) { continue; }
        
        size_t first_comm_pos = line.find(" ");
        if (first_comm_pos != std::string::npos)
        {
            line1 = line.substr(0, first_comm_pos);
            line2 = line.substr(first_comm_pos+1);
        }
        else
        {
            line1 = line;
            line2 = "";
        }
        
        if (rawdata)
        {
            if (line1 == "ENDCHAR")
            {
                rawdata = false;
                continue;
            }
            else
            {
                unsigned int c;
                std::stringstream ss;
                ss << std::hex << line1;
                ss >> c;
                tex_tmp.push_back(c);
                continue;
            }
        }
        else
        {
            if (line1 == "BITMAP")
            {
                rawdata = true;
                continue;
            }
            else if (line1 == "ENCODING")
            {
                unsigned long ul = std::stoul(line2);
                chars.push_back(uint(ul));
                continue;
            }
        }
    }
}


int main ()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, inf.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -2;
    }
    
    if (!bdf)
    {
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        
        FT_Set_Pixel_Sizes(face, 0, 18);
        /*
        FT_Set_Char_Size(
            face,    // handle to face object
            0,       // char_width in 1/64th of points
            12*64,   // char_height in 1/64th of points
            300,     // horizontal device resolution
            300 );   // vertical device resolution
        */
        
        std::ifstream charset_file("charset.txt");
        if (!charset_file) { std::cout << "Error charset.txt file not found" << std::endl; return -3; }
        std::string line;
        while (getline_winlin(charset_file, line))
        {
            size_t first_comm_pos = line.find(" ");
            if (first_comm_pos != std::string::npos) line = line.substr(0, first_comm_pos);
            line = trim(line);
            
            if (line.size() == 0) { continue; }
            
            chars.push_back(std::stoi(line));
        }
    }
    else
    {
        process_bdf(inf);
    }
    
    int char_cnt = (int)chars.size();
    int tex_rows = char_cnt / tex_cols;
    if (tex_rows*tex_cols < char_cnt) { ++tex_rows; }
    
    int tex_width  = char_width  * tex_cols;
    int tex_height = char_height * tex_rows;
    //unsigned char* tex = new unsigned char[tex_width * tex_height];
    tex.resize(tex_width * tex_height);
    
    if (!bdf)
    {
        for (int y = 0; y < tex_rows; y++)
        for (int x = 0; x < tex_cols; x++)
        {
            int c = y * tex_cols + x;
            int u = x * char_width;
            int v = y * char_height;
            
            if (c >= chars.size() || FT_Load_Char(face, chars[c], FT_LOAD_RENDER))
            {
                //std::cout << "Load char " << FT_Load_Char(face, c, FT_LOAD_RENDER);
                for (int j=0 ; j<char_height ; ++j)
                for (int i=0 ; i<char_width  ; ++i)
                {
                    tex[(v+j) * tex_width + u + i] = 0;
                }
            }
            else
            {
                int ww = (int)(face->glyph->bitmap.width);
                int hh = (int)(face->glyph->bitmap.rows);
                for (int j=0 ; j<char_height ; ++j)
                for (int i=0 ; i<char_width  ; ++i)
                {
                    int ii = i - ((char_width  - ww) / 2);
                    int jj = j - baseline + face->glyph->bitmap_top;
                    
                    if (ii >= 0 && ii < ww && jj >= 0 && jj < hh)
                        tex[(v+j) * tex_width + u + i] = face->glyph->bitmap.buffer[jj*face->glyph->bitmap.width + ii];
                    else
                        tex[(v+j) * tex_width + u + i] = 0;
                }
            }
            //std::cout << face->glyph->bitmap.width  << " " << face->glyph->bitmap.rows << std::endl;
        }
    }
    else
    {
        for (int y = 0; y < tex_rows; y++)
        for (int x = 0; x < tex_cols; x++)
        {
            int c = y * tex_cols + x;
            int u = x * char_width;
            int v = y * char_height;
            
            if (c >= chars.size())
            {
                for (int j=0 ; j<char_height ; ++j)
                for (int i=0 ; i<char_width  ; ++i)
                {
                    tex[(v+j) * tex_width + u + i] = 0;
                }
            }
            else
            {
                for (int j=0 ; j<char_height ; ++j)
                for (int i=0 ; i<char_width  ; ++i)
                {
                    unsigned int mask = 1;
                    for (int k=0 ; k<(7-i) ; ++k) { mask <<= 1; }
                    tex[(v+j) * tex_width + u + i] = (tex_tmp[c*char_height + j] & mask) ? 255 : 0;
                }
            }
        }
    }
    
    stbi_write_png(ouf.c_str(), tex_width, tex_height, 1, tex.data(), tex_width);
    
    std::ofstream f(outcnf.c_str());
    if (!f) { std::cout << "ERROR writing parameters" << std::endl; return -3; }
    
    size_t oufp1 = ouf.find_last_of('/');
    std::string fontname = (oufp1 == std::string::npos) ? ouf : ouf.substr(oufp1+1);
    size_t oufp2 = fontname.find_last_of('.');
    
    unsigned char eol = '\n';
    f << std::endl;
    f << "params = {" << eol;
    f << "    name:        " << fontname.substr(0, oufp2) << "," << eol;
    f << "    imgtype:     " << fontname.substr(oufp2+1)  << "," << eol;
    f << "    char_width:  " << char_width  << "," << eol;
    f << "    char_height: " << char_height << "," << eol;
    f << "    baseline:    " << baseline << "," << eol;
    f << "    char_count:  " << char_cnt << "," << eol;
    f << "    rows: " << tex_rows << "," << eol;
    f << "    cols: " << tex_cols << "," << eol;
    
    int ww = 7;
    f << "    chars: [";
    for (int i=1 ; i<ww ; ++i) f << " ";
    f << "0,";
    int j=2;
    for (size_t i=1 ; i<chars.size() ; ++i)
    {
        f << std::setw(ww) << chars[i];
        if (i<chars.size()-1) { f << ","; }
        if (j == tex_cols)    { f << eol << "            "; j=0; }
        ++j;
    }
    f << "]" << eol;
    
    //for (int i=0 ; i<0 ; ++i)
    
    f << "}" << eol;
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    return 0;
}
