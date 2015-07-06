#ifdef _MSC_VER
#  include <windows.h>
#endif

#include <glpp/glpp.h>
#include <glpp/assets.h>
#include <glpp/utils.h>
#include <glpp/scene.h>

#include <glm/gtx/transform.hpp>

#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

#include <random>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <map>


namespace gl {
#ifdef _MSC_VER
   using namespace gles2;
   using angle::vertex_attrib_divisor;
   using angle::draw_arrays_instanced;
#else
   using namespace gl2;
   using namespace osx;
#endif
}


// forward declarations
void buffer_demo();
void text_demo();
void texture_batches_demo();
void triangle_demo();


#ifdef _MSC_VER
int CALLBACK WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR lpCmdLine,
   int nCmdShow
)
#else
int main()
#endif
{
   utils::log(utils::LOG_INFO, "starting (cwd: %s)\n", utils::getcwd().c_str());

   try {
      buffer_demo();
   }
   catch (glpp::shader_compile_error const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }

   glpp::shutdown();

   exit(EXIT_SUCCESS);
}
