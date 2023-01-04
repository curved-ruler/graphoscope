#pragma once

#include <string>
#include <map>

namespace cr {

class ini_config
{
    private:
        std::map< std::string, std::map<std::string, std::string> > config;
        
    public:
        ini_config();
        ini_config(const std::string& filepath);
        ~ini_config();
        
        void setvalue(const std::string& category, const std::string& key, const std::string& value);
        
        bool getvalue(const std::string& category, const std::string& key, std::string& value) const;
        void getvalue(const std::string& category, const std::string& key, std::string& value, const std::string& def_val) const;
        
        bool getvalue(const std::string& category, const std::string& key, int& value) const;
        void getvalue(const std::string& category, const std::string& key, int& value, int def_val) const;
        bool getvalue(const std::string& category, const std::string& key, float& value) const;
        void getvalue(const std::string& category, const std::string& key, float& value, float def_val) const;
};

}