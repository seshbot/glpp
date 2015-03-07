#include <glpp/gles2.h>

#include <glad/glad.h>
#include <glpp/angle_ext.h>

#include <stdexcept>

namespace gles2 {
   void init() {
//      typedef void* (*GLADloadproc)(const char *name);
//      using gl_proc = void(*)();
//      gl_proc get_proc_address(const char* proc_name) {
      if (!gladLoadGLES2Loader((GLADloadproc)angle::get_proc_address)) {
         throw std::runtime_error("Cannot initialise GL");
      }
   }
}