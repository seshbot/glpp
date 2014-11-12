#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>

#  define USE_OPENGL_ES_2
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <GLFW/glfw3.h>

#include <iostream>
#include "utils.h"


namespace
{
   void error_callback(int error, const char* description)
   {
      utils::log(utils::LOG_ERROR, "Error: %s\n", description);

      const char * err = "No Error";
      GLenum glErr = glGetError();
      switch (glErr)
      {
      case GL_INVALID_ENUM:      err = "GL_INVALID_ENUM";      break;
      case GL_INVALID_VALUE:     err = "GL_INVALID_VALUE";     break;
      case GL_INVALID_OPERATION: err = "GL_INVALID_OPERATION"; break;
      case GL_STACK_OVERFLOW:    err = "GL_STACK_OVERFLOW";    break;
      case GL_STACK_UNDERFLOW:   err = "GL_STACK_UNDERFLOW";   break;
      case GL_OUT_OF_MEMORY:     err = "GL_OUT_OF_MEMORY";     break;
      default:                   err = "unrecognised enum value!";
      }

      utils::log(utils::LOG_ERROR, "\n\tOpenGL error: %s\n", err);
   }

   static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
   {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
         glfwSetWindowShouldClose(window, GL_TRUE);
   }
}

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

   glfwSetErrorCallback(error_callback);

   if (!glfwInit())
      exit(EXIT_FAILURE);

   // some more info on extensions: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-12-opengl-extensions/
   // glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1); 
   // glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
   // GL_ARB_framebuffer_object available in GL ES 2.0?

#ifdef USE_OPENGL_ES_2
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

   GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      exit(EXIT_FAILURE);
   }

   glfwMakeContextCurrent(window);
   glfwSetKeyCallback(window, key_callback);
   
#ifdef USE_GLEW
   GLenum err = glewInit();
   if (err != GLEW_OK)
   {
      print_error("GLEW init error\n");
      exit(EXIT_FAILURE);
   }

   if (!GLEW_VERSION_2_1)
   {
      print_error("GLEW v2.1 not available!\n");
      exit(EXIT_FAILURE);
   }
#endif

   utils::log(utils::LOG_INFO, "GLFW version        : %s\n", glfwGetVersionString());
   utils::log(utils::LOG_INFO, "GL_VERSION          : %s\n", glGetString(GL_VERSION));
   utils::log(utils::LOG_INFO, "GL_VENDOR           : %s\n", glGetString(GL_VENDOR));
   utils::log(utils::LOG_INFO, "GL_RENDERER         : %s\n", glGetString(GL_RENDERER));
//   utils::log(utils::LOG_INFO, "GL_SHADING_LANGUAGE_VERSION : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

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

   while (!glfwWindowShouldClose(window))
   {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);

      glClearColor(.9f, .9f, .9f, 1.f);

      glViewport(0, 0, width, height);
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

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   glDeleteProgram(shaderProgram);

   glfwDestroyWindow(window);
   glfwTerminate();

   exit(EXIT_SUCCESS);
}
