#include "ini.hh"

#include <iostream>
#include <fstream>

#include <utility>

#include "../gen/gu.hh"

namespace cr {

ini_config::ini_config() {}

ini_config::ini_config(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file)
    {
        std::cerr << "ERROR: ini file does not exist: " << filepath << std::endl;
        return;
    }
    
    std::string cat("global");
    std::string line;
    bool noteof;
    int lnum = 0;
    do
    {
        noteof = getline_winlin(file, line);
        ++lnum;
        
        line = trim(line);
        
        if (line.size() == 0 || line.at(0) == ';') continue;
        
        if (line.at(0) == '[')
        {
            cat = line.substr(1, line.size()-2);
            continue;
        }
        else
        {
            size_t first = line.find("=");
            if (first == std::string::npos)
            {
                std::cerr << "ERROR: ini format error in line " << lnum << std::endl;
                continue;
            }
            std::string key = line.substr(0,first);
            std::string val = line.substr(first+1);
            key = trim(key);
            val = trim(val);
            if (val.size() > 1 && val.at(0) == '"') val = val.substr(1, val.size()-2);
            
            if (key.size() > 0)
            {
                setvalue(cat, key, val);
            }
            else
            {
                std::cerr << "ERROR: ini format error in line " << lnum << std::endl;
                continue;
            }
        }
    }
    while (noteof);
}

ini_config::~ini_config() {}




void ini_config::setvalue(const std::string& category, const std::string& key, const std::string& value)
{
    auto it = config.find(category);
    
    if (it == config.cend())
    {
        std::map<std::string, std::string> newc;
        newc.insert(std::make_pair(key, value));
        config.insert(std::make_pair(category, newc));
    }
    else
    {
        auto& exc = (*it).second;
        auto it2 = exc.find(key);
        
        if (it2 == exc.cend())
        {
            exc.insert(std::make_pair(key, value));
        }
        else
        {
            std::cerr << "Warning: ini existing value reset." << std::endl;
            (*it2).second = value;
        }
    }
}
bool ini_config::getvalue(const std::string& category, const std::string& key, std::string& value) const
{
    auto it = config.find(category);
    
    if (it == config.cend()) return false;
    
    auto& exc = (*it).second;
    auto it2  = exc.find(key);
        
    if (it2 == exc.cend()) return false;
    
    value = (*it2).second;
    
    return true;
}

void ini_config::getvalue(const std::string& category, const std::string& key, std::string& value, const std::string& def_val) const
{
    bool f = getvalue(category, key, value);
    if (!f) { value = def_val; }
}

bool ini_config::getvalue(const std::string& category, const std::string& key, int& value) const
{
    std::string v;
    bool f = getvalue(category, key, v);
    if (f)
    {
        try
        {
            value = std::stoi(v);
        }
        catch (const std::exception& /*e*/)
        {
            f = false;
        }
    }
    
    return f;
}

void ini_config::getvalue(const std::string& category, const std::string& key, int& value, int def_val) const
{
    std::string v;
    bool f = getvalue(category, key, v);
    if (f)
    {
        try
        {
            value = std::stoi(v);
        }
        catch (const std::exception& /*e*/)
        {
            value = def_val;
        }
    }
    else
    {
        value = def_val;
    }
}

bool ini_config::getvalue(const std::string& category, const std::string& key, float& value) const
{
    std::string v;
    bool f = getvalue(category, key, v);
    if (f)
    {
        try
        {
            value = std::stof(v);
        }
        catch (const std::exception& /*e*/)
        {
            f = false;
        }
    }
    
    return f;
}

void ini_config::getvalue(const std::string& category, const std::string& key, float& value, float def_val) const
{
    std::string v;
    bool f = getvalue(category, key, v);
    if (f)
    {
        try
        {
            value = std::stof(v);
        }
        catch (const std::exception& /*e*/)
        {
            value = def_val;
        }
    }
    else
    {
        value = def_val;
    }
}

}
