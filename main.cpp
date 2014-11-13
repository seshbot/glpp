#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>

#  define USE_OPENGL_ES_2
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <iostream>
#include "utils.h"
#include "gl.h"


namespace
{
   void print_shader_info_log(GLuint shader)
   {
      GLint  length;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

      if (length)
      {
         char* buffer = new char[length];
         glGetShaderInfoLog(shader, length, NULL, buffer);
         utils::log(utils::LOG_INFO, "shader info: %s\n", buffer);
         delete[] buffer;

         GLint success;
         glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
         if (success != GL_TRUE)
         {
            utils::log(utils::LOG_ERROR, "GL Error: Could not get shader info\n");
            // TODO: get error
            exit(EXIT_FAILURE);
         }
      }
   }

   GLuint load_shader(const char * source, GLenum type)
   {
      GLuint shader = glCreateShader(type);


      glShaderSource(shader, 1, &source, NULL);
      glCompileShader(shader);

      print_shader_info_log(shader);

      return shader;
   }
}

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
      utils::log(utils::LOG_ERROR, ex.what());
      exit(EXIT_FAILURE);
   }

   try {
      auto key_handler = [](gl::context & ctx, gl::Key key, int scancode, gl::KeyAction action, int mods) {
         if (key == gl::KEY_ESCAPE && action == gl::KEY_ACTION_PRESS)
            ctx.set_should_close();
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

      utils::log(utils::LOG_INFO, context.info().c_str());

      utils::log(utils::LOG_INFO, "creating shader program...\n");
      
      const char vertex_src[] = "\
         attribute vec4 position;                     \n\
                                                      \n\
         void main() {                                \n\
            gl_Position = position;                   \n\
         }";

      const char fragment_src[] = "\
         void main() {                                \n\
            gl_FragColor = vec4(.2, .2, .2, 1.);      \n\
         }";

      GLuint vertexShader = load_shader(vertex_src, GL_VERTEX_SHADER);
      GLuint fragmentShader = load_shader(fragment_src, GL_FRAGMENT_SHADER);

      GLuint shaderProgram = glCreateProgram();
      glAttachShader(shaderProgram, vertexShader);
      glAttachShader(shaderProgram, fragmentShader);

      glLinkProgram(shaderProgram);    // link the program
      glUseProgram(shaderProgram);    // and select it for usage

      utils::log(utils::LOG_INFO, "initialising shader...\n");

      GLint position_loc = glGetAttribLocation(shaderProgram, "position");

      if (position_loc < 0) {
         utils::log(utils::LOG_ERROR, "GL Error: Unable to get uniform location\n");

         exit(EXIT_FAILURE);
      }

      while (!context.closing())
      {
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

      glDeleteProgram(shaderProgram);

      context.destroy();
      gl::shutdown();

      exit(EXIT_SUCCESS);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, ex.what());

      gl::shutdown();
      exit(EXIT_FAILURE);
   }
}
