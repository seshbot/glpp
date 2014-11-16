#ifndef PCX_GL__H
#define PCX_GL__H

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <memory>

#include "input.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>


namespace gl {
	using id_t = unsigned int;
	using name_t = std::string;

   struct error : public std::runtime_error {
      error() : std::runtime_error("") { }
      error(std::string message) : std::runtime_error(message) { }
   };

   struct shader_compile_error : public error {
      shader_compile_error(std::string source_name, shader_compile_error const & parent);
      shader_compile_error(id_t shader_id);
      std::string const & log() const { return log_; }
   private:
      std::string log_;
   };

   class shader {
   public:
      enum Type { Vertex, Fragment };

      ~shader();

      shader(shader && other);
      shader & operator=(shader && other);

      static shader create_from_file(std::string const & filename, Type type);
      static shader create_from_source(std::string const & source, Type type);

      id_t id() const { return id_; }
      Type type() const { return type_; }
      std::string const & compile_log() const { return compile_log_; }

   private:
      shader(shader const &) = delete;
      shader & operator=(shader const &) = delete;

      shader(std::string const & source, Type type);

      id_t id_;
      Type type_;
      std::string compile_log_;
   };

   // attr types: GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, or GL_FLOAT_MAT4
   // attr { name, idx, location, size, type }

   enum ValueType {
      Unknown,
      Int, UInt, Float,
      FloatVec2, FloatVec3, FloatVec4,
      IntVec2, IntVec3, IntVec4,
      FloatMat2, FloatMat3, FloatMat4,

      // uniform only
      Bool, Byte, UByte, Short, UShort, Fixed,
      BoolVec2, BoolVec3, BoolVec4,
      Sampler2d, SamplerCube,

      // attrib only
      FloatMat2x3, FloatMat2x4, FloatMat3x2, FloatMat3x4, FloatMat4x2, FloatMat4x3,
      UIntVec2, UIntVec3, UIntVec4,
      Double, DoubleVec2, DoubleVec3, DoubleVec4,
      DoubleMat2, DoubleMat3, DoubleMat4,
      DoubleMat2x3, DoubleMat2x4,
      DoubleMat3x2, DoubleMat3x4,
      DoubleMat4x2, DoubleMat4x3,
   };

   class uniform {
   public:
      void set(int val);
      void set(float val);
      void set(glm::mat4 const & mat);
      void set(glm::mat3 const & mat);
      void set(glm::mat2 const & mat);
      void set(glm::vec4 const & vec);
      void set(glm::vec3 const & vec);
      void set(glm::vec2 const & vec);

      std::string const & name() const { return state_->name_; }
      int location() const { return state_->location_; }
      int size() const { return state_->size_; }
      ValueType type() const { return state_->type_; }

      bool is_valid() const { return state_->location_ != -1; }

   private:
      friend class program;

      uniform(std::string const & name, int location, int size, ValueType type);
      uniform(std::string const & name); // creates an INVALID uniform
      void reset(int location, int size, ValueType type);
      void reset();

      struct state {
         std::string name_;
         int location_;
         int size_;
         ValueType type_;
         bool error_; // just used to prevent repeated error reports
      };

      std::shared_ptr<state> state_;
   };

   class attrib {
   public:
      std::string const & name() const { return state_->name_; }
      int location() const { return state_->location_; }
      int size() const { return state_->size_; }
      ValueType type() const { return state_->type_; }

      bool is_valid() const { return state_->location_ != -1; }

   private:
      friend class program;

      attrib(std::string const & name, int location, int size, ValueType type);
      attrib(std::string const & name); // creates an INVALID uniform
      void reset(int location, int size, ValueType type);
      void reset();

      struct state {
         std::string name_;
         int location_;
         int size_;
         ValueType type_;
         bool error_; // just used to prevent repeated error reports
      };

      std::shared_ptr<state> state_;
   };

	class program {
   public:
      program(shader s1, shader s2);
      ~program();

      static program create_using_vars_from(program & other, shader s1, shader s2);

		program(program && other);
		program & operator=(program && other);

      void reload(shader s1, shader s2);

      gl::uniform uniform(std::string const & name);
      gl::attrib attrib(std::string const & name);

      void use() const;

      void destroy();
      id_t id() const { return id_; }
      void attach(shader s);

      std::string compile_logs() const;

   private:
      program();
      program(program & other, shader s);
      program(program & other, shader s1, shader s2);

      program(program const &) = delete;
      program & operator=(program const &) = delete;

      void reload();

      id_t id_;
      std::vector<shader> shaders_;
      std::vector<gl::uniform> uniforms_;
      std::vector<gl::attrib> attribs_;

      int version_ = 1;
   };

   class context;

   class window {
   public:
      void set_should_close(bool close = true);
      bool closing() const;

      void swap();

      struct dim_t { int x; int y; };
      dim_t window_dims() const;
      dim_t frame_buffer_dims() const;

   private:
      friend class context;
      window(context & ctx);

      window(window const &) = delete;
      window & operator=(window const &) = delete;
      window(window &&) = delete;
      window & operator=(window &&) = delete;

      context & ctx_;
   };

   class array {
   public:
      enum Type { Byte, UByte, Short, UShort, Int, UInt, Float, Double };

      template <typename T> static Type type_of();

      array(void* data, Type type, unsigned count, std::size_t elem_size);

      void * const data() const { return state_->data_; }
      Type type() const { return state_->type_; }
      unsigned count() const { return state_->count_; }
      std::size_t elem_size() const { return state_->elem_size_; }
      std::size_t byte_size() const { return state_->elem_size_ * state_->count_; }

      struct state {
         state(void* d, Type t, unsigned c, std::size_t sz) : data_(d), type_(t), count_(c), elem_size_(sz) { }
         void * const data_; Type type_; unsigned count_; std::size_t elem_size_;
      };

      std::shared_ptr<state const> state_;
   };


   class buffer {
   };

   enum class DrawType {
      Points,
      Lines, LineLoop,
      Triangles, TriangleStrip, TriangleFan,
   };

   class draw_helper_t {
   public:
      draw_helper_t(draw_helper_t const &) = delete;
      draw_helper_t & operator=(draw_helper_t const &) = delete;

      draw_helper_t & with_attrib(attrib a, array data, unsigned count, unsigned stride_bytes);
      draw_helper_t & with_attrib(attrib a, array data, unsigned offset_bytes, unsigned count, unsigned stride_bytes);
      draw_helper_t & validate_attribs(bool validate = true);

      draw_helper_t & draw(DrawType type);
      draw_helper_t & draw(DrawType type, unsigned first, unsigned count);

   private:
      friend class context;
      draw_helper_t(context & ctx) : ctx_(ctx) { }

      struct attrib_binding {
         attrib attrib; array array; unsigned offset_bytes; unsigned count; unsigned stride;
         unsigned block_size() const;
      };
      context & ctx_;
      std::vector<attrib_binding const> attrib_bindings_;
   };

	class context {
   public:
      using key_callback_t = std::function < void(context &, Key, int, KeyAction, int) >; // key, scancode, action, mods

      context(key_callback_t key_handler);
      context(context const &) = delete;
      context & operator=(context const &) = delete;
      ~context();

      std::string info() const;

      void destroy(); // optional, will be invoked by dtor

      window const & win() const { return win_; }
      window & win() { return win_; }

      template <typename T, unsigned N>
      array new_array(const T(&data)[N]) { return new_array_impl((void*)data, array::type_of<T>(), N, sizeof(T)); }

      draw_helper_t pass();

   private:
      friend class window;
      window win_;

      array new_array_impl(void* data, array::Type type, unsigned count, std::size_t elem_size);

      struct impl;
      std::unique_ptr<impl> impl_;
	};

   void init();
   void shutdown();

   double get_time();

   void glUniform(int location, glm::mat4 const & mat);
   void glUniform(int location, glm::mat3 const & mat);
   void glUniform(int location, glm::mat2 const & mat);
   void glUniform(int location, glm::vec4 const & vec);
   void glUniform(int location, glm::vec3 const & vec);
   void glUniform(int location, glm::vec2 const & vec);
   void glUniform(int location, float f);
   void glUniform(int location, int i);
   //void glUniform(GLint location, GLuint i);

}


#endif
