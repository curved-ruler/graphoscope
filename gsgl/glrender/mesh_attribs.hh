#pragma once

#include <cstddef>
#include "../gsgl_config.hh"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace obj {

const GLuint POS_BUF_ID = 0; // vertex buffer id
const GLuint COL_BUF_ID = 1; // color  buffer id
const GLuint NOR_BUF_ID = 2; // normal buffer id

}
