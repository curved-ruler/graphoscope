
#pragma once

#include <string>
#include <crcc/crcc.hh>
#include "../gsgl_config.hh"


namespace gsgl {


class renderprog
{
    private:
        GLuint program;
        GLuint vshader, fshader, gshader;
        std::string vsSource;
        std::string fsSource;
        std::string gsSource;
        bool useGeometry;
        int status;
    
    public:
        renderprog(std::string& vs, std::string& fs);
        renderprog(std::string& vs, std::string& gs, std::string& fs);
        ~renderprog();
        
        GLuint get_progid() const;
        void init();
        int get_state() const;
        
        
        bool set_uniform_m4(const char* name, cr::mat4 m);
        bool set_uniform_v3(const char* name, cr::vec3 v);
        
        bool set_uniform_int(const char* name, int value);
        bool set_uniform_float(const char* name, float value);
        
        bool set_uniform_float_array(const char* name, float* values, int len);
};

}
