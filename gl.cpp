#include "gl.h"

#ifdef WIN32
#  include <GLES2/gl2.h>

#  define USE_OPENGL_ES_2
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <utility>
#include <atomic>
#include <mutex>
#include <cassert>

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

   std::string to_string(gl::uniform::Type type) {
      switch (type) {
      case gl::uniform::Type::Byte: return "Byte";
      case gl::uniform::Type::UByte: return "UByte";
      case gl::uniform::Type::Short: return "Short";
      case gl::uniform::Type::UShort: return "UShort";
      case gl::uniform::Type::Int: return "Int";
      case gl::uniform::Type::UInt: return "UInt";
      case gl::uniform::Type::Float: return "Float";
      case gl::uniform::Type::Fixed: return "Fixed";

      case gl::uniform::Type::FloatVec2: return "FloatVec2";
      case gl::uniform::Type::FloatVec3: return "FloatVec3";
      case gl::uniform::Type::FloatVec4: return "FloatVec4";
      case gl::uniform::Type::IntVec2: return "IntVec2";
      case gl::uniform::Type::IntVec3: return "IntVec3";
      case gl::uniform::Type::IntVec4: return "IntVec4";
      case gl::uniform::Type::Bool: return "Bool";
      case gl::uniform::Type::BoolVec2: return "BoolVec2";
      case gl::uniform::Type::BoolVec3: return "BoolVec3";
      case gl::uniform::Type::BoolVec4: return "BoolVec4";
      case gl::uniform::Type::FloatMat2: return "FloatMat2";
      case gl::uniform::Type::FloatMat3: return "FloatMat3";
      case gl::uniform::Type::FloatMat4: return "FloatMat4";
      case gl::uniform::Type::Sampler2d: return "Sampler2d";
      case gl::uniform::Type::SamplerCube: return "SamplerCube";

      case gl::uniform::Type::Unknown: return "Unknown";

      default: return "UNRECOGNISED TYPE";
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

   uniform::uniform(std::string const & name, int location, int size, Type type)
      : state_{ new state{ name, location, size, type, false } } {
   }

   uniform::uniform(std::string const & name)
      : uniform{ name, -1, 1, uniform::Type::Unknown } {
   }

   void uniform::reset(int location, int size, Type type) {
      state_->location_ = location;
      state_->size_ = size;
      state_->type_ = type;
      state_->error_ = false;
   }

   void uniform::reset() {
      state_->location_ = -1;
      state_->error_ = false;
   }

   namespace {
      template <typename T>
      uniform::Type uniform_type();
      template <> uniform::Type uniform_type<int>() { return uniform::Type::Int; }
      template <> uniform::Type uniform_type<float>() { return uniform::Type::Float; }
      template <> uniform::Type uniform_type<glm::vec2>() { return uniform::Type::FloatVec2; }
      template <> uniform::Type uniform_type<glm::vec3>() { return uniform::Type::FloatVec3; }
      template <> uniform::Type uniform_type<glm::vec4>() { return uniform::Type::FloatVec4; }
      template <> uniform::Type uniform_type<glm::mat2>() { return uniform::Type::FloatMat2; }
      template <> uniform::Type uniform_type<glm::mat3>() { return uniform::Type::FloatMat3; }
      template <> uniform::Type uniform_type<glm::mat4>() { return uniform::Type::FloatMat4; }

      template <typename T>
      bool set_uniform(uniform & u, T val, bool report_errors) {
         if (!u.is_valid()) {
            if (report_errors) {
               utils::log(utils::LOG_WARN, "cannot set uniform - either program does not contain '%s' or it has been invalidated\n",
                  u.name().c_str());
            }
            return false;
         }

         if (uniform_type<T>() != u.type()) {
            if (report_errors) {
               utils::log(utils::LOG_WARN, "cannot set uniform - shader uniform '%s' is type %s, app provided %s\n",
                  u.name().c_str(),
                  ::to_string(u.type()).c_str(),
                  ::to_string(uniform_type<T>()).c_str());
            }
            return false;
         }

         gl::glUniform(u.location(), val);
         return true;
      }
   }

   void uniform::set(int val) {
      if (!set_uniform(*this, val, !state_->error_))
         state_->error_ = true;
   }

   void uniform::set(float val) {
      if (!set_uniform(*this, val, !state_->error_))
         state_->error_ = true;
   }

   void uniform::set(glm::vec2 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::vec3 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::vec4 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::mat2 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::mat3 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::mat4 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
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
      decltype(uniforms_) old_uniforms;
      old_uniforms.swap(uniforms_);

      auto find_uniform_with_name = [&](std::string const & name) {
         for (auto it = old_uniforms.begin(); it != old_uniforms.end(); ++it) {
            if (it->name() == name) return it;
         }

         return old_uniforms.end();
      };

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

         auto it = find_uniform_with_name(name);
         if (it == old_uniforms.end()) {
            uniforms_.push_back({ name, idx, size, gl_to_uniform_type(type) });
         }
         else {
            it->reset(idx, size, gl_to_uniform_type(type));
            uniforms_.push_back(*it);
            old_uniforms.erase(it);
         }
      }

      // make sure we keep handles to the old uniforms so existing handles dont just break
      for (auto & u : old_uniforms) {
         u.reset();
         uniforms_.push_back(u);
      }
   }

   uniform program::uniform(std::string const & name) {
      for (auto & u : uniforms_) {
         if (u.name() == name) return u;
      }

      // not found, create and start tracking an invalid uniform
      auto new_uniform = gl::uniform{ name };
      uniforms_.push_back(new_uniform);
      return new_uniform;
   }

   uniform program::uniform(unsigned int idx) {
      for (auto & u : uniforms_) {
         if (u.location() == idx) return u;
      }

      throw error(std::string("cannot retrieve uniform - no uniform has index ") + std::to_string(idx));
   }


   program::program(program && other) {
      *this = std::move(other);

      other.id_ = 0;
   }

   program & program::operator=(program && other) {
      std::swap(id_, other.id_);

      std::swap(shaders_, other.shaders_);
      std::swap(uniforms_, other.uniforms_);

      return *this;
   }

   void program::use() const {
      glUseProgram(id_);
   }

   void program::destroy() {
      shaders_.clear();
      for (auto & u : uniforms_) { u.reset(); }

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

   void glUniform(int location, glm::mat4 const & mat) { glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)); }
   void glUniform(int location, glm::mat3 const & mat) { glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat)); }
   void glUniform(int location, glm::mat2 const & mat) { glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat)); }
   void glUniform(int location, glm::vec4 const & vec) { glUniform4f(location, vec.x, vec.y, vec.z, vec.w); }
   void glUniform(int location, glm::vec3 const & vec) { glUniform3f(location, vec.x, vec.y, vec.z); }
   void glUniform(int location, glm::vec2 const & vec) { glUniform2f(location, vec.x, vec.y); }
   void glUniform(int location, float f) { glUniform1f(location, f); }
   void glUniform(int location, int i) { glUniform1i(location, i); }
   //void glUniform(GLint location, GLuint i) { glUniform1ui(location, i); }
}