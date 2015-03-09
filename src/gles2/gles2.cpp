#include <glpp/gles2.h>

#include <glad/glad.h>
#include <glpp/angle_ext.h>

#include <stdexcept>

namespace gles2 {
   void init() {
#if _MSC_VER
      if (!gladLoadGLES2Loader((GLADloadproc)angle::get_proc_address)) {
#else
      if (!gladLoadGLES2()) {
#endif
         throw std::runtime_error("Cannot initialise GL");
      }
   }
}
