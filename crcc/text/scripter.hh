#pragma once

#include <string>
#include <vector>

#include "../base.hh"

namespace cr {



struct script_variable
{
    std::string name;
    std::string value;
};

class scripter
{
    private:
        std::vector< script_variable > variables;
        
        int lnum;
        
    public:
        scripter();
        scripter(const std::string& filepath);
        ~scripter();
        
        void setobj(const std::string& prefix, char eq, const std::string& valuedesc, int depth);
        
        void setvalue(const std::string& name, const std::string& value);
        
        bool getvalue(const std::string& name, std::string& value) const;
        void getvalue(const std::string& name, std::string& value, const std::string& def_val) const;
        
        bool getvalue(const std::string& name, int& value) const;
        void getvalue(const std::string& name, int& value, int def_val) const;
        bool getvalue(const std::string& name, float& value) const;
        void getvalue(const std::string& name, float& value, float def_val) const;
        
        int  arraysize(const std::string& name) const;
        void getarray(const std::string& name, std::string* values, int max) const;
        void getarray(const std::string& name, int* values, int max) const;
        void getarray(const std::string& name, uint32* values, int max) const;
        void getarray(const std::string& name, float* values, int max) const;
};

}