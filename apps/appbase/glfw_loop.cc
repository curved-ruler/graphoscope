
#include "glfw_loop.hh"

#include <ctime>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

//#define STBI_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>




glfw_loop::glfw_loop (cr::scripter& config_file)
{
    bool found;
    int _w, _h;
    config_file.getvalue("window.width",  _w, 600);
    config_file.getvalue("window.height", _h, 600);
    
    DEF_WIDTH = _w;  DEF_HEIGHT = _h;
    W = DEF_WIDTH;   H = DEF_HEIGHT;
    fullscreen = false;
    
    minframetime = 16666.6f;

    scr_counter = 0;
    scr_channel_comps = 3;
    screen_s = new unsigned char[W * H * scr_channel_comps];

    mouse_drag = false;
    eat_cursor = false;
    
    int mm;
    mouse_move_event = false;
    config_file.getvalue("input.mouse_move", mm, 0);
    if (mm > 0) { mouse_move_event = true; }
    
    found = config_file.getvalue("folders.target", target_dir);
    
    if (!found)
    {
        std::cerr << "ERROR: config: \"folders.target\" not found" << std::endl;
    }
    
    target_dir = cr::spath(target_dir);
    found = config_file.getvalue("folders.screen_dir", screen_dir);
    if (found)
    {
        screen_dir = cr::spath(target_dir + screen_dir);
    }
    else
    {
        screen_dir = target_dir;
    }
    
    gif_dir = cr::spath(screen_dir + "gif");
    
    config_file.getvalue("folders.screen_prefix", screen_prefix);
    
    
    config_file.getvalue("window.name", appname, "Graphoscope");
    
    
    
    srand((unsigned)time(NULL));
    char cc[2];
    cc[1] = 0;
    for (int i=0 ; i<7 ; i++) {
        cc[0] = (rand() % 26) + 'a';
        sessionid.append(cc);
    }
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwSwapInterval(1);


    //window = glfwCreateWindow(WIDTH, HEIGHT, "NewWorld", glfwGetPrimaryMonitor(), nullptr);
    window = glfwCreateWindow(W, H, appname.c_str(), nullptr, nullptr);
    glfwSetWindowPos(window, 30, 30);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    //glewExperimental = GL_TRUE;
    //if (glewInit() != GLEW_OK)
    //{
    //    std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
    //}
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    
    glfwSwapInterval(0);
    
    // GLFW bug
    //glGetError();
    
    create_keymap();
}

glfw_loop::~glfw_loop ()
{
    glfwTerminate();
}
void glfw_loop::done ()
{
    delete[] screen_s;
}
void glfw_loop::set_scene (scene* s) { data = s; data->resize_screen(W, H); }

void glfw_loop::save_screen (bool gif)
{
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, screen_s);
    
    unsigned int row = W * scr_channel_comps;
    unsigned char* screen_s_tmp = new unsigned char[H * row];
    for (unsigned int i=0 ; i<H ; ++i)
    {
        std::memcpy(&screen_s_tmp[i * row],
                    &screen_s[(H-1-i) * row],
                    row);
    }
    
    std::stringstream savename("");
    savename << screen_prefix;
    savename << sessionid;
    savename << "_";
    savename << std::setfill('0') << std::setw(3) << scr_counter;
    savename << ".png";
    std::string savestr = gif ? cr::spath(gif_dir) + savename.str() : cr::spath(screen_dir) + savename.str();
    //lodepng_encode24_file(savestr.c_str(), screen_s_tmp, W, H);
    stbi_write_png(savestr.c_str(), W, H, scr_channel_comps, screen_s_tmp, W*scr_channel_comps);
    
    std::cout << "Screenshot: " << savestr.c_str() << std::endl;
    
    delete[] screen_s_tmp;
    ++scr_counter;
}

void glfw_loop::create_keymap ()
{
    keymap =
    {
        {GLFW_KEY_LEFT_SHIFT,   keys::SHIFT},
        {GLFW_KEY_LEFT_CONTROL, keys::CONTROL},
        {GLFW_KEY_LEFT_ALT,     keys::ALT},
        
        {GLFW_KEY_UP,    keys::UP},
        {GLFW_KEY_DOWN,  keys::DOWN},
        {GLFW_KEY_LEFT,  keys::LEFT},
        {GLFW_KEY_RIGHT, keys::RIGHT},
    
        {GLFW_KEY_KP_SUBTRACT, keys::MINUS},
        {GLFW_KEY_KP_ADD,      keys::PLUS},
        
        {GLFW_KEY_ENTER,      keys::ENTER},
        {GLFW_KEY_BACKSPACE,  keys::BACKSPACE},
        
        {GLFW_KEY_Q,     keys::Q},
        {GLFW_KEY_W,     keys::W},
        {GLFW_KEY_E,     keys::E},
        {GLFW_KEY_R,     keys::R},
        {GLFW_KEY_T,     keys::T},
        {GLFW_KEY_Z,     keys::Z},
        {GLFW_KEY_U,     keys::U},
        {GLFW_KEY_I,     keys::I},
        {GLFW_KEY_O,     keys::O},
        {GLFW_KEY_P,     keys::P},
        {GLFW_KEY_A,     keys::A},
        {GLFW_KEY_S,     keys::S},
        {GLFW_KEY_D,     keys::D},
        {GLFW_KEY_F,     keys::F},
        {GLFW_KEY_G,     keys::G},
        {GLFW_KEY_H,     keys::H},
        {GLFW_KEY_J,     keys::J},
        {GLFW_KEY_K,     keys::K},
        {GLFW_KEY_L,     keys::L},
        {GLFW_KEY_Y,     keys::Y},
        {GLFW_KEY_X,     keys::X},
        {GLFW_KEY_C,     keys::C},
        {GLFW_KEY_V,     keys::V},
        {GLFW_KEY_B,     keys::B},
        {GLFW_KEY_N,     keys::N},
        {GLFW_KEY_M,     keys::M},
        
        {GLFW_KEY_S,     keys::S},
        {GLFW_KEY_W,     keys::W},
        {GLFW_KEY_A,     keys::A},
        {GLFW_KEY_D,     keys::D},
        {GLFW_KEY_Q,     keys::Q},
        {GLFW_KEY_E,     keys::E},
        {GLFW_KEY_X,     keys::X},
        {GLFW_KEY_Y,     keys::Y},
        {GLFW_KEY_Z,     keys::Z},
    
        // Problem: keyboard layouts {GLFW_KEY_0,     keys::BUT_0},
        {GLFW_KEY_1,     keys::BUT_1},
        {GLFW_KEY_2,     keys::BUT_2},
        {GLFW_KEY_3,     keys::BUT_3},
        {GLFW_KEY_4,     keys::BUT_4},
        {GLFW_KEY_5,     keys::BUT_5},
        {GLFW_KEY_6,     keys::BUT_6},
        {GLFW_KEY_7,     keys::BUT_7},
        {GLFW_KEY_8,     keys::BUT_8},
        {GLFW_KEY_9,     keys::BUT_9},
    
        {GLFW_KEY_F1,    keys::F1},
        {GLFW_KEY_F2,    keys::F2},
        {GLFW_KEY_F3,    keys::F3},
        {GLFW_KEY_F4,    keys::F4},
        {GLFW_KEY_F5,    keys::F5},
        {GLFW_KEY_F6,    keys::F6},
        {GLFW_KEY_F7,    keys::F7},
        {GLFW_KEY_F8,    keys::F8},
        {GLFW_KEY_F9,    keys::F9},
        {GLFW_KEY_F10,   keys::F10},
        {GLFW_KEY_F11,   keys::F11},
        {GLFW_KEY_F12,   keys::F12}
    };
}




void glfw_loop::toggleFullscreen ()
{
    if (fullscreen)
    {
        W = DEF_WIDTH;
        H = DEF_HEIGHT;
        glfwSetWindowMonitor(window, nullptr, 0, 0, W, H, 0);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetWindowPos(window, 30, 30);
        fullscreen = false;
    }
    else
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        W = mode->width;
        H = mode->height;
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, W, H, mode->refreshRate);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        fullscreen = true;
    }
    
    eat_cursor = mouse_move_event && fullscreen;
    
    std::cout << "New screen size: [" << W << ", " << H << "]" << std::endl;
    data->resize_screen(W, H);
    delete[] screen_s;
    screen_s = new unsigned char[W * H * scr_channel_comps];
}

int glfw_loop::find_key (int glfw_key)
{
    auto it = keymap.find(glfw_key);
    if (it == keymap.cend())
    {
        return keys::UNKNOWN_VALUE;
    }
    return (*it).second;
}
int glfw_loop::find_action (int glfw_act)
{
    if (glfw_act == GLFW_PRESS)   return keys::PRESS;
    if (glfw_act == GLFW_RELEASE) return keys::RELEASE;
    if (glfw_act == GLFW_REPEAT)  return keys::REPEAT;
    else return keys::UNKNOWN_VALUE;
}
int glfw_loop::map_mods (int glfw_mods)
{
    int ret = 0;
    if (glfw_mods & GLFW_MOD_CONTROL)  ret |= keys::CONTROL_BIT;
    if (glfw_mods & GLFW_MOD_SHIFT)    ret |= keys::SHIFT_BIT;
    if (glfw_mods & GLFW_MOD_ALT)      ret |= keys::ALT_BIT;
    return ret;
}
int glfw_loop::find_mousebutton (int glfw_mb)
{
    if (glfw_mb == GLFW_MOUSE_BUTTON_LEFT)   return keys::MOUSE_L;
    if (glfw_mb == GLFW_MOUSE_BUTTON_RIGHT)  return keys::MOUSE_R;
    if (glfw_mb == GLFW_MOUSE_BUTTON_MIDDLE) return keys::MOUSE_M;
    else return keys::UNKNOWN_VALUE;
}

void glfw_loop::key (int keycode, int /*scancode*/, int action, int mods)
{
    //std::cout << keycode << std::endl;
    if (keycode == GLFW_KEY_ESCAPE)
    {
        if (action == GLFW_PRESS)
        {
            if (fullscreen)
            {
                toggleFullscreen();
            }
            else
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
        }
        return;
    }
    
    if (keycode == GLFW_KEY_F1)
    {
        if (action == GLFW_PRESS)
        {
            mouse_move_event = !mouse_move_event;
            eat_cursor = mouse_move_event && fullscreen;
        }
        return;
    }
    
    if (keycode == GLFW_KEY_F11)
    {
        if (action == GLFW_PRESS) toggleFullscreen();
        return;
    }
    
    if (keycode == GLFW_KEY_F8)
    {
        if (action == GLFW_PRESS)
        {
            save_screen(false);
        }
        return;
    }
    
    
    if (keycode == (GLFW_KEY_S) && (mods & GLFW_MOD_CONTROL))
    {
        if (action == GLFW_PRESS) data->save();
        return;
    }
    
    if (keycode == (GLFW_KEY_C) && (mods & GLFW_MOD_CONTROL))
    {
        if (action == GLFW_PRESS) glfwSetClipboardString(NULL, data->to_clipboard());
        return;
    }
    
    if (keycode == (GLFW_KEY_V) && (mods & GLFW_MOD_CONTROL))
    {
        if (action == GLFW_PRESS) data->from_clipboard(glfwGetClipboardString(NULL));
        return;
    }
    
    
    int scene_action =  find_action(action);
    if (scene_action == keys::UNKNOWN_VALUE) return;
    int scene_key    =  find_key(keycode);
    int mods2        =  map_mods(mods);
    
    if (scene_key == keys::UNKNOWN_VALUE && action == GLFW_PRESS)
    {
        //data->keyaction(scancode, scene_action, mods2);
        std::cout << "Key not found: " << keycode << std::endl;
        return;
    }
    
    data->keyaction(scene_key, scene_action, mods2);
}

void glfw_loop::chr (unsigned int codepoint)
{
    data->chr(codepoint);
}

void glfw_loop::scroll (double xoffset, double yoffset)
{
    data->mousescroll((float)xoffset, (float)yoffset);
}

void glfw_loop::cursorpos (double xpos, double ypos)
{
    if (mouse_drag)
    {
        //glfwGetCursorPos(window, &mx, &my);
        mx = xpos;
        my = ypos;
        data->mousedrag((float)xpos, (float)ypos, (float)(mxo-mx), (float)(-myo+my), dragbutton);
        mxo = mx;
        myo = my;
    }
    else if (mouse_move_event)
    {
        mx = xpos;
        my = ypos;
        data->mousemove((float)xpos, (float)ypos, (float)(mxo-mx), (float)(-myo+my));
        mxo = mx;
        myo = my;
        
        
        if (eat_cursor)
        {
            glfwSetCursorPos(window, W / 2.0, H / 2.0);
            mxo = W / 2.0;
            myo = H / 2.0;
        }
    }
}

void glfw_loop::mousebutton (int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &mxo, &myo);
        mouse_drag = true;
        dragbutton = find_mousebutton(button);
    }
    else if (action == GLFW_RELEASE)
    {
        mouse_drag = false;
    }
    
    int scene_action = find_action(action);
    if (scene_action == keys::REPEAT)
    {
        std::cout << "MOUSE BUTTON REPEAT" << std::endl;
        return;
    }
    if (scene_action == keys::UNKNOWN_VALUE) return;
    
    int scene_key = find_mousebutton(button);
    if (scene_key == keys::UNKNOWN_VALUE)
    {
        std::cout << "Mouse button not found: " << button << std::endl;
        return;
    }
    
    data->mousebutton(scene_key, scene_action, mods);
}




void glfw_loop::one_loop ()
{
    t2 = std::chrono::steady_clock::now();
    dt = t2 - t1;
    
    float slp = minframetime - std::chrono::duration_cast<time_meas>(dt).count();
    if (slp > 200.0f)
    {
      //cout << "Sleep: " << slp << endl;
      std::this_thread::sleep_for(time_meas((int)slp - 100));
    }
    
    t2 = std::chrono::steady_clock::now();
    dt = t2 - t1;
    //std::cout << "dt: " << std::chrono::duration_cast<time_meas>(dt).count() << std::endl;
    
    t1 = t2;
    
    glfwPollEvents();
    
    data->step(std::chrono::duration_cast<time_meas>(dt).count() / minframetime);
    //data->pre_render();
    data->render();
    glfwSwapBuffers(window);
    
#if 0
    save_screen(true);
#endif
}

int glfw_loop::loop ()
{
    t1 = std::chrono::steady_clock::now();
    
    data->resize_screen(W, H);
    data->init_render();
    
    while (!glfwWindowShouldClose(window))
    {
        one_loop();
    }

    return 0;
}
