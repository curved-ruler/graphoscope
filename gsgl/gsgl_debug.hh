#pragma once

#include "gsgl_config.hh"

#define GL_CHECK_ERROR() gsgl::check_error(__FILE__, __LINE__)

namespace gsgl {

GLenum check_error(const char *file, int line);

}
