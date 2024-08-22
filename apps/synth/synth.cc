
#include <iostream>
#include <string>

#include <apps/appbase/scene.hh>
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

#define SOKOL_AUDIO_IMPL
#include <sokol_audio.h>

#define OSCILLOSCOPE_N (1000)
#define RENDER_SAMPLES (735)
#define SOKOL_BUF_SIZE (128)


glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void char_callback        (GLFWwindow* /*wnd*/, unsigned int codepoint)                      { global_loop->chr(codepoint); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }



// ffmpeg -formats | grep PCM
// ffmpeg -i wish.m4a -f f32le output.raw
size_t read_file_f32 (const char* filename, float*& data)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    if (!ifs) { return 0; }
    
    std::ifstream::pos_type pos = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    size_t ret = pos / sizeof(float);
    data = new float[ret];
    ifs.read((char*)(data), sizeof(float) * ret);
    
    ifs.close();
    
    return ret;
}



class renderer_noshade
{
    private:
        gsgl::renderprog* vf;
        int status;
        cr::render_mode* settings;
        
    public:
        renderer_noshade()  { vf = nullptr; }
        ~renderer_noshade() { delete vf; };
        
        void setup       (cr::render_mode* rm)
        {
            settings = rm;
            
            std::string vs = R"VS(
#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
out vec3 color;

void main ()
{
    gl_Position  = vec4(pos.x, pos.y, 0.0, 1.0);
    gl_PointSize = 1.0;
    color = col;
}
)VS";
            std::string fs = R"FS(
#version 460 core

in  vec3 color;
out vec4 out_color;

void main()
{
    out_color = vec4(color, 1.0);
}
)FS";
            vf = new gsgl::renderprog(vs, fs);
            status = vf->get_state();
        }
        void init_render (unsigned int w, unsigned int h)
        {
            glUseProgram(vf->get_progid());
            
            glDisable(GL_CULL_FACE);
            
            glViewport(0, 0, w, h);
        }
        void pre_render  ()
        {
            glClearBufferfv(GL_COLOR, 0, settings->back);
        }
        void render      (const cr::rrr_buffers& bufs)
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            
            glBindBuffer(GL_ARRAY_BUFFER, bufs.pnt_buf);
    
            glEnableVertexAttribArray(obj::POS_BUF_ID);
            glEnableVertexAttribArray(obj::COL_BUF_ID);
            
            glVertexAttribPointer(obj::POS_BUF_ID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(NULL));
            glVertexAttribPointer(obj::COL_BUF_ID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
            
            glDrawArrays(GL_POINTS, 0, bufs.pntn);
            
            glDisableVertexAttribArray(obj::POS_BUF_ID);
            glDisableVertexAttribArray(obj::COL_BUF_ID);
        }
};



class synth_scene : public scene
{
    private:
        float* freqs;
        float* t0s;
        float  T;
        float  dT;
        float  dA;
        float  MA;
        int*   keys;
        bool*  pressed;
        float* amps;
        int    PN;
        int    N;
        
        bool lows;
        bool highs;
        float freqplus;
        float freqminus;
        
        std::vector<float> main_buf;
        size_t main_bufi;
        
        int frame_count;
        
        int render_bufi;
        renderer_noshade rrr;
        float render_buf[OSCILLOSCOPE_N * 6];
        cr::rrr_buffers rdata;
        
        float sokol_buf[SOKOL_BUF_SIZE];
        int   sokol_bufi;
        
        bool openfile;
        bool savefile;
        std::string target_dir;
        std::string sound_dir;
        std::string soundfile;
        float* sound_buf;
        size_t sound_bufi;
        size_t sound_bufn;
        
        
    public:
        synth_scene(cr::scripter& _conf) : scene(_conf)
        {
            bool found = sysconf.getvalue("folders.target", target_dir);
            target_dir = cr::spath(target_dir);
            found = sysconf.getvalue("folders.sound_dir", sound_dir);
            sound_dir = cr::spath(sound_dir);
            
            sound_buf = nullptr;
            
            found = sysconf.getvalue("data.play", soundfile);
            if (found && soundfile.length() > 0)
            {
                openfile = true;
                std::cout << "Playing: " << (target_dir+sound_dir+soundfile).c_str() << std::endl;
                sound_bufn = read_file_f32((target_dir+sound_dir+soundfile).c_str(), sound_buf);
            }
            
            if (!openfile)
            {
                found = sysconf.getvalue("data.record", soundfile);
                if (found && soundfile.length() > 0)
                {
                    savefile = true;
                }
            }
            
            N = 13;
            freqs   = new float[N];
            amps    = new float[N];
            t0s     = new float[N];
            keys    = new int[N];
            pressed = new bool[N];
            
            
            freqs[0]  = 440.0000f;
            freqs[1]  = 466.1638f;
            freqs[2]  = 493.8833f;
            freqs[3]  = 523.2511f;
            freqs[4]  = 554.3653f;
            freqs[5]  = 587.3295f;
            freqs[6]  = 622.2540f;
            freqs[7]  = 659.2551f;
            freqs[8]  = 698.4565f;
            freqs[9]  = 739.9888f;
            freqs[10] = 783.9909f;
            freqs[11] = 830.6094f;
            freqs[12] = 880.0000f;
            
            keys[0]  = keys::A;
            keys[1]  = keys::W;
            keys[2]  = keys::S;
            keys[3]  = keys::D;
            keys[4]  = keys::R;
            keys[5]  = keys::F;
            keys[6]  = keys::T;
            keys[7]  = keys::G;
            keys[8]  = keys::H;
            keys[9]  = keys::U;
            keys[10] = keys::J;
            keys[11] = keys::I;
            keys[12] = keys::K;
            
            for (int i=0 ; i<N ; ++i)
            {
                pressed[i] = false;
                t0s[i]     = 0.0f;
                amps[i]    = 0.0f;
            }
            PN = 0;
            T  = 0.0f;
            dA = 0.002f;
            MA = 0.12f;
            dT = 1.0f / 44100.0f;
            
            main_bufi   = 0;
            sokol_bufi  = 0;
            render_bufi = 0;
            
            frame_count = 0;
            
            lows  = false;
            highs = false;
            freqplus  = 0.5f;
            freqminus = 0.5f;
            
            rrr.setup(&rmode);
            init_gpu();
        }
        
        virtual ~synth_scene()
        {
            if (savefile)
            {
                std::ofstream ofs((target_dir+sound_dir+soundfile).c_str(), std::ios::binary|std::ios::ate);
                if (ofs)
                {
                    std::cout << "Saving " << (target_dir+sound_dir+soundfile).c_str() << std::endl;
                    ofs.write((char*)(main_buf.data()), sizeof(float) * main_buf.size());
                    ofs.close();
                }
            }
            
            delete[] freqs;
            delete[] amps;
            delete[] t0s;
            delete[] keys;
            delete[] pressed;
            delete[] sound_buf;
            
            //delete[] render_buf;
            glDeleteBuffers(1, &(rdata.pnt_buf));
        }
        
        int findkey (int k)
        {
            for (int i=0 ; i<N ; ++i)
            {
                if (k == keys[i]) return i;
            }
            return -1;
        }
        void calcpn ()
        {
            PN = 0;
            for (int i=0 ; i<N ; ++i) { if (pressed[i]) ++PN; }
        }
        
        virtual void step(float /*dt*/)
        {
            //std::cout << "S: " << dt << std::endl;
        }
        
        virtual void save() {}
        
        void init_gpu ()
        {
            rdata.pntn    = OSCILLOSCOPE_N;
            rdata.pnt_buf = 0;
            
            for (unsigned int i=0 ; i<rdata.pntn ; ++i)
            {
                render_buf[i*6 + 0] = (2.0f * i - rdata.pntn) / float(rdata.pntn);
                render_buf[i*6 + 1] = 0.0f;
                render_buf[i*6 + 2] = 0.0f;
                render_buf[i*6 + 3] = 1.0f;
                render_buf[i*6 + 4] = 1.0f;
                render_buf[i*6 + 5] = 1.0f;
            }
            
            glGenBuffers(1, &(rdata.pnt_buf));
            glBindBuffer(GL_ARRAY_BUFFER, rdata.pnt_buf);
            glBufferData(GL_ARRAY_BUFFER, rdata.pntn * 6 * sizeof(float), render_buf, GL_DYNAMIC_DRAW);
        }
        
        virtual void render()
        {
            //calcpn();
            
            for (int i=0 ; i<RENDER_SAMPLES ; ++i)
            {
                for (int i=0 ; i<N ; ++i)
                {
                    if (pressed[i])
                    {
                        if (amps[i] < MA) amps[i] += dA;
                    }
                    else
                    {
                        if (amps[i] > 0.0f) amps[i] -= dA;
                        if (amps[i] < 0.0f) amps[i] = 0.0f;
                    }
                }
                
                float sum = 0.0f;
                for (int i=0 ; i<N ; ++i)
                {
                    float FI = (freqs[i] * 2.0f*freqplus) / (2.0f*freqminus);
                    
                    sum += amps[i]  * std::sin(2.0f*cr::pi*FI*T - t0s[i]);
                    //sum +=  0.4f  * std::sin(2.0f*cr::pi*FI*T);
                    
                    if (highs)
                    {
                        sum += (amps[i] / 4.0f) * std::sin(2.0f*cr::pi*(FI*2.0f)*T - t0s[i]);
                        sum += (amps[i] / 8.0f) * std::sin(2.0f*cr::pi*(FI*4.0f)*T - t0s[i]);
                    }
                    
                    if (lows)
                    {
                        sum += (amps[i] / 4.0f) * std::sin(2.0f*cr::pi*(FI/2.0f)*T - t0s[i]);
                        sum += (amps[i] / 8.0f) * std::sin(2.0f*cr::pi*(FI/4.0f)*T - t0s[i]);
                    }
                }
                
                if (openfile && sound_bufi < sound_bufn-2)
                {
                    sum += sound_buf[sound_bufi];
                    sum += sound_buf[sound_bufi+1];
                    sum *= 0.5f;
                    sound_bufi += 2;
                }
                
                //if (PN > 1.5f) { sum = sum / float(PN); }
                main_buf.push_back(sum);
                
                
                T += dT;
            }
            
            ++frame_count;
            
            //if (main_size - render_buf_i >= OSCILLOSCOPE_N)
            //{
                //int j = render_buf_i;
                //for (int i=0 ; i<OSCILLOSCOPE_N ; ++i)
                //{
                //    render_buf[i*6 + 1] = 0.5f * main_buf[j];
                //    ++j;
                //}
                //render_buf_i += OSCILLOSCOPE_N;
            if (frame_count >= 30)
            {
                frame_count = 0;
                
                size_t main_size = main_buf.size();
                for (int i=0 ; i<OSCILLOSCOPE_N ; ++i)
                {
                    render_buf[i*6 + 1] = 0.5f * main_buf[main_size - OSCILLOSCOPE_N + i];
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, rdata.pnt_buf);
                glBufferSubData(GL_ARRAY_BUFFER, 0, rdata.pntn * 6 * sizeof(float), render_buf);
            }
            
            
            int num_frames = saudio_expect();
            
            for (int i = 0; i < num_frames; i++)
            {
                if (main_bufi < main_buf.size())
                {
                    sokol_buf[sokol_bufi] = main_buf[main_bufi];
                    ++main_bufi;
                
                    ++sokol_bufi;
                    if (sokol_bufi == SOKOL_BUF_SIZE)
                    {
                        sokol_bufi = 0;
                        saudio_push(sokol_buf, SOKOL_BUF_SIZE);
                    }
                }
                else
                {
                    break;
                }
            }
            
            framebuf->use();
            
            rrr.init_render(uint32(rmode.screen_w), uint32(rmode.screen_h));
            rrr.pre_render();
            
            rrr.render(rdata);
            
            framebuf->render();
        }
        
        virtual void keyaction (int key, int action, int /*mods*/)
        {
            if (key == keys::CONTROL && action == keys::PRESS)   { freqplus  = 1.0f; return; }
            if (key == keys::CONTROL && action == keys::RELEASE) { freqplus  = 0.5f; return; }
            if (key == keys::SHIFT   && action == keys::PRESS)   { freqminus = 1.0f; return; }
            if (key == keys::SHIFT   && action == keys::RELEASE) { freqminus = 0.5f; return; }
            
            if (key == keys::BUT_1 && action == keys::PRESS) { lows  = !lows;  return; }
            if (key == keys::BUT_2 && action == keys::PRESS) { highs = !highs; return; }
            
            int k = findkey(key);
            if (k == -1) return;
            
            if (action == keys::RELEASE) { pressed[k] = false; }
            if (action == keys::PRESS)   { pressed[k] = true; t0s[k] = T; }
        }
};

int main ()
{
    std::string conf = cr::read_file("synth.conf");
    cr::scripter ttconf(conf);
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new synth_scene(ttconf);
    global_loop->set_scene(s);
    
    
    glfwSetKeyCallback(global_loop->window, key_callback);
    
    
    saudio_desc desc = {};
    desc.sample_rate = 44100;
    desc.num_channels = 1;
    saudio_setup(&desc);
    
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete s;
    delete global_loop;
    return ret;
}
