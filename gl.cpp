#include "gl.h"

#ifdef WIN32
#  include <GLES2/gl2.h>

#  define USE_OPENGL_ES_2
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <SOIL2/SOIL2.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <utility>
#include <atomic>
#include <mutex>
#include <cassert>

#include "utils.h"


void checkOpenGLError(const char* function, const char* file, int line) {
   GLenum err = glGetError(); if (err == GL_NO_ERROR) return;
   utils::log(utils::LOG_ERROR, "OpenGL error '%s' (0x%04x) called from %s in file %s line %d\n", gl::openGlErrorString(err), err, function, file, line);
}

void checkOpenGLError(const char* stmt, const char* function, const char* file, int line) {
   GLenum err = glGetError(); if (err == GL_NO_ERROR) return;
   utils::log(utils::LOG_ERROR, "OpenGL error '%s' (0x%04x) at %s called from %s in file %s line %d\n", gl::openGlErrorString(err), err, stmt, function, file, line);
}

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

   gl::ValueType gl_to_value_type(GLuint type) {
      switch (type) {
      case GL_BYTE: return gl::ValueType::Byte;
      case GL_UNSIGNED_BYTE: return gl::ValueType::UByte;
      case GL_SHORT: return gl::ValueType::Short;
      case GL_UNSIGNED_SHORT: return gl::ValueType::UShort;
      case GL_INT: return gl::ValueType::Int;
      case GL_UNSIGNED_INT: return gl::ValueType::UInt;
      case GL_FLOAT: return gl::ValueType::Float;
      case GL_FIXED: return gl::ValueType::Fixed;

      case GL_FLOAT_VEC2: return gl::ValueType::FloatVec2;
      case GL_FLOAT_VEC3: return gl::ValueType::FloatVec3;
      case GL_FLOAT_VEC4: return gl::ValueType::FloatVec4;
      case GL_INT_VEC2: return gl::ValueType::IntVec2;
      case GL_INT_VEC3: return gl::ValueType::IntVec3;
      case GL_INT_VEC4: return gl::ValueType::IntVec4;
      case GL_BOOL: return gl::ValueType::Bool;
      case GL_BOOL_VEC2: return gl::ValueType::BoolVec2;
      case GL_BOOL_VEC3: return gl::ValueType::BoolVec3;
      case GL_BOOL_VEC4: return gl::ValueType::BoolVec4;
      case GL_FLOAT_MAT2: return gl::ValueType::FloatMat2;
      case GL_FLOAT_MAT3: return gl::ValueType::FloatMat3;
      case GL_FLOAT_MAT4: return gl::ValueType::FloatMat4;
      case GL_SAMPLER_2D: return gl::ValueType::Sampler2d;
      case GL_SAMPLER_CUBE: return gl::ValueType::SamplerCube;
      default: return gl::ValueType::Unknown;
      }
   }

   std::string to_string(gl::ValueType type) {
      switch (type) {
      case gl::ValueType::Unknown: return "Unknown";
      case gl::ValueType::Int: return "Int";
      case gl::ValueType::UInt: return "UInt";
      case gl::ValueType::Float: return "Float";
      case gl::ValueType::FloatVec2: return "FloatVec2";
      case gl::ValueType::FloatVec3: return "FloatVec3";
      case gl::ValueType::FloatVec4: return "FloatVec4";
      case gl::ValueType::IntVec2: return "IntVec2";
      case gl::ValueType::IntVec3: return "IntVec3";
      case gl::ValueType::IntVec4: return "IntVec4";
      case gl::ValueType::FloatMat2: return "FloatMat2";
      case gl::ValueType::FloatMat3: return "FloatMat3";
      case gl::ValueType::FloatMat4: return "FloatMat4";

      case gl::ValueType::Bool: return "Bool";
      case gl::ValueType::Byte: return "Byte";
      case gl::ValueType::UByte: return "UByte";
      case gl::ValueType::Short: return "Short";
      case gl::ValueType::UShort: return "UShort";
      case gl::ValueType::Fixed: return "Fixed";
      case gl::ValueType::BoolVec2: return "BoolVec2";
      case gl::ValueType::BoolVec3: return "BoolVec3";
      case gl::ValueType::BoolVec4: return "BoolVec4";
      case gl::ValueType::Sampler2d: return "Sampler2d";
      case gl::ValueType::SamplerCube: return "SamplerCube";

      case gl::ValueType::FloatMat2x3: return "FloatMat2x3";
      case gl::ValueType::FloatMat2x4: return "FloatMat2x4";
      case gl::ValueType::FloatMat3x2: return "FloatMat3x2";
      case gl::ValueType::FloatMat3x4: return "FloatMat3x4";
      case gl::ValueType::FloatMat4x2: return "FloatMat4x2";
      case gl::ValueType::FloatMat4x3: return "FloatMat4x3";
      case gl::ValueType::UIntVec2: return "UIntVec2";
      case gl::ValueType::UIntVec3: return "UIntVec3";
      case gl::ValueType::UIntVec4: return "UIntVec4";
      case gl::ValueType::Double: return "Double";
      case gl::ValueType::DoubleVec2: return "DoubleVec2";
      case gl::ValueType::DoubleVec3: return "DoubleVec3";
      case gl::ValueType::DoubleVec4: return "DoubleVec4";
      case gl::ValueType::DoubleMat2: return "DoubleMat2";
      case gl::ValueType::DoubleMat3: return "DoubleMat3";
      case gl::ValueType::DoubleMat4: return "DoubleMat4";
      case gl::ValueType::DoubleMat2x3: return "DoubleMat2x3";
      case gl::ValueType::DoubleMat2x4: return "DoubleMat2x4";
      case gl::ValueType::DoubleMat3x2: return "DoubleMat3x2";
      case gl::ValueType::DoubleMat3x4: return "DoubleMat3x4";
      case gl::ValueType::DoubleMat4x2: return "DoubleMat4x2";
      case gl::ValueType::DoubleMat4x3: return "DoubleMat4x3";

      default: return "UNRECOGNISED TYPE";
      }
   }

   
   std::size_t attrib_atomic_val_bytes(gl::ValueType type) {
      switch (type) {
      case gl::ValueType::Byte:
      case gl::ValueType::UByte:
         return 1;

      case gl::ValueType::Short:
      case gl::ValueType::UShort:
         return 2;

      case gl::ValueType::Int:
      case gl::ValueType::UInt:
      case gl::ValueType::Float:
      case gl::ValueType::FloatVec2:
      case gl::ValueType::FloatVec3:
      case gl::ValueType::FloatVec4:
      case gl::ValueType::IntVec2:
      case gl::ValueType::IntVec3:
      case gl::ValueType::IntVec4:
      case gl::ValueType::FloatMat2:
      case gl::ValueType::FloatMat3:
      case gl::ValueType::FloatMat4:

      case gl::ValueType::FloatMat2x3:
      case gl::ValueType::FloatMat2x4:
      case gl::ValueType::FloatMat3x2:
      case gl::ValueType::FloatMat3x4:
      case gl::ValueType::FloatMat4x2:
      case gl::ValueType::FloatMat4x3:
      case gl::ValueType::UIntVec2:
      case gl::ValueType::UIntVec3:
      case gl::ValueType::UIntVec4:
         return 4;

      case gl::ValueType::Double:
      case gl::ValueType::DoubleVec2:
      case gl::ValueType::DoubleVec3:
      case gl::ValueType::DoubleVec4:
      case gl::ValueType::DoubleMat2:
      case gl::ValueType::DoubleMat3:
      case gl::ValueType::DoubleMat4:
      case gl::ValueType::DoubleMat2x3:
      case gl::ValueType::DoubleMat2x4:
      case gl::ValueType::DoubleMat3x2:
      case gl::ValueType::DoubleMat3x4:
      case gl::ValueType::DoubleMat4x2:
      case gl::ValueType::DoubleMat4x3:
         return 8;

      default: throw gl::error("cannot calculate primitive size of invalid attribute type");
      }
   }


   GLuint attrib_atomic_gl_type(gl::ValueType type) {
      switch (type) {
      case gl::ValueType::Byte: return GL_BYTE;
      case gl::ValueType::UByte: return GL_UNSIGNED_BYTE;
      case gl::ValueType::Short: return GL_SHORT;
      case gl::ValueType::UShort: return GL_UNSIGNED_SHORT;
      case gl::ValueType::Int: return GL_INT;
      case gl::ValueType::UInt: return GL_UNSIGNED_INT;
      case gl::ValueType::Float: return GL_FLOAT;
      case gl::ValueType::Double: return GL_DOUBLE;

      case gl::ValueType::FloatVec2: return GL_FLOAT;
      case gl::ValueType::FloatVec3: return GL_FLOAT;
      case gl::ValueType::FloatVec4: return GL_FLOAT;
      case gl::ValueType::IntVec2: return GL_INT;
      case gl::ValueType::IntVec3: return GL_INT;
      case gl::ValueType::IntVec4: return GL_INT;

      case gl::ValueType::FloatMat2: return GL_FLOAT;
      case gl::ValueType::FloatMat3: return GL_FLOAT;
      case gl::ValueType::FloatMat4: return GL_FLOAT;

      default: throw gl::error("invalid data value type when drawing");
      }
   }
}

namespace gl
{
   /**
    * utils
    */

   const char * openGlErrorString(GLenum err) {
      switch (err) {
      case 0x0500: return "GL_INVALID_ENUM";
      case 0x0501: return "GL_INVALID_VALUE";
      case 0x0502: return "GL_INVALID_OPERATION";
      case 0x0503: return "GL_STACK_OVERFLOW";
      case 0x0504: return "GL_STACK_UNDERFLOW";
      case 0x0505: return "GL_OUT_OF_MEMORY";
      case 0x0506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
      case 0x8031: return "GL_TABLE_TOO_LARGE1";
      default: return "unrecognised";
      }
   }

   void checkOpenGLError(const char* function, const char* file, int line)
   {
      GLenum err = glGetError(); if (err == GL_NO_ERROR) return;

      utils::log(utils::LOG_ERROR, "OpenGL error '%s' (0x%04x) called from %s in file %s line %d\n", openGlErrorString(err), err, function, file, line);
   }

   void checkOpenGLError(const char* stmt, const char* function, const char* file, int line)
   {
      GLenum err = glGetError(); if (err == GL_NO_ERROR) return;

      utils::log(utils::LOG_ERROR, "OpenGL error '%s' (0x%04x) at %s called from %s in file %s line %d\n", openGlErrorString(err), err, stmt, function, file, line);
   }


  /**
   * class texture
   *
   */

   texture_t::texture_t(std::string const & filename, int tex_unit)
      : tex_id_(SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS))
      , tex_unit_(tex_unit) {
      if (!tex_id_) {
         throw gl::error(std::string("Could not load texture from '") + filename + "': " + SOIL_last_result());
      }

      GL_VERIFY(glActiveTexture(GL_TEXTURE0 + tex_unit_));
      GL_VERIFY(glBindTexture(GL_TEXTURE_2D, tex_id_));

      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // GL_NEAREST
      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // GL_CLAMP_TO_EDGE
      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
   }


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

   uniform::uniform(std::string const & name, int location, int size, ValueType type)
      : state_{ new state{ name, location, size, type, false } } {
   }

   uniform::uniform(std::string const & name)
      : uniform{ name, -1, 1, ValueType::Unknown } {
   }

   void uniform::reset(int location, int size, ValueType type) {
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
      ValueType uniform_type();
      template <> ValueType uniform_type<int>() { return ValueType::Int; }
      template <> ValueType uniform_type<float>() { return ValueType::Float; }
      template <> ValueType uniform_type<glm::vec2>() { return ValueType::FloatVec2; }
      template <> ValueType uniform_type<glm::vec3>() { return ValueType::FloatVec3; }
      template <> ValueType uniform_type<glm::vec4>() { return ValueType::FloatVec4; }
      template <> ValueType uniform_type<glm::mat2>() { return ValueType::FloatMat2; }
      template <> ValueType uniform_type<glm::mat3>() { return ValueType::FloatMat3; }
      template <> ValueType uniform_type<glm::mat4>() { return ValueType::FloatMat4; }
      template <> ValueType uniform_type<texture_t>() { return ValueType::Sampler2d; }

      template <typename T>
      bool set_uniform(uniform & u, T const & val, bool report_errors) {
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

         gl::set_uniform(u.location(), val);
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

   void uniform::set(texture_t const & tex) {
      auto success = set_uniform(*this, tex, !state_->error_);
      if (!success) state_->error_ = true;
   }


  /**
   * class attrib
   *
   */

   attrib::attrib(std::string const & name, int location, int size, ValueType type)
      : state_{ new state{ name, location, size, type, false } } {
   }

   attrib::attrib(std::string const & name)
      : attrib{ name, -1, 1, ValueType::Unknown } {
   }

   void attrib::reset(int location, int size, ValueType type) {
      state_->location_ = location;
      state_->size_ = size;
      state_->type_ = type;
      state_->error_ = false;
   }

   void attrib::reset() {
      state_->location_ = -1;
      state_->error_ = false;
   }


  /**
   * buffer management
   *
   */

   buffer_t::idx_array_t::idx_array_t(void* data, unsigned count, ValueType data_type, std::size_t byte_size)
      : data(data), count(count), data_type(data_type), byte_size(byte_size) {
      assert((data_type == ValueType::UInt || data_type == ValueType::UShort || data_type == ValueType::UByte) && "invalid index buffer type");
   }


   buffer_t::state::state(void* vertex_data, std::size_t vertex_data_byte_size)
      : vertex_buffer_size_(vertex_data_byte_size) {
      glGenBuffers(1, &vertex_id_);
      glBindBuffer(GL_ARRAY_BUFFER, vertex_id_);
      glBufferData(GL_ARRAY_BUFFER, vertex_data_byte_size, vertex_data, GL_STATIC_DRAW);
   }

   buffer_t::state::state(void* vertex_data, std::size_t vertex_byte_size, void* index_data, unsigned index_count, std::size_t index_byte_size, ValueType index_data_type)
   : state(vertex_data, vertex_byte_size) {
      index_data_type_ = index_data_type;
      index_count_ = index_count;
      glGenBuffers(1, &index_id_);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_byte_size, index_data, GL_STATIC_DRAW);
   }

   buffer_t::state::~state() {
      if (0 != vertex_id_) glDeleteBuffers(1, &vertex_id_);
      if (0 != index_id_) glDeleteBuffers(1, &index_id_);
   }


   buffer_t::buffer_t(static_array_t vertex_data)
      : state_(new state(vertex_data.data(), vertex_data.size())) {
   }

   buffer_t::buffer_t(static_array_t vertex_data, idx_array_t index_data)
      : state_(new state(vertex_data.data(), vertex_data.size(), index_data.data, index_data.count, index_data.byte_size, index_data.data_type)) {
   }

   void buffer_t::use() const {
      assert(0 != state_->vertex_id_ && "vertex buffer not initialised");
      glBindBuffer(GL_ARRAY_BUFFER, state_->vertex_id_);
      if (0 != state_->index_id_) {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state_->index_id_);
      }
   }


   unsigned attrib_data::calc_stride_bytes() const{
      return stride_bytes != 0
         ? stride_bytes
         : attrib_atomic_val_bytes(attrib.type()) * count;
   }

   unsigned num_vertices(array_spec_t const & packed) {
      auto block_count = [&](attrib_data const & b) {
         auto stride_bytes = b.calc_stride_bytes();
         assert(packed.array.size() % stride_bytes == 0);

         return packed.array.size() / stride_bytes;
      };

      auto min_block_count = block_count(packed.attribs.front());

      for (auto it = packed.attribs.begin() + 1; it != packed.attribs.end(); ++it) {
         auto c = block_count(*it);
         if (c < min_block_count) min_block_count = c;
      }

      assert(min_block_count > 0);

      return min_block_count;
   }

   void use(array_spec_t const & packed) {
      for (auto & attrib_data : packed.attribs) {
		 auto gl_type = attrib_atomic_gl_type(attrib_data.attrib.type());
         int8_t * data = reinterpret_cast<int8_t*>(packed.array.data()) + attrib_data.offset_bytes;

         GL_VERIFY(glVertexAttribPointer(attrib_data.attrib.location(), attrib_data.count, gl_type, false, attrib_data.stride_bytes, data));
         GL_VERIFY(glEnableVertexAttribArray(attrib_data.attrib.location()));
      }
   }


   unsigned num_vertices(buffer_spec_t const & packed) {
      auto block_count = [&](attrib_data const & b) {
         auto stride_bytes = b.calc_stride_bytes();
         assert(packed.buffer.vertex_buffer_size() % stride_bytes == 0);

         return packed.buffer.vertex_buffer_size() / stride_bytes;
      };

      auto min_block_count = block_count(packed.attribs.front());

      for (auto it = packed.attribs.begin() + 1; it != packed.attribs.end(); ++it) {
         auto c = block_count(*it);
         if (c < min_block_count) min_block_count = c;
      }

      assert(min_block_count > 0);

      return min_block_count;
   }

   void use(buffer_spec_t const & packed) {
      packed.buffer.use();
      for (auto & attrib_data : packed.attribs) {
         auto gl_type = attrib_atomic_gl_type(attrib_data.attrib.type());

         GL_VERIFY(glVertexAttribPointer(
            attrib_data.attrib.location(), attrib_data.count, gl_type,
            false, attrib_data.stride_bytes, reinterpret_cast<void*>(attrib_data.offset_bytes)));

         GL_VERIFY(glEnableVertexAttribArray(attrib_data.attrib.location()));
      }
   }

   void draw(array_spec_t const & b, DrawMode mode) {
      return draw(b, mode, 0, num_vertices(b));
   }

   void draw(buffer_spec_t const & b, DrawMode mode) {
      unsigned count = b.buffer.has_index_data() ? b.buffer.index_count() : num_vertices(b);
      return draw(b, mode, 0, count);
   }

   namespace {
      GLenum gl_draw_mode(DrawMode mode) {
         switch (mode) {
         case DrawMode::Points: return GL_POINTS;
         case DrawMode::Lines: return GL_LINES;
         case DrawMode::LineLoop: return GL_LINE_LOOP;
         case DrawMode::Triangles: return GL_TRIANGLES;
         case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
         case DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
         default: throw error("unrecognised draw type when drawing");
         }
      }
   }

   void draw(array_spec_t const & b, DrawMode mode, unsigned first, unsigned count) {
      use(b);

      GL_VERIFY(glDrawArrays(gl_draw_mode(mode), first, count));
   }

   void draw(buffer_spec_t const & b, DrawMode mode, unsigned first, unsigned count) {
      use(b);

      if (b.buffer.has_index_data()) {
         assert(count <= b.buffer.index_count() && "index count greater than number of indices in buffer while drawing indexed buffer");
         assert(first <= b.buffer.index_count() && "first index greater than total number of indices in buffer while drawing indexed buffer");
         assert(first + count <= b.buffer.index_count() && "end index greater than total number of indices in buffer while drawing indexed buffer");

         auto gl_mode = gl_draw_mode(mode);
         auto gl_type = attrib_atomic_gl_type(b.buffer.index_data_type());
         auto idx_count = count;
         auto idx_start = reinterpret_cast<GLvoid*>(first * attrib_atomic_val_bytes(b.buffer.index_data_type()));

         GL_VERIFY(glDrawElements(gl_mode, idx_count, gl_type, idx_start));
      }
      else {
         GL_VERIFY(glDrawArrays(gl_draw_mode(mode), first, count));
      }
   }



   array_spec_builder_t::array_spec_builder_t(static_array_t array)
      : spec_prototype_(array_spec_t{std::move(array), {}}) {
   }

   array_spec_builder_t::array_spec_builder_t(array_spec_builder_t && other)
      : pos_bytes_(other.pos_bytes_)
      , spec_prototype_(std::move(other.spec_prototype_)) {
   }

   array_spec_builder_t & array_spec_builder_t::operator=(array_spec_builder_t && other) {
      std::swap(spec_prototype_, other.spec_prototype_);
      std::swap(pos_bytes_, other.pos_bytes_);

      return *this;
   }

   array_spec_builder_t & array_spec_builder_t::add(attrib attrib, unsigned count) {
      auto slice_size = count * attrib_atomic_val_bytes(attrib.type());
      spec_prototype_.attribs.push_back({ std::move(attrib), count, 0, pos_bytes_ });
      pos_bytes_ += slice_size;

      return *this;
   }

   array_spec_builder_t & array_spec_builder_t::skip(unsigned bytes) {
      pos_bytes_ += bytes;

      return *this;
   }

   array_spec_t array_spec_builder_t::build() const {
      decltype(spec_prototype_.attribs) attribs;

      // prototype doesnt have correct stride
      for (auto & d : spec_prototype_.attribs) {
         attribs.push_back({ d.attrib, d.count, pos_bytes_, d.offset_bytes});
      }

      return{ spec_prototype_.array, std::move(attribs) };
   }

   array_spec_builder_t describe_static(static_array_t array) {
      return{ array };
   }




   buffer_spec_builder_t::buffer_spec_builder_t(buffer_t buffer)
      : spec_prototype_(buffer_spec_t{ std::move(buffer), {} }) {
   }

   buffer_spec_builder_t::buffer_spec_builder_t(buffer_spec_builder_t && other)
      : pos_bytes_(other.pos_bytes_)
      , spec_prototype_(std::move(other.spec_prototype_)) {
   }

   buffer_spec_builder_t & buffer_spec_builder_t::operator=(buffer_spec_builder_t && other) {
      std::swap(spec_prototype_, other.spec_prototype_);
      std::swap(pos_bytes_, other.pos_bytes_);

      return *this;
   }

   buffer_spec_builder_t & buffer_spec_builder_t::add(attrib attrib, unsigned count) {
      if (!attrib.is_valid()) {
         utils::log(utils::LOG_WARN, "adding an invalid attribute '%s' when rendering\n", attrib.name().c_str());

         // guess how big it is
         auto elem_size_guess
            = attrib.type() == ValueType::Unknown ? 4
            : attrib_atomic_val_bytes(attrib.type());

         return skip_bytes(count * elem_size_guess);
      }

      auto slice_size = count * attrib_atomic_val_bytes(attrib.type());
      spec_prototype_.attribs.push_back({ std::move(attrib), count, 0, pos_bytes_ });
      pos_bytes_ += slice_size;

      return *this;
   }

   buffer_spec_builder_t & buffer_spec_builder_t::skip_bytes(unsigned bytes) {
      pos_bytes_ += bytes;

      return *this;
   }

   buffer_spec_t buffer_spec_builder_t::build() const {
      decltype(spec_prototype_.attribs) attribs;

      // prototype doesnt have correct stride
      for (auto & d : spec_prototype_.attribs) {
         attribs.push_back({ d.attrib, d.count, pos_bytes_, d.offset_bytes });
      }

      return{ spec_prototype_.buffer, std::move(attribs) };
   }

   buffer_spec_builder_t describe_buffer(buffer_t buffer) {
      return{ buffer };
   }





   //buffer_spec_t buffer(buffer_t b, std::initializer_list<attrib_data> attribs) {
   //   return{ b, { std::begin(attribs), std::end(attribs) } };
   //}

   //buffer_spec_t buffer(std::initializer_list<attrib_data> attribs) {
   //   return{ {}, { std::begin(attribs), std::end(attribs) } };
   //}


  /**
   * draw_helper_t
   *
   */

   draw_helper_t & draw_helper_t::with(static_array_t array, std::initializer_list<attrib_data> attribs) {
	   vertex_data_.push_back(array_spec_t{ std::move(array), { std::begin(attribs), std::end(attribs) } });
      return *this;
   }

   draw_helper_t & draw_helper_t::with(array_spec_t array_spec) {
      vertex_data_.push_back(std::move(array_spec));
      return *this;
   }

   draw_helper_t & draw_helper_t::with(buffer_spec_t buffer_spec) {
      vertex_data_.push_back(std::move(buffer_spec));
      return *this;
   }


   //draw_helper_t & draw_helper_t::validate_attribs(bool validate) {
   //   if (!validate) return *this;

   //   struct slice_info { unsigned offset; unsigned long size; };
   //   struct array_info { void * const data; unsigned block_size; std::vector<slice_info> slices; };

   //   std::vector<array_info> arrays;
   //   auto find_array_info = [&](void * const p, unsigned block_size) -> array_info & {
   //      for (auto & a : arrays) {
   //         if (a.data == p) {
   //            assert(a.block_size == block_size && "attribute bindings disagree on block sizes");
   //            return a;
   //         }
   //      }
   //      arrays.push_back({ p, block_size });
   //      return arrays.back();
   //   };

   //   for (auto & b : attrib_bindings_) {
   //      // find or add the array info
   //      auto & arrinfo = find_array_info(b.array.data(), b.calc_stride_bytes());

   //      // add the slice info for this attrib in the array
   //      auto ins_it = arrinfo.slices.begin();
   //      while (ins_it != arrinfo.slices.end() && b.offset_bytes > ins_it->offset)
   //         ++ins_it;

   //      arrinfo.slices.insert(ins_it, { b.offset_bytes, b.count * b.array.elem_size() });
   //   }

   //   // validate the structure of the slices within each array
   //   for (auto & arrinfo : arrays) {
   //      auto block_size = arrinfo.block_size;
   //      auto end_of_last_attrib = 0U;
   //      for (auto & slice : arrinfo.slices) {
   //         assert(slice.offset < block_size && "attribute data exceeds the expected 'block' size");
   //         assert(slice.offset >= end_of_last_attrib && "attribute data overlaps another attribute's data");
   //         auto end_of_attrib = slice.offset + slice.size;
   //         assert(end_of_attrib <= block_size && "attribute data runs off the end of the block");
   //         end_of_last_attrib = end_of_attrib;
   //      }
   //   }

   //   return *this;
   //}



   draw_helper_t & draw_helper_t::draw(DrawMode type) {
      prg_.use();
      for (auto & v : vertex_data_) v.draw(type);
      return *this;
   }

   draw_helper_t & draw_helper_t::draw(DrawMode type, unsigned first, unsigned count) {
      prg_.use();
      for (auto & v : vertex_data_) v.draw(type, first, count);
      return *this;
   }


  /**
   * class program
   *
   */

   program::program()
      : id_(glCreateProgram()) {
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

   program::program(program & other, shader s1, shader s2)
      : program() {
      try {
         attach(std::move(s1));
         attach(std::move(s2));

         for (auto & v : other.uniforms_) uniforms_.push_back({ v.name() });
         for (auto & v : other.attribs_) attribs_.push_back({ v.name() });

         reload();
      }
      catch (...) {
         destroy();
         throw;
      }
   }

   program program::create_using_vars_from(program & other, shader s1, shader s2) {
      return{ other, std::move(s1), std::move(s2) };
   }

   void program::reload(shader s1, shader s2) {
      for (auto & s : shaders_) glDetachShader(id_, s.id());
      shaders_.clear();

      attach(std::move(s1));
      attach(std::move(s2));

      reload();
   }

   program::~program() {
      destroy();
   }

   void program::reload() {
      decltype(uniforms_) old_uniforms;
      old_uniforms.swap(uniforms_);

      decltype(attribs_) old_attribs;
      old_attribs.swap(attribs_);

      auto find_uniform_with_name = [&](std::string const & name) {
         for (auto it = old_uniforms.begin(); it != old_uniforms.end(); ++it) {
            if (it->name() == name) return it;
         }

         return old_uniforms.end();
      };

      auto find_attrib_with_name = [&](std::string const & name) {
         for (auto it = old_attribs.begin(); it != old_attribs.end(); ++it) {
            if (it->name() == name) return it;
         }

         return old_attribs.end();
      };

      glLinkProgram(id_);

      use();

      GLint num_uniforms;
      glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &num_uniforms);

      for (auto idx = 0; idx < num_uniforms; idx++) {
         int size;
         unsigned int gl_type;
         char name[513];

         glGetActiveUniform(id_, idx, 512, nullptr, &size, &gl_type, name);
         assert(idx == glGetUniformLocation(id_, name));

         auto type = gl_to_value_type(gl_type);
         auto it = find_uniform_with_name(name);
         if (it == old_uniforms.end()) {
            uniforms_.push_back({ name, idx, size, type });
         }
         else {
            it->reset(idx, size, type);
            uniforms_.push_back(*it);
            old_uniforms.erase(it);
         }
      }

      GLint num_attribs;
      glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &num_attribs);

      for (auto idx = 0; idx < num_attribs; idx++) {
         int size;
         unsigned int gl_type;
         char name[513];

         glGetActiveAttrib(id_, idx, 512, nullptr, &size, &gl_type, name);
         assert(idx == glGetAttribLocation(id_, name));

         auto type = gl_to_value_type(gl_type);
         auto it = find_attrib_with_name(name);
         if (it == old_attribs.end()) {
            attribs_.push_back({ name, idx, size, type });
         }
         else {
            it->reset(idx, size, type);
            attribs_.push_back(*it);
            old_attribs.erase(it);
         }
      }

      // make sure we keep handles to the old uniforms so existing handles dont just break
      for (auto & u : old_uniforms) {
         u.reset();
         uniforms_.push_back(u);
      }
      for (auto & a : old_attribs) {
         a.reset();
         attribs_.push_back(a);
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

   attrib program::attrib(std::string const & name) {
      for (auto & a : attribs_) {
         if (a.name() == name) return a;
      }

      // not found, create and start tracking an invalid attrib
      auto new_attrib = gl::attrib{ name };
      attribs_.push_back(new_attrib);
      return new_attrib;
   }

   program::program(program && other) {
      *this = std::move(other);

      other.id_ = 0;
   }

   program & program::operator=(program && other) {
      std::swap(id_, other.id_);

      std::swap(shaders_, other.shaders_);
      std::swap(uniforms_, other.uniforms_);
      std::swap(attribs_, other.attribs_);

      return *this;
   }

   void program::use() const {
      glUseProgram(id_);
   }

   void program::destroy() {
      shaders_.clear();
      for (auto & u : uniforms_) { u.reset(); }
      for (auto & a : attribs_) { a.reset(); }

      glDeleteProgram(id_);
      id_ = 0;
   }

   void program::attach(shader s) {
      GL_VERIFY(glAttachShader(id_, s.id()));
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

   draw_helper_t program::pass() {
      return{ *this };
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

   void set_uniform(int location, glm::mat4 const & mat) { GL_VERIFY(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
   void set_uniform(int location, glm::mat3 const & mat) { GL_VERIFY(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
   void set_uniform(int location, glm::mat2 const & mat) { GL_VERIFY(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
   void set_uniform(int location, glm::vec4 const & vec) { GL_VERIFY(glUniform4f(location, vec.x, vec.y, vec.z, vec.w)); }
   void set_uniform(int location, glm::vec3 const & vec) { GL_VERIFY(glUniform3f(location, vec.x, vec.y, vec.z)); }
   void set_uniform(int location, glm::vec2 const & vec) { GL_VERIFY(glUniform2f(location, vec.x, vec.y)); }
   void set_uniform(int location, float f) { GL_VERIFY(glUniform1f(location, f)); }
   void set_uniform(int location, int i) { GL_VERIFY(glUniform1i(location, i)); }
   void set_uniform(int location, texture_t const & tex) { GL_VERIFY(glUniform1i(location, tex.texture_unit())); }

   //void set_uniform(GLint location, GLuint i) { glUniform1ui(location, i); }
}