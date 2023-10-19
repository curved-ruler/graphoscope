#include "scripter.hh"

#include <iostream>
#include <fstream>

#include "../gen/gu.hh"

namespace cr {

scripter::scripter() {}

scripter::scripter(const std::string& script_str)
{
    std::string current_statement;
    std::string obj_prefix("");
    std::string line;
    
    cr::stringlines slines(script_str);
    size_t lmax = slines.line_cnt();
    lnum = 0;
    
    do
    {
        current_statement = "";
        
        bool closed;
        do
        {
            line = slines.line(lnum);
            ++lnum;
            
            size_t first_comm_pos = line.find("//");
            if (first_comm_pos != std::string::npos) line = line.substr(0, first_comm_pos);
            line = trim(line);
            
            if (line.size() == 0) { closed = false; continue; }
            
            current_statement += line;
            
            closed = parenthesis_check(current_statement);
            
        } while (!closed && lnum <= lmax);
        
        // trailing empty lines in file
        if (current_statement.size() == 0) continue;
        
        //std::cout << "PROC: " << current_statement << std::endl;
        
        setobj("", '=', current_statement, 0);
    }
    while (lnum <= lmax);
    
    //for (size_t i = 0 ; i<variables.size() ; ++i) { std::cout << "N: " << variables[i].name << " V: " << variables[i].value << std::endl; }
}

scripter::~scripter() {}




void scripter::setobj(const std::string& prefix, char eq, const std::string& valuedesc, int depth)
{
    //if (depth > 20)
    
    size_t first_eq = valuedesc.find(eq);
    if (first_eq == std::string::npos)
    {
        std::cerr << "WARNING: script: no equal sign found. Processing line: " << lnum << std::endl;
        return;
    }
    
    std::string key = valuedesc.substr(0,first_eq);
    std::string val = valuedesc.substr(first_eq+1);
    key = trim(key);
    val = trim(val);
    
    key = prefix + key;
    
    if (key.size() > 0)
    {
        if (val.size() == 0)
        {
            setvalue(key, "");
            return;
        }
        
        if (val.at(0) == '"')
        {
            setvalue(key, val.substr(1, val.size()-2));
            return;
        }
        
        if (val.at(0) == '[')
        {
            setvalue(key, val.substr(1, val.size()-2));
            return;
        }
        
        if (val.at(0) == '{')
        {
            val = val.substr(1, val.size()-2);
            val = trim(val);
            while (val.size() > 0)
            {
                std::string firstval("");
                std::string chk;
                size_t comma = 0;
                do
                {
                    comma = val.find(',', comma+1);
                    chk = (comma == std::string::npos) ? "" : val.substr(0,comma);
                } while (!parenthesis_check(chk) && (comma != std::string::npos));
                
                if (comma == std::string::npos)
                {
                    firstval = val;
                    val = "";
                }
                else
                {
                    firstval = trim(val.substr(0,comma));
                    val      = trim(val.substr(comma+1));
                }
                
                setobj(key + '.', ':', firstval, depth+1);
            }
            return;
        }
        
        setvalue(key, val);
    }
    else
    {
        std::cerr << "ERROR: script format error in line " << lnum << std::endl;
    }
}




void scripter::setvalue(const std::string& name, const std::string& value)
{
    size_t i;
    for (i = 0 ; i<variables.size() ; ++i) { if (variables[i].name == name) break; }
    
    if (i == variables.size())
    {
        variables.push_back(script_variable{ name, value });
    }
    else
    {
        variables[i].value = value;
        std::cerr << "WARNING: scripter: existing value reset." << std::endl;
    }
}
bool scripter::getvalue(const std::string& name, std::string& value) const
{
    size_t i;
    for (i = 0 ; i<variables.size() ; ++i) { if (variables[i].name == name) break; }
    
    if (i == variables.size()) { return false; }
    
    value = variables[i].value;
    return true;
}

void scripter::getvalue(const std::string& name, std::string& value, const std::string& def_val) const
{
    bool f = getvalue(name, value);
    if (!f) { value = def_val; }
}

bool scripter::getvalue(const std::string& name, int& value) const
{
    std::string v;
    bool f = getvalue(name, v);
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

void scripter::getvalue(const std::string& name, int& value, int def_val) const
{
    std::string v;
    bool f = getvalue(name, v);
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

bool scripter::getvalue(const std::string& name, float& value) const
{
    std::string v;
    bool f = getvalue(name, v);
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

void scripter::getvalue(const std::string& name, float& value, float def_val) const
{
    std::string v;
    bool f = getvalue(name, v);
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

int scripter::arraysize(const std::string& name) const
{
    std::string v;
    bool f = getvalue(name, v);
    if (!f) return -1;
    
    int cnt = 1;
    for (size_t i=0 ; i<v.size() ; ++i) { if (v.at(i) == ',') ++cnt; }
    return cnt;
}
void scripter::getarray(const std::string& name, std::string* values, int max) const
{
    std::string v;
    bool f = getvalue(name, v);
    if (!f) return;
    
    int i = 0;
    while ((i < max) && (v.size() > 0))
    {
        std::string firstv;
        size_t first_comma = v.find(',');
        if (first_comma == std::string::npos)
        {
            firstv = v;
            v = "";
        }
        else
        {
            firstv = trim(v.substr(0,first_comma));
            v      = trim(v.substr(first_comma+1));
        }
        
        values[i] = firstv;
        
        ++i;
    }
}
void scripter::getarray(const std::string& name, int* values, int max) const
{
    std::string v;
    bool f = getvalue(name, v);
    if (!f) return;
    
    int i = 0;
    while ((i < max) && (v.size() > 0))
    {
        std::string firstv;
        size_t first_comma = v.find(',');
        if (first_comma == std::string::npos)
        {
            firstv = v;
            v = "";
        }
        else
        {
            firstv = trim(v.substr(0,first_comma));
            v      = trim(v.substr(first_comma+1));
        }
        
        try
        {
            values[i] = std::stoi(firstv);
        }
        catch (const std::exception& /*e*/)
        {
            std::cerr << "WARNING: scripter: failed int conversion." << std::endl;
        }
        
        ++i;
    }
}
void scripter::getarray(const std::string& name, uint32* values, int max) const
{
    std::string v;
    bool f = getvalue(name, v);
    if (!f) return;
    
    int i = 0;
    while ((i < max) && (v.size() > 0))
    {
        std::string firstv;
        size_t first_comma = v.find(',');
        if (first_comma == std::string::npos)
        {
            firstv = v;
            v = "";
        }
        else
        {
            firstv = trim(v.substr(0,first_comma));
            v      = trim(v.substr(first_comma+1));
        }
        
        try
        {
            values[i] = uint32(std::stoul(firstv));
        }
        catch (const std::exception& /*e*/)
        {
            std::cerr << "WARNING: scripter: failed int conversion." << std::endl;
        }
        
        ++i;
    }
}
void scripter::getarray(const std::string& name, float* values, int max) const
{
    std::string v;
    bool f = getvalue(name, v);
    if (!f) return;
    
    int i = 0;
    while ((i < max) && (v.size() > 0))
    {
        std::string firstv;
        size_t first_comma = v.find(',');
        if (first_comma == std::string::npos)
        {
            firstv = v;
            v = "";
        }
        else
        {
            firstv = trim(v.substr(0,first_comma));
            v      = trim(v.substr(first_comma+1));
        }
        
        try
        {
            values[i] = std::stof(firstv);
        }
        catch (const std::exception& /*e*/)
        {
            std::cerr << "WARNING: scripter: failed float conversion." << std::endl;
        }
        
        ++i;
    }
}

}
