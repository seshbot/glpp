#ifdef WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>

#include <iostream>


namespace
{
   template <typename T>
   void print_log(T msg)
   {
#ifdef WIN32
      ::OutputDebugString((LPCSTR)msg);
#endif
      std::cout << msg;
   }

   template <typename T>
   void print_error(T msg)
   {
#ifdef WIN32
      ::OutputDebugString((LPCSTR)msg);
#endif
      std::cerr << msg;
   }

   void error_callback(int error, const char* description)
   {
      print_error("Error: ");
      print_error(description);
      print_error("\n\tOpenGL error: ");

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

      print_error(err);
      print_error("\n");
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
         print_log("shader info: ");
         print_log(buffer);
         print_log("\n");
         delete[] buffer;

         GLint success;
         glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
         if (success != GL_TRUE)
         {
            print_error("GL Error: Could not get shader info\n");
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
   print_log("starting (cwd: ");

   char cwd[MAX_PATH];
   print_log(_getcwd(cwd, MAX_PATH));
   print_log(")...");

   glfwSetErrorCallback(error_callback);

   if (!glfwInit())
      exit(EXIT_FAILURE);

   // some more info on extensions: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-12-opengl-extensions/
   // glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1); 
   // glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
   // GL_ARB_framebuffer_object available in GL ES 2.0?

   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
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
   
   print_log("GLFW version        : "); print_log(glfwGetVersionString());
   print_log("\nGL_VERSION          : "); print_log(glGetString(GL_VERSION));
   print_log("\nGL_VENDOR           : "); print_log(glGetString(GL_VENDOR));
   print_log("\nGL_RENDERER         : "); print_log(glGetString(GL_RENDERER));
   print_log("\n");
//   print_log("GL_SHADING_LANGUAGE_VERSION : "); print_log(glGetString(GL_SHADING_LANGUAGE_VERSION));

   print_log("creating shader program...\n");

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

   print_log("initialising shader...\n");

   GLint position_loc = glGetAttribLocation(shaderProgram, "position");

   if (position_loc < 0) {
      print_error("GL Error: Unable to get uniform location\n");

      exit(EXIT_FAILURE);
   }

   const float vertexArray[] = {
      0.0, 0.5, 0.0,
      -0.5, 0.0, 0.0,
      0.0, -0.5, 0.0,
      0.5, 0.0, 0.0,
      0.0, 0.5, 0.0
   };

   int t = 0;
   while (!glfwWindowShouldClose(window))
   {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      float ratio = width / (float)height;

      glClearColor(.9f, .9f, .9f, 1.f);

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

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
