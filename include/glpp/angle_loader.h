#ifndef ANGLE_LOADER__H
#define ANGLE_LOADER__H

#include "types.h"

namespace gles2 {  
  gl::gl_proc get_proc_address(const char* proc_name);
}

#endif // #ifndef ANGLE_LOADER__H