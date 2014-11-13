#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <cstdlib>
#include "utils.h"
#include "gl.h"


#ifdef WIN32
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
      gl::init();
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      exit(EXIT_FAILURE);
   }

   try {
      bool reload_program = false;
      auto key_handler = [&](gl::context & ctx, gl::Key key, int scancode, gl::KeyAction action, int mods) {
         if (key == gl::KEY_ESCAPE && action == gl::KEY_ACTION_PRESS)
            ctx.set_should_close();
         if (key == gl::KEY_R && action == gl::KEY_ACTION_PRESS)
            reload_program = true;
      };

      gl::context context { key_handler };

#ifdef USE_GLEW
      GLenum err = glewInit();
      if (err != GLEW_OK)
      {
         utils::log(utils::LOG_ERROR, "GLEW init error\n");
         exit(EXIT_FAILURE);
      }

      if (!GLEW_VERSION_2_1)
      {
         utils::log(utils::LOG_ERROR, "GLEW v2.1 not available!\n");
         exit(EXIT_FAILURE);
      }
#endif

      utils::log(utils::LOG_INFO, "%s\n", context.info().c_str());
      
      auto create_program = [] {
         utils::log(utils::LOG_INFO, "compiling shader program...\n");

         auto program = gl::program { 
               gl::shader::create_from_file("shaders/2d.vert", gl::shader::Vertex),
               gl::shader::create_from_file("shaders/2d.frag", gl::shader::Fragment)
            };

         utils::log(utils::LOG_INFO, " ...success\n");

         auto logs = program.compile_logs();
         if (logs.length() > 0) {
            utils::log(utils::LOG_INFO, "%s", logs.c_str());
         }

         return program;
      };

      auto program = create_program();

      GLint position_loc = glGetAttribLocation(program.id(), "position");
      if (position_loc < 0) { throw std::runtime_error("GL Error: Unable to get uniform location"); }

      while (!context.closing())
      {
         if (reload_program) {
            try {
               program = create_program();
               position_loc = glGetAttribLocation(program.id(), "position");
            }
            catch (gl::shader_compile_error const & ex) {
               utils::log(utils::LOG_ERROR, "%s\n", ex.what());
               utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
            }
            reload_program = false;
         }

         auto dims = context.frame_buffer_dims();

         glClearColor(.9f, .9f, .9f, 1.f);

         glViewport(0, 0, dims.x, dims.y);
         glClear(GL_COLOR_BUFFER_BIT);

         static const float vertexArray[] = {
            0.0, 0.5, 0.0,
            -0.5, 0.0, 0.0,
            0.0, -0.5, 0.0,
            0.5, 0.0, 0.0,
            0.0, 0.5, 0.0
         };

         glVertexAttribPointer(position_loc, 3, GL_FLOAT, false, 0, vertexArray);
         glEnableVertexAttribArray(position_loc);
         glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

         context.swap();
      }

      program.destroy();
      context.destroy();
      gl::shutdown();

      exit(EXIT_SUCCESS);
   }
   catch (gl::shader_compile_error const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());

      gl::shutdown();
      exit(EXIT_FAILURE);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());

      gl::shutdown();
      exit(EXIT_FAILURE);
   }
}
