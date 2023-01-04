
#include "renderprog.hh"

#include <string>
#include <iostream>


namespace gsgl {


renderprog::renderprog(std::string& vs, std::string& fs)
{
    vshader = 0;
    gshader = 0;
    fshader = 0;
    useGeometry = false;
    vsSource = vs;
    fsSource = fs;
    init();
}

renderprog::renderprog(std::string& vs, std::string& gs, std::string& fs)
{
    vshader = 0;
    gshader = 0;
    fshader = 0;
    useGeometry = true;
    vsSource = vs;
    gsSource = gs;
    fsSource = fs;
    init();
}

renderprog::~renderprog()
{
    glDetachShader(program, vshader);
    glDeleteShader(vshader);
    
    if (useGeometry)
    {
        glDetachShader(program, gshader);
        glDeleteShader(gshader);
    }
    
    glDetachShader(program, fshader);
    glDeleteShader(fshader);
    
    //glDeleteProgram(program);
}


int renderprog::get_state () const
{
    return status;
}

GLuint renderprog::get_progid () const
{
    return program;
}

void renderprog::init ()
{
    GLint success = 0;
    GLint length = 0;
    vshader = glCreateShader(GL_VERTEX_SHADER);
    if (useGeometry) {
        gshader = glCreateShader(GL_GEOMETRY_SHADER);
    }
    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();
    GLchar* log = 0;
    
    
    const GLchar* s1 = vsSource.c_str();
    glShaderSource(vshader, 1, &s1, NULL);
    glCompileShader(vshader);
    
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &length);
        log = new GLchar[length+2];
        GLint x;
        glGetShaderInfoLog(vshader, length+1, &x, log);
        //log[length+1] = 0;
        std::cout << "Vertex shader ERROR:" << std::endl << log << std::endl;
        status = -1;
        delete[] log;
        return;
    }
    
    
    const GLchar* s2 = fsSource.c_str();
    glShaderSource(fshader, 1, &s2, NULL);
    glCompileShader(fshader);
    
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &length);
        log = new GLchar[length+2];
        GLint x;
        glGetShaderInfoLog(fshader, length+1, &x, log);
        //log[length+1] = 0;
        std::cout << "Fragment shader ERROR:" << std::endl << std::endl;
        status = -2;
        delete[] log;
        return;
    }
    
    if (useGeometry) {
        const GLchar* s3 = gsSource.c_str();
        glShaderSource(gshader, 1, &s3, NULL);
        glCompileShader(gshader);
    
        glGetShaderiv(gshader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            glGetShaderiv(gshader, GL_INFO_LOG_LENGTH, &length);
            log = new GLchar[length+2];
            GLint x;
            glGetShaderInfoLog(gshader, length+1, &x, log);
            //log[length+1] = 0;
            std::cout << "Geometry shader ERROR:" << std::endl << log << std::endl;
            status = -3;
            delete[] log;
            return;
        }
    }

    
    glAttachShader(program, vshader);
    if (useGeometry) {
        glAttachShader(program, gshader);
    }
    glAttachShader(program, fshader);

    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        log = new GLchar[length+2];
        GLint x;
        glGetProgramInfoLog(program, length+1, &x, log);
        //log[length+1] = 0;
        std::cout << "Shader link ERROR:" << std::endl << log << std::endl;
        status = -4;
        delete[] log;
        return;
    }
    
    status = 1;
}


// Possibly check for GL_INVALID_VALUE, GL_INVALID_OPERATION, but there are not return value (?)
bool renderprog::set_uniform_m4(const char* name, cr::mat4 m)
{
    GLint m_location = glGetUniformLocation(program, name);
    //std::cout << GL_INVALID_VALUE << std::endl;
    //std::cout << GL_INVALID_OPERATION << std::endl;
    if (m_location != -1)
    {
        glUniformMatrix4fv(m_location, 1, GL_TRUE, m.data);
        return true;
    }
    else
    {
#ifdef C_GSGL_DEBUG
        std::cout << "ERROR: Uniform variable: " << name << std::endl;
#endif
        return false;
    }
}

bool renderprog::set_uniform_v3(const char* name, cr::vec3 v)
{
    GLint m_location = glGetUniformLocation(program, name);
    if (m_location != -1)
    {
        glUniform3f(m_location, v.x, v.y, v.z);
        return true;
    }
    else
    {
#ifdef C_GSGL_DEBUG
        std::cout << "ERROR: Uniform variable: " << name << std::endl;
#endif
        return false;
    }
}

bool renderprog::set_uniform_float(const char* name, float value)
{
    GLint m_location = glGetUniformLocation(program, name);
    if (m_location != -1)
    {
        glUniform1f(m_location, value);
        return true;
    }
    else
    {
#ifdef C_GSGL_DEBUG
        std::cout << "ERROR: Uniform variable: " << name << std::endl;
#endif
        return false;
    }
}

bool renderprog::set_uniform_int(const char* name, int value)
{
    GLint m_location = glGetUniformLocation(program, name);
    if (m_location != -1)
    {
        //std::cout << "setUnfiormInt: " << value << std::endl;
        glUniform1i(m_location, value);
        return true;
    }
    else
    {
#ifdef C_GSGL_DEBUG
        std::cout << "ERROR: Uniform variable: " << name << std::endl;
#endif
        return false;
    }
}

bool renderprog::set_uniform_float_array(const char* name, float* values, int len)
{
    GLint m_location = glGetUniformLocation(program, name);
    if (m_location != -1)
    {
        //for (int i=0 ; i<len ; ++i) std::cout << values[i] << "  ";
        //std::cout << std::endl;
        glUniform1fv(m_location, len, values);
        return true;
    }
    else
    {
#ifdef C_GSGL_DEBUG
        std::cout << "ERROR: Uniform variable: " << name << std::endl;
#endif
        return false;
    }
}

}
