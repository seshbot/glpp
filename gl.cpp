#include "gl.h"

#ifdef WIN32
#  include <GLES2/gl2.h>

#  define USE_OPENGL_ES_2
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <GLFW/glfw3.h>

#include <vector>
#include <utility>
#include <atomic>
#include <mutex>

#include "utils.h"


namespace {

   std::string get_shader_info_log(gl::id_t shader_id)
   {
      GLint  length;
      glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

      if (length) {
         char* buffer = new char[length];
         glGetShaderInfoLog(shader_id, length, NULL, buffer);

         std::string message(buffer);
         delete[] buffer;
         return message;
      }

      return "";
   }

   void delete_program_shaders(gl::id_t id) {
      if (0 == id) return;

      GLint shader_count;
      glGetProgramiv(id, GL_ATTACHED_SHADERS, &shader_count);

      std::vector<GLuint> shaders(shader_count);
      glGetAttachedShaders(id, shader_count, nullptr, shaders.data());

      for (auto s : shaders) { glDeleteShader(s); }
   }

   GLenum to_glenum(gl::shader::Type type) {
      switch (type) {
      case gl::shader::Vertex: return GL_VERTEX_SHADER;
      case gl::shader::Fragment: return GL_FRAGMENT_SHADER;
      default: throw gl::error("unsupported shader type");
      }
   }

   gl::uniform::Type gl_to_uniform_type(GLuint type) {
      switch (type) {
      case GL_BYTE: return gl::uniform::Type::Byte;
      case GL_UNSIGNED_BYTE: return gl::uniform::Type::UByte;
      case GL_SHORT: return gl::uniform::Type::Short;
      case GL_UNSIGNED_SHORT: return gl::uniform::Type::UShort;
      case GL_INT: return gl::uniform::Type::Int;
      case GL_UNSIGNED_INT: return gl::uniform::Type::UInt;
      case GL_FLOAT: return gl::uniform::Type::Float;
      case GL_FIXED: return gl::uniform::Type::Fixed;

      case GL_FLOAT_VEC2: return gl::uniform::Type::FloatVec2;
      case GL_FLOAT_VEC3: return gl::uniform::Type::FloatVec3;
      case GL_FLOAT_VEC4: return gl::uniform::Type::FloatVec4;
      case GL_INT_VEC2: return gl::uniform::Type::IntVec2;
      case GL_INT_VEC3: return gl::uniform::Type::IntVec3;
      case GL_INT_VEC4: return gl::uniform::Type::IntVec4;
      case GL_BOOL: return gl::uniform::Type::Bool;
      case GL_BOOL_VEC2: return gl::uniform::Type::BoolVec2;
      case GL_BOOL_VEC3: return gl::uniform::Type::BoolVec3;
      case GL_BOOL_VEC4: return gl::uniform::Type::BoolVec4;
      case GL_FLOAT_MAT2: return gl::uniform::Type::FloatMat2;
      case GL_FLOAT_MAT3: return gl::uniform::Type::FloatMat3;
      case GL_FLOAT_MAT4: return gl::uniform::Type::FloatMat4;
      case GL_SAMPLER_2D: return gl::uniform::Type::Sampler2d;
      case GL_SAMPLER_CUBE: return gl::uniform::Type::SamplerCube;
      default: return gl::uniform::Type::Unknown;
      }
   }
}

namespace gl
{

   /**
   * class shader
   *
   */


   shader_compile_error::shader_compile_error(std::string source_name, shader_compile_error const & parent)
      : error(parent.what()) {
      log_ = std::string(" --- ") + source_name + " --- \n" + parent.log_;
   }

   shader_compile_error::shader_compile_error(id_t shader_id)
      : error("could not compile shader")
      , log_(get_shader_info_log(shader_id)) {
   }

   shader::shader(std::string const & source, shader::Type type)
      : id_(glCreateShader(to_glenum(type)))
   {
      const char * srcdata = source.c_str();
      glShaderSource(id_, 1, &srcdata, NULL);
      glCompileShader(id_);

      GLint success;
      glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
      if (success != GL_TRUE) {
         throw shader_compile_error(id_);
         // TODO: delete shader?
      }

      compile_log_ = get_shader_info_log(id_);
   }

   shader shader::create_from_file(std::string const & filename, shader::Type type)
   try {
      return create_from_source(utils::read_file(filename), type);
   }
   catch (shader_compile_error & ex) {
      throw shader_compile_error(filename, ex);
   }

   shader shader::create_from_source(std::string const & source, shader::Type type) {
      return{ source, type };
   }

   shader::~shader() {
      glDeleteShader(id_);
   }

   shader::shader(shader && other)
      : id_(other.id_)
      , type_(other.type_)
      , compile_log_(std::move(other.compile_log_)) {
      other.id_ = 0;
   }

   shader & shader::operator=(shader && other) {
      std::swap(id_, other.id_);
      type_ = other.type_;
      compile_log_ = std::move(other.compile_log_);
      return *this;
   }

  /**
   * class uniform
   *
   */

   uniform uniform::INVALID = {"INVALID", -1, 1, uniform::Type::Unknown};

   uniform::uniform(std::string const & name, int location, int size, Type type)
      : name_(name), location_(location), size_(size), type_(type) {
   }

   uniform::uniform(uniform && other) {
      *this = std::move(other);
   }

   uniform & uniform::operator=(uniform && other) {
      name_ = other.name_;
      location_ = other.location_;
      size_ = other.size_;
      type_ = other.type_;

      return *this;
   }

   void uniform::set(int val) {
      if (!is_valid()) { utils::log(utils::LOG_INFO, "cannot set uniform - program does not contain '%s'", name_.c_str()); return; }
      glUniform1i(location_, val);
   }

   void uniform::set(float val) {
      if (!is_valid()) { utils::log(utils::LOG_INFO, "cannot set uniform - program does not contain '%s'", name_.c_str()); return; }
      glUniform1f(location_, val);
   }


  /**
   * class program
   *
   */

   program::program()
      : id_(glCreateProgram()) {

   }

   program::program(shader s)
      : program() {
      try {
         attach(std::move(s));

         reload();
      }
      catch (...) {
         destroy();
         throw;
      }
   }

   program::program(shader s1, shader s2)
   : program() {
      try {
         attach(std::move(s1));
         attach(std::move(s2));

         reload();
      }
      catch (...) {
         destroy();
         throw;
      }
   }

   program::~program() {
      destroy();
   }

   void program::reload() {
      // TODO: ensure same attrib locations with 'glBindAttribLocation(id_, u.idx, u.name.c_str())'
      uniforms_.clear();

      glLinkProgram(id_);

      use();

      GLint uniforms;
      glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &uniforms);

      for (auto idx = 0; idx < uniforms; idx++) {
         int size;
         unsigned int type;
         char name[513];

         glGetActiveUniform(id_, idx, 512, nullptr, &size, &type, name);
         int location = glGetUniformLocation(id_, name);

         assert(location == idx);

         uniforms_.push_back({ name, idx, size, gl_to_uniform_type(type) });
      }
   }

   uniform & program::uniform(std::string const & name) {
      for (auto & u : uniforms_) {
         if (u.name() == name) return u;
      }

      return uniform::INVALID;
   }

   uniform & program::uniform(unsigned int idx) {
      utils::log(utils::LOG_INFO, "WARNING: idx may be unstable for now when program reloaded!\n");

      if (idx >= uniforms_.size()) return uniform::INVALID;

      return uniforms_.at(idx);
   }


   program::program(program && other) {
      *this = std::move(other);

      other.id_ = 0;
   }

   program & program::operator=(program && other) {
      std::swap(id_, other.id_);

      shaders_ = std::move(other.shaders_);
      uniforms_ = std::move(other.uniforms_);

      return *this;
   }

   void program::use() const {
      glUseProgram(id_);
   }

   void program::destroy() {
      shaders_.clear();

      glDeleteProgram(id_);
      id_ = 0;
   }

   void program::attach(shader s) {
      glAttachShader(id_, s.id());
      shaders_.push_back(std::move(s));
   }

   std::string program::compile_logs() const {
      auto type_to_string = [](shader const & s) -> std::string {
         switch (s.type()) {
         case shader::Vertex: return " --- vertex log ---\n";
         case shader::Fragment: return " --- fragment log ---\n";
         default: return " --- unrecognised shader type log ---\n";
         }
      };

      std::string logs;
      for (auto & s : shaders_) {
         auto log = s.compile_log();
         if (log.length() == 0) continue;

         logs += type_to_string(s);
         logs += log + "\n";
      }

      return logs;
   }


  /**
   * class window
   *
   */

   struct context::impl {
      impl(GLFWwindow* window) : window_(window) { }

      GLFWwindow* window_ = nullptr;
   };


   window::window(context & ctx)
      : ctx_(ctx) {
   }

   void window::set_should_close(bool close) {
      glfwSetWindowShouldClose(ctx_.impl_->window_, close ? GL_TRUE : GL_FALSE);
   }

   bool window::closing() const {
      return GL_TRUE == glfwWindowShouldClose(ctx_.impl_->window_);
   }

   void window::swap() {
      glfwSwapBuffers(ctx_.impl_->window_);
      glfwPollEvents();
   }

   window::dim_t window::window_dims() const {
      int width, height;
      glfwGetWindowSize(ctx_.impl_->window_, &width, &height);
      return{ width, height };
   }

   window::dim_t window::frame_buffer_dims() const {
      int width, height;
      glfwGetFramebufferSize(ctx_.impl_->window_, &width, &height);
      return{ width, height };
   }


  /**
   * class context
   *
   */

   namespace {
      static std::atomic<bool> initialized_ { false };

      struct window_key_callbacks_t {
         struct callback_info_t { GLFWwindow* window; context & context; context::key_callback_t callback; };

         void set(GLFWwindow* window, context & context, context::key_callback_t callback) {
            std::unique_lock<std::mutex> l(m_);

            for (auto p : callbacks_) {
               if (p.window == window) throw error("cannot set window key callback - key handler already registered for window");
            }

            callbacks_.push_back({ window, context, callback });
         }

         void invoke(GLFWwindow* window, Key key, int scancode, KeyAction action, int mods) {
            auto info = [&]() -> callback_info_t {
               std::unique_lock<std::mutex> l(m_);

               for (auto info : callbacks_) {
                  if (info.window == window) return info;
               }

               throw error("cannot get window key callback - no key callback registered for window");
            }();

            info.callback(info.context, key, scancode, action, mods);
         }

         std::mutex m_;
         std::vector<callback_info_t> callbacks_;
      };

      window_key_callbacks_t window_key_callbacks_;

      void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
         window_key_callbacks_.invoke(
            window, 
            gl::from_glfw_key(key), 
            scancode, 
            gl::from_glfw_key_action(action), 
            gl::from_glfw_key_mods(mods));
      }
   }

   context::context(key_callback_t key_handler)
   : win_(*this), impl_(nullptr)
   {
      if (!initialized_) { throw error("runtime not initialised - call gl::init() first"); }

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
      if (!window) {
         throw error("could not create GLFW window or context");
      }

      glfwMakeContextCurrent(window);

      window_key_callbacks_.set(window, *this, key_handler);
      glfwSetKeyCallback(window, key_callback);

      impl_.reset(new impl{ window });
   }

   context::~context() {
      destroy();
   }

   std::string context::info() const {
      std::string buf;
      buf += "GLFW version        : "; buf += glfwGetVersionString();
      buf += "\nGL_VERSION          : "; buf += (const char *)glGetString(GL_VERSION);
      buf += "\nGL_VENDOR           : "; buf += (const char *)glGetString(GL_VENDOR);
      buf += "\nGL_RENDERER         : "; buf += (const char *)glGetString(GL_RENDERER);
#ifdef GL_SHADING_LANGUAGE_VERSION
      buf += "\nGL_SHADING_LANGUAGE_VERSION : "; buf += (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
#endif

      return buf;
   }

   void context::destroy() {
      if (impl_ && impl_->window_) glfwDestroyWindow(impl_->window_);
   }

   /**
    * free functions
    */

   void init() {
      auto throwing_error_callback = [](int error, const char* description) {
         auto errstr = [=] {
            switch (error) {
            case GLFW_NOT_INITIALIZED: return "GLFW_NOT_INITIALIZED";
            case GLFW_NO_CURRENT_CONTEXT: return "GLFW_NO_CURRENT_CONTEXT";
            case GLFW_INVALID_ENUM: return "GLFW_INVALID_ENUM";
            case GLFW_INVALID_VALUE: return "GLFW_INVALID_VALUE";
            case GLFW_OUT_OF_MEMORY: return "GLFW_OUT_OF_MEMORY";
            case GLFW_API_UNAVAILABLE: return "GLFW_API_UNAVAILABLE";
            case GLFW_VERSION_UNAVAILABLE: return "GLFW_VERSION_UNAVAILABLE";
            case GLFW_PLATFORM_ERROR: return "GLFW_PLATFORM_ERROR";
            case GLFW_FORMAT_UNAVAILABLE: return "GLFW_FORMAT_UNAVAILABLE";
            default: return "unrecognised error";
            }
         }();
         auto message = std::string("could not initialize GLFW - ") + description + " (" + errstr + ")";
         throw gl::error(message);
      };

      glfwSetErrorCallback(throwing_error_callback);

      if (!glfwInit())
         throw error("could not initialize GLFW");

      initialized_ = true;
   }

   void shutdown() {
      glfwTerminate();

      initialized_ = false;
   }

   double get_time() {
      return glfwGetTime();
   }
}