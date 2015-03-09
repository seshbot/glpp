#include <glpp/gl2.h>

#include <glad/glad.h>

#include <stdexcept>

namespace gl2 {
   void init() {
      if (!gladLoadGL()) {
         throw std::runtime_error("Cannot initialise GL");
      }
   }
}
