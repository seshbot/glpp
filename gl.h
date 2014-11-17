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

   class array_t {
   public:
      enum class AttribDataType { Byte, UByte, Short, UShort, Int, UInt, Float, Double };
      template <typename T> static AttribDataType type_of();

      template <typename T, unsigned N>
      array_t(const T(&data)[N]) : array_t((void*)data, type_of<T>(), N, sizeof(T)) {}

      array_t(void* data, AttribDataType data_type, unsigned count, std::size_t elem_size);

      void * const data() const { return state_->data_; }
      AttribDataType data_type() const { return state_->type_; }
      unsigned count() const { return state_->count_; }
      std::size_t elem_size() const { return state_->elem_size_; }
      std::size_t byte_size() const { return state_->elem_size_ * state_->count_; }

      friend bool operator==(array_t const & lhs, array_t const & rhs) { return lhs.data() == rhs.data(); }
      friend bool operator!=(array_t const & lhs, array_t const & rhs) { return !(lhs == rhs); }

   private:
      struct state {
         state(void* d, AttribDataType t, unsigned c, std::size_t sz) : data_(d), type_(t), count_(c), elem_size_(sz) { }
         void * const data_; AttribDataType type_; unsigned count_; std::size_t elem_size_;
      };

      std::shared_ptr<state const> state_;
   };

   class buffer_t {
   public:
      enum Type { Vertex, Index };
      buffer_t();

      id_t id() const { return state_->id_; }
      void use(Type type) const;

      friend bool operator==(buffer_t const & lhs, buffer_t const & rhs) { return lhs.id() == rhs.id(); }
      friend bool operator!=(buffer_t const & lhs, buffer_t const & rhs) { return !(lhs == rhs); }

   private:
      struct state {
         state();
         ~state();

         id_t id_;
      };

      std::shared_ptr<state const> state_;
   };


   struct attrib_data {
      attrib_data(attrib attrib, unsigned count, unsigned stride_bytes = 0, unsigned offset_bytes = 0)
         : attrib(std::move(attrib)), count(count), stride_bytes(stride_bytes), offset_bytes(offset_bytes) {}

      // estimate block size (stride) based on attrib specifications
      unsigned calc_stride_bytes() const;

      const attrib attrib;
      const unsigned count;
      const unsigned stride_bytes;
      const unsigned offset_bytes;
   };

   struct array_pack_t {
      array_t array;
      std::vector<attrib_data> attribs;
   };

   struct buffer_pack_t {
      buffer_t buffer;
      std::vector<attrib_data> attribs;
   };

   buffer_pack_t buffer(buffer_t b, std::initializer_list<attrib_data> attribs);
   buffer_pack_t buffer(std::initializer_list<attrib_data> attribs);

   unsigned num_vertices(array_pack_t const & b);
   unsigned num_vertices(buffer_pack_t const & b);
   void use(array_pack_t const & b);
   void use(buffer_pack_t const & b);


   enum class DrawType {
      Points,
      Lines, LineLoop,
      Triangles, TriangleStrip, TriangleFan,
   };

   class draw_helper_t {
   public:
      draw_helper_t(draw_helper_t const &) = delete;
      draw_helper_t & operator=(draw_helper_t const &) = delete;

      draw_helper_t & with(array_t array, std::initializer_list<attrib_data> attribs);
      //draw_helper_t & with(buffer_pack_t buffer_pack);

      //draw_helper_t & validate_attribs(bool validate = true);

      draw_helper_t & draw(DrawType type);
      draw_helper_t & draw(DrawType type, unsigned first, unsigned count);

   private:
      friend class program;
      draw_helper_t(program & prg) : prg_(prg) { }

      program & prg_;

      struct vertex_data_layout {
         unsigned num_vertices() const { return model_->num_vertices(); }
         void use() const { model_->use(); }

         vertex_data_layout(vertex_data_layout && other) : model_(std::move(other.model_)) { }

         template <typename T>
         vertex_data_layout(T impl) : model_(new model<T>(std::move(impl))) {}
         struct concept {
            virtual ~concept() {}
            virtual unsigned num_vertices() const = 0;
            virtual void use() const = 0;
         };
         template <typename T>
         struct model : public concept {
            model(T data) : data_(std::move(data)) {}
            virtual unsigned num_vertices() const { return gl::num_vertices(data_); }
            virtual void use() const { gl::use(data_); }
            T data_;
         };

         std::unique_ptr<concept> model_;
      };
      std::vector<vertex_data_layout> vertex_data_;
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

      draw_helper_t pass();

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

   private:
      friend class window;
      window win_;

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
