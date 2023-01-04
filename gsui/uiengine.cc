
#include "uiengine.hh"

#include <iostream>
#include <vector>


namespace gs {

uiengine::uiengine(cr::scripter& conf)
{
    bool found = true;
    std::string f;
    found &= conf.getvalue("params.name",     f);
    std::string ext;
    found &= conf.getvalue("params.imgtype" , ext);
    settings.font_name = f + "." + ext;
    if (!found)
    {
        std::cout << "uiengine settings: Font not found" << std::endl;
        return;
    }
    
    found = true;
    found &= conf.getvalue("params.char_width",  settings.im_char_x);
    found &= conf.getvalue("params.char_height", settings.im_char_y);
    //found &&= conf.getvalue("params.baseline",);
    found &= conf.getvalue("params.char_count",  settings.char_count);
    found &= conf.getvalue("params.rows",        settings.im_char_rows);
    found &= conf.getvalue("params.cols",        settings.im_char_cols);
    
    if (!found)
    {
        std::cout << "uiengine settings: Param not found" << std::endl;
        return;
    }
    
    settings.chars = new uint32[settings.char_count];
    conf.getarray("params.chars", settings.chars, settings.char_count);
}

uiengine::~uiengine()
{
}


}
