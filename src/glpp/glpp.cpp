#include <glpp/glpp.h>

#ifdef WIN32
#  include <GLES2/gl2.h>
#  include <GLES2/gl2ext.h>

#  define USE_OPENGL_ES_2
#else
#   include <GL/glew.h>
#   define USE_GLEW
#endif

#include <SOIL2/SOIL2.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <algorithm>
#include <utility>
#include <atomic>
#include <mutex>
#include <cassert>

#include <glpp/utils.h>

void checkOpenGLError(const char* function, const char* file, int line) {
   GLenum err = glGetError(); if (err == GL_NO_ERROR) return;
   utils::log(utils::LOG_ERROR, "OpenGL error '%s' (0x%04x) called from %s in file %s line %d\n", glpp::openGlErrorString(err), err, function, file, line);
}

void checkOpenGLError(const char* stmt, const char* function, const char* file, int line) {
   GLenum err = glGetError(); if (err == GL_NO_ERROR) return;
   utils::log(utils::LOG_ERROR, "OpenGL error '%s' (0x%04x) at %s called from %s in file %s line %d\n", glpp::openGlErrorString(err), err, stmt, function, file, line);
}

namespace {
   bool extensionEnabled(std::string const & extName) {
      const char* extString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
      return strstr(extString, extName.c_str()) != NULL;
   }

   std::string get_shader_info_log(glpp::id_t shader_id)
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

   void delete_program_shaders(glpp::id_t id) {
      if (0 == id) return;

      GLint shader_count;
      glGetProgramiv(id, GL_ATTACHED_SHADERS, &shader_count);

      std::vector<GLuint> shaders(shader_count);
      glGetAttachedShaders(id, shader_count, nullptr, shaders.data());

      for (auto s : shaders) { glDeleteShader(s); }
   }

   GLenum to_glenum(glpp::shader::Type type) {
      switch (type) {
      case glpp::shader::Vertex: return GL_VERTEX_SHADER;
      case glpp::shader::Fragment: return GL_FRAGMENT_SHADER;
      default: throw glpp::error("unsupported shader type");
      }
   }

   glpp::ValueType gl_to_value_type(GLuint type) {
      switch (type) {
      case GL_BYTE: return glpp::ValueType::Byte;
      case GL_UNSIGNED_BYTE: return glpp::ValueType::UByte;
      case GL_SHORT: return glpp::ValueType::Short;
      case GL_UNSIGNED_SHORT: return glpp::ValueType::UShort;
      case GL_INT: return glpp::ValueType::Int;
      case GL_UNSIGNED_INT: return glpp::ValueType::UInt;
      case GL_FLOAT: return glpp::ValueType::Float;
      case GL_FIXED: return glpp::ValueType::Fixed;

      case GL_FLOAT_VEC2: return glpp::ValueType::FloatVec2;
      case GL_FLOAT_VEC3: return glpp::ValueType::FloatVec3;
      case GL_FLOAT_VEC4: return glpp::ValueType::FloatVec4;
      case GL_INT_VEC2: return glpp::ValueType::IntVec2;
      case GL_INT_VEC3: return glpp::ValueType::IntVec3;
      case GL_INT_VEC4: return glpp::ValueType::IntVec4;
      case GL_BOOL: return glpp::ValueType::Bool;
      case GL_BOOL_VEC2: return glpp::ValueType::BoolVec2;
      case GL_BOOL_VEC3: return glpp::ValueType::BoolVec3;
      case GL_BOOL_VEC4: return glpp::ValueType::BoolVec4;
      case GL_FLOAT_MAT2: return glpp::ValueType::FloatMat2;
      case GL_FLOAT_MAT3: return glpp::ValueType::FloatMat3;
      case GL_FLOAT_MAT4: return glpp::ValueType::FloatMat4;
      case GL_SAMPLER_2D: return glpp::ValueType::Sampler2d;
      case GL_SAMPLER_CUBE: return glpp::ValueType::SamplerCube;
      default: return glpp::ValueType::Unknown;
      }
   }

   std::string to_string(glpp::ValueType type) {
      switch (type) {
      case glpp::ValueType::Unknown: return "Unknown";
      case glpp::ValueType::Int: return "Int";
      case glpp::ValueType::UInt: return "UInt";
      case glpp::ValueType::Float: return "Float";
      case glpp::ValueType::FloatVec2: return "FloatVec2";
      case glpp::ValueType::FloatVec3: return "FloatVec3";
      case glpp::ValueType::FloatVec4: return "FloatVec4";
      case glpp::ValueType::IntVec2: return "IntVec2";
      case glpp::ValueType::IntVec3: return "IntVec3";
      case glpp::ValueType::IntVec4: return "IntVec4";
      case glpp::ValueType::FloatMat2: return "FloatMat2";
      case glpp::ValueType::FloatMat3: return "FloatMat3";
      case glpp::ValueType::FloatMat4: return "FloatMat4";

      case glpp::ValueType::Bool: return "Bool";
      case glpp::ValueType::Byte: return "Byte";
      case glpp::ValueType::UByte: return "UByte";
      case glpp::ValueType::Short: return "Short";
      case glpp::ValueType::UShort: return "UShort";
      case glpp::ValueType::Fixed: return "Fixed";
      case glpp::ValueType::BoolVec2: return "BoolVec2";
      case glpp::ValueType::BoolVec3: return "BoolVec3";
      case glpp::ValueType::BoolVec4: return "BoolVec4";
      case glpp::ValueType::Sampler2d: return "Sampler2d";
      case glpp::ValueType::SamplerCube: return "SamplerCube";

      case glpp::ValueType::FloatMat2x3: return "FloatMat2x3";
      case glpp::ValueType::FloatMat2x4: return "FloatMat2x4";
      case glpp::ValueType::FloatMat3x2: return "FloatMat3x2";
      case glpp::ValueType::FloatMat3x4: return "FloatMat3x4";
      case glpp::ValueType::FloatMat4x2: return "FloatMat4x2";
      case glpp::ValueType::FloatMat4x3: return "FloatMat4x3";
      case glpp::ValueType::UIntVec2: return "UIntVec2";
      case glpp::ValueType::UIntVec3: return "UIntVec3";
      case glpp::ValueType::UIntVec4: return "UIntVec4";
      case glpp::ValueType::Double: return "Double";
      case glpp::ValueType::DoubleVec2: return "DoubleVec2";
      case glpp::ValueType::DoubleVec3: return "DoubleVec3";
      case glpp::ValueType::DoubleVec4: return "DoubleVec4";
      case glpp::ValueType::DoubleMat2: return "DoubleMat2";
      case glpp::ValueType::DoubleMat3: return "DoubleMat3";
      case glpp::ValueType::DoubleMat4: return "DoubleMat4";
      case glpp::ValueType::DoubleMat2x3: return "DoubleMat2x3";
      case glpp::ValueType::DoubleMat2x4: return "DoubleMat2x4";
      case glpp::ValueType::DoubleMat3x2: return "DoubleMat3x2";
      case glpp::ValueType::DoubleMat3x4: return "DoubleMat3x4";
      case glpp::ValueType::DoubleMat4x2: return "DoubleMat4x2";
      case glpp::ValueType::DoubleMat4x3: return "DoubleMat4x3";

      default: return "UNRECOGNISED TYPE";
      }
   }

   
   std::size_t attrib_atomic_val_bytes(glpp::ValueType type) {
      switch (type) {
      case glpp::ValueType::Byte:
      case glpp::ValueType::UByte:
         return 1;

      case glpp::ValueType::Short:
      case glpp::ValueType::UShort:
         return 2;

      case glpp::ValueType::Int:
      case glpp::ValueType::UInt:
      case glpp::ValueType::Float:
      case glpp::ValueType::FloatVec2:
      case glpp::ValueType::FloatVec3:
      case glpp::ValueType::FloatVec4:
      case glpp::ValueType::IntVec2:
      case glpp::ValueType::IntVec3:
      case glpp::ValueType::IntVec4:
      case glpp::ValueType::FloatMat2:
      case glpp::ValueType::FloatMat3:
      case glpp::ValueType::FloatMat4:

      case glpp::ValueType::FloatMat2x3:
      case glpp::ValueType::FloatMat2x4:
      case glpp::ValueType::FloatMat3x2:
      case glpp::ValueType::FloatMat3x4:
      case glpp::ValueType::FloatMat4x2:
      case glpp::ValueType::FloatMat4x3:
      case glpp::ValueType::UIntVec2:
      case glpp::ValueType::UIntVec3:
      case glpp::ValueType::UIntVec4:
         return 4;

      case glpp::ValueType::Double:
      case glpp::ValueType::DoubleVec2:
      case glpp::ValueType::DoubleVec3:
      case glpp::ValueType::DoubleVec4:
      case glpp::ValueType::DoubleMat2:
      case glpp::ValueType::DoubleMat3:
      case glpp::ValueType::DoubleMat4:
      case glpp::ValueType::DoubleMat2x3:
      case glpp::ValueType::DoubleMat2x4:
      case glpp::ValueType::DoubleMat3x2:
      case glpp::ValueType::DoubleMat3x4:
      case glpp::ValueType::DoubleMat4x2:
      case glpp::ValueType::DoubleMat4x3:
         return 8;

      default: throw glpp::error("cannot calculate primitive size of invalid attribute type");
      }
   }


   GLuint attrib_atomic_gl_type(glpp::ValueType type) {
      switch (type) {
      case glpp::ValueType::Byte: return GL_BYTE;
      case glpp::ValueType::UByte: return GL_UNSIGNED_BYTE;
      case glpp::ValueType::Short: return GL_SHORT;
      case glpp::ValueType::UShort: return GL_UNSIGNED_SHORT;
      case glpp::ValueType::Int: return GL_INT;
      case glpp::ValueType::UInt: return GL_UNSIGNED_INT;
      case glpp::ValueType::Float: return GL_FLOAT;
      case glpp::ValueType::Double: return GL_DOUBLE;

      case glpp::ValueType::FloatVec2: return GL_FLOAT;
      case glpp::ValueType::FloatVec3: return GL_FLOAT;
      case glpp::ValueType::FloatVec4: return GL_FLOAT;
      case glpp::ValueType::IntVec2: return GL_INT;
      case glpp::ValueType::IntVec3: return GL_INT;
      case glpp::ValueType::IntVec4: return GL_INT;

      case glpp::ValueType::FloatMat2: return GL_FLOAT;
      case glpp::ValueType::FloatMat3: return GL_FLOAT;
      case glpp::ValueType::FloatMat4: return GL_FLOAT;

      default: throw glpp::error("invalid data value type when drawing");
      }
   }
}

namespace glpp
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

   texture_t::texture_t(std::string const & filename)
      : state_(std::make_shared<state>(filename)) {
   }

   texture_t::texture_t(dim_t const & dims, Format format)
      : state_(std::make_shared<state>(dims, format)) {
   }

   texture_t::state::state(std::string const & filename)
   : format_(texture_t::RGBA) { // TODO: this may not be right?
      int width;
      int height;
      int resized_width;
      int resized_height;
      int channels;

      id_ = SOIL_load_OGL_texture_and_details(
         filename.c_str(),
         SOIL_LOAD_AUTO,
         SOIL_CREATE_NEW_ID,
         SOIL_FLAG_INVERT_Y, // SOIL_FLAG_MIPMAPS // BEWARE mipmaps make colours bleed! (alters the tex params)
         &width, &height, &resized_width, &resized_height, &channels);

      if (!id_) {
         throw glpp::error(std::string("Could not load texture from '") + filename + "': " + SOIL_last_result());
      }

      dims_.x = width;
      dims_.y = height;

      GL_VERIFY(glBindTexture(GL_TEXTURE_2D, id_));

      // set texture parameters
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      GL_VERIFY(glBindTexture(GL_TEXTURE_2D, 0));
   }

   texture_t::state::state(dim_t const & dims, texture_t::Format format)
   : dims_(dims), format_(format) {
      GL_VERIFY(glGenTextures(1, &id_));
      GL_VERIFY(glBindTexture(GL_TEXTURE_2D, id_));

      auto fmt
         = format == texture_t::RGBA ? GL_RGBA
         : format == texture_t::RGB ? GL_RGB
#ifdef WIN32
         : format == texture_t::BGRA ? GL_BGRA_EXT
#endif
         : throw error("unrecognised image format");

      // TODO: add GL_ARB_texture_storage, and use e.g., RGB8 etc for storage...
      GL_VERIFY(glTexImage2D(GL_TEXTURE_2D, 0, fmt, dims_.x, dims_.y, 0, fmt, GL_UNSIGNED_BYTE, NULL));
      //GL_VERIFY(glTexStorage2D(GL_TEXTURE_2D, 1, fmt, dims_.x, dims_.y));
     // GL_VERIFY(glTexSubImage2D(GL_TEXTURE_2D, 1, 0, 0, dims_.x, dims_.y, fmt, GL_UNSIGNED_BYTE, NULL));

      // set texture parameters
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
      GL_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      GL_VERIFY(glBindTexture(GL_TEXTURE_2D, 0));
   }

   texture_t::state::~state() {
      glDeleteTextures(1, &id_);
   }

   void texture_t::save(std::string const & filename) const {
      std::vector<uint8_t> buffer(4 * state_->dims_.x * state_->dims_.y);

      auto format = state_->format_;
      auto fmt
         = format == texture_t::RGBA ? GL_RGBA
         : format == texture_t::RGB ? GL_RGB
#ifdef WIN32
         : format == texture_t::BGRA ? GL_BGRA_EXT
#endif
         : throw error("unrecognised image format");

      GL_VERIFY(glReadPixels(0, 0, state_->dims_.x, state_->dims_.y, fmt, GL_UNSIGNED_BYTE, &buffer[0]));

      auto soil_format = [&filename] {
         auto ext = filename.substr(filename.length() - 4);
         if (ext == ".png") return SOIL_SAVE_TYPE_PNG;
         if (ext == ".tga") return SOIL_SAVE_TYPE_TGA;
         if (ext == ".bmp") return SOIL_SAVE_TYPE_BMP;
         if (ext == ".dds") return SOIL_SAVE_TYPE_DDS;
         utils::log(utils::LOG_WARN, "unrecognised extension '%s' when saving %s, defaulting to PNG\n", ext.c_str(), filename.c_str());
         return SOIL_SAVE_TYPE_PNG;
      }();

      SOIL_save_image(
         filename.c_str(), soil_format,
         state_->dims_.x, state_->dims_.y, 4, // TODO: channels?
         buffer.data());
   }

   void texture_t::bind() const {
      GL_VERIFY(glBindTexture(GL_TEXTURE_2D, id()));

      // TODO: make this stuff configurable via the with() call...
      // TODO: or should this be initialised in texture creation?
      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // GL_NEAREST
      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // GL_CLAMP_TO_EDGE
      GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
   }


   /**
    * class texture_unit_t
    */

   void texture_unit_t::activate() const {
      GL_VERIFY(glActiveTexture(GL_TEXTURE0 + id));
   }

  /**
   * class frame_buffer_t
   *
   */

   frame_buffer_t::frame_buffer_t(texture_t const & tex)
   : dims_(tex.dims()), samples_(0) {
      GL_VERIFY(glGenFramebuffers(1, &fbo_id_));
      GL_VERIFY(glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_));

      // attach images (texture objects and renderbuffer objects) for each buffer (color, depth, stencil or a combination of depth and stencil)
      GL_VERIFY(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.id(), 0));

      // create render buffer for stencil and depth (we arent interested in reading it)
      GL_VERIFY(glGenRenderbuffers(1, &depth_rbo_id_));
      GL_VERIFY(glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_id_));
      GL_VERIFY(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, dims_.x, dims_.y)); // GL_DEPTH24_STENCIL8
      GL_VERIFY(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_id_)); // GL_DEPTH_STENCIL_ATTACHMENT

      GL_VERIFY(glBindRenderbuffer(GL_RENDERBUFFER, 0));

      check_fbo();

      GL_VERIFY(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }

   frame_buffer_t::frame_buffer_t(dim_t dims, unsigned samples)
      : dims_(dims), samples_(samples) {
      // NOTE: ANGLE doesn't support glTexImage2DMultisample (for MSAA), only glRenderbufferStorageMultisampleANGLE

      GL_VERIFY(glGenFramebuffers(1, &fbo_id_));
      GL_VERIFY(glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_));

      // TODO: verify samples_ is valid
      // must use GL_BGRA8_EXT because thats what the default buffer format is in ANGLE (must match for blitting)
      GL_VERIFY(glGenRenderbuffers(1, &colour_rbo_id_));
      GL_VERIFY(glBindRenderbuffer(GL_RENDERBUFFER, colour_rbo_id_));
#ifdef WIN32
      if (samples_ == 0) GL_VERIFY(glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA8_EXT, dims_.x, dims_.y));
      else GL_VERIFY(glRenderbufferStorageMultisampleANGLE(GL_RENDERBUFFER, samples_, GL_BGRA8_EXT, dims_.x, dims_.y));
#else
      if (samples_ == 0) GL_VERIFY(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, dims_.x, dims_.y));
      else GL_VERIFY(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_, GL_RGBA8, dims_.x, dims_.y));
#endif
      GL_VERIFY(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colour_rbo_id_));

      GL_VERIFY(glGenRenderbuffers(1, &depth_rbo_id_));
      GL_VERIFY(glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_id_));
#ifdef WIN32
      if (samples_ == 0) GL_VERIFY(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, dims_.x, dims_.y));
      else GL_VERIFY(glRenderbufferStorageMultisampleANGLE(GL_RENDERBUFFER, samples_, GL_DEPTH_COMPONENT16, dims_.x, dims_.y));
#else
      if (samples_ == 0) GL_VERIFY(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, dims_.x, dims_.y));
      else GL_VERIFY(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_, GL_DEPTH_COMPONENT16, dims_.x, dims_.y));
#endif
      GL_VERIFY(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_id_));

      GL_VERIFY(glBindRenderbuffer(GL_RENDERBUFFER, 0));

      check_fbo();

      GL_VERIFY(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }

   frame_buffer_t::~frame_buffer_t() {
      if (0 != depth_rbo_id_) GL_VERIFY(glDeleteRenderbuffers(1, &depth_rbo_id_));
      if (0 != colour_rbo_id_) GL_VERIFY(glDeleteRenderbuffers(1, &colour_rbo_id_));
      GL_VERIFY(glDeleteFramebuffers(1, &fbo_id_));
   }

   void frame_buffer_t::bind(BindTarget target) const {
      check_fbo();
      GLenum t
         = target == ReadDraw ? GL_FRAMEBUFFER
         : target == Read ? GL_READ_FRAMEBUFFER_ANGLE
         : target == Draw ? GL_DRAW_FRAMEBUFFER_ANGLE
         : throw error("unrecognised bind target");

      if (t == ReadDraw) glBindFramebuffer(GL_READ_FRAMEBUFFER_ANGLE, 0);
      GL_VERIFY(glBindFramebuffer(t, fbo_id_));
   }

   void frame_buffer_t::unbind(BindTarget target) const {
      GLenum t
         = target == ReadDraw ? GL_FRAMEBUFFER
         : target == Read ? GL_READ_FRAMEBUFFER_ANGLE
         : target == Draw ? GL_DRAW_FRAMEBUFFER_ANGLE
         : throw error("unrecognised bind target");
      GL_VERIFY(glBindFramebuffer(t, 0));
   }

   void frame_buffer_t::blit_to_draw_buffer() const {
      // https://www.opengl.org/wiki/Framebuffer_Object
      // from https://www.opengl.org/wiki/Multisampling

      // you can do a full screen blit from an MSAA fbo to a non - MSAA fbo if they're the same format
      // see BlitFramebufferANGLE on entry_points_gles_2_0_ext.cpp : 667

      //GL_VERIFY(glBindFramebuffer(GL_DRAW_FRAMEBUFFER_ANGLE, 0));   // Make sure no FBO is set as the draw framebuffer
      GL_VERIFY(glBindFramebuffer(GL_READ_FRAMEBUFFER_ANGLE, fbo_id_)); // Make sure your multisampled FBO is the read framebuffer
#ifdef WIN32
      GL_VERIFY(glBlitFramebufferANGLE(0, 0, dims_.x, dims_.y, 0, 0, dims_.x, dims_.y, GL_COLOR_BUFFER_BIT, GL_NEAREST));
#else
      GL_VERIFY(glBlitFramebuffer(0, 0, dims_.x, dims_.y, 0, 0, dims_.x, dims_.y, GL_COLOR_BUFFER_BIT, GL_NEAREST));
#endif
   }

   void frame_buffer_t::blit_to_screen() const {
      GL_VERIFY(glBindFramebuffer(GL_DRAW_FRAMEBUFFER_ANGLE, 0));
      blit_to_draw_buffer();
   }


   void frame_buffer_t::check_fbo() const {
      // Your framebuffer can only be used as a render target if memory has been allocated to store the results
      auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
         auto status_msg = [status] {
            switch (status) {
               //case GL_FRAMEBUFFER_UNDEFINED: return "GL_FRAMEBUFFER_UNDEFINED";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT ";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ";
               //case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER : return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER ";
               //case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER : return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER ";
            case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED ";
               //case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE : return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE ";
               //case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS : return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS ";
            default: return "UNRECOGNISED ENUM";
            }
         }();

         utils::log(utils::LOG_ERROR, "framebuffer not initialised correctly: error 0x%x (%s)\n", status, status_msg);
      }

      GL_CHECK();
   }

   id_t current_frame_buffer() {
      int fbo;
      GL_VERIFY(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo));
      return fbo;
   }

   void bind_frame_buffer(id_t new_fbo) {
      GL_VERIFY(glBindFramebuffer(GL_FRAMEBUFFER, new_fbo));
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
      , type_(type)
   {
      GL_CHECK();

      const char * srcdata = source.c_str();
      GL_VERIFY(glShaderSource(id_, 1, &srcdata, NULL));
      GL_VERIFY(glCompileShader(id_));

      GLint success;
      GL_VERIFY(glGetShaderiv(id_, GL_COMPILE_STATUS, &success));
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
   * ValueType functions
   **/

   template <> ValueType value_type<int8_t>() { return ValueType::Byte; }
   template <> ValueType value_type<uint8_t>() { return ValueType::UByte; }
   template <> ValueType value_type<int16_t>() { return ValueType::Short; }
   template <> ValueType value_type<uint16_t>() { return ValueType::UShort; }
   template <> ValueType value_type<int32_t>() { return ValueType::Int; }
   template <> ValueType value_type<uint32_t>() { return ValueType::UInt; }
   template <> ValueType value_type<float>() { return ValueType::Float; }
   template <> ValueType value_type<glm::ivec2>() { return ValueType::IntVec2; }
   template <> ValueType value_type<glm::ivec3>() { return ValueType::IntVec3; }
   template <> ValueType value_type<glm::ivec4>() { return ValueType::IntVec4; }
   template <> ValueType value_type<glm::vec2>() { return ValueType::FloatVec2; }
   template <> ValueType value_type<glm::vec3>() { return ValueType::FloatVec3; }
   template <> ValueType value_type<glm::vec4>() { return ValueType::FloatVec4; }
   template <> ValueType value_type<glm::mat2>() { return ValueType::FloatMat2; }
   template <> ValueType value_type<glm::mat3>() { return ValueType::FloatMat3; }
   template <> ValueType value_type<glm::mat4>() { return ValueType::FloatMat4; }
   template <> ValueType value_type<texture_unit_t>() { return ValueType::Sampler2d; }


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
      template <> ValueType uniform_type<glm::ivec2>() { return ValueType::IntVec2; }
      template <> ValueType uniform_type<glm::ivec3>() { return ValueType::IntVec3; }
      template <> ValueType uniform_type<glm::ivec4>() { return ValueType::IntVec4; }
      template <> ValueType uniform_type<glm::vec2>() { return ValueType::FloatVec2; }
      template <> ValueType uniform_type<glm::vec3>() { return ValueType::FloatVec3; }
      template <> ValueType uniform_type<glm::vec4>() { return ValueType::FloatVec4; }
      template <> ValueType uniform_type<glm::mat2>() { return ValueType::FloatMat2; }
      template <> ValueType uniform_type<glm::mat3>() { return ValueType::FloatMat3; }
      template <> ValueType uniform_type<glm::mat4>() { return ValueType::FloatMat4; }
      template <> ValueType uniform_type<texture_unit_t>() { return ValueType::Sampler2d; }

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

         glpp::set_uniform(u.location(), val);
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

   void uniform::set(glm::ivec2 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::ivec3 const & val) {
      auto success = set_uniform(*this, val, !state_->error_);
      if (!success) state_->error_ = true;
   }

   void uniform::set(glm::ivec4 const & val) {
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

   void uniform::set(texture_unit_t tex) {
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

   buffer_t::state::state(void* vertex_data, std::size_t vertex_data_byte_size)
      : vertex_buffer_size_(vertex_data_byte_size) {
      if (vertex_data == nullptr) return;

      glGenBuffers(1, &vertex_id_);
      glBindBuffer(GL_ARRAY_BUFFER, vertex_id_);
      glBufferData(GL_ARRAY_BUFFER, vertex_data_byte_size, vertex_data, GL_STATIC_DRAW);
   }

   buffer_t::state::state(void* vertex_data, std::size_t vertex_byte_size, void* index_data, ValueType index_data_type, unsigned index_count, std::size_t index_byte_size)
   : state(vertex_data, vertex_byte_size) {
      index_data_type_ = index_data_type;
      index_count_ = index_count;
      // TODO: infer byte size?
      assert(index_byte_size == index_count * attrib_atomic_val_bytes(index_data_type));
      glGenBuffers(1, &index_id_);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_byte_size, index_data, GL_STATIC_DRAW);
   }

   buffer_t::state::state(void* index_data, ValueType index_data_type, unsigned index_count, std::size_t index_byte_size)
   : state(nullptr, 0, index_data, index_data_type, index_count, index_byte_size) {
   }

   buffer_t::state::~state() {
      if (0 != vertex_id_) glDeleteBuffers(1, &vertex_id_);
      if (0 != index_id_) glDeleteBuffers(1, &index_id_);
   }


   buffer_t::buffer_t(Target target, static_array_t data) {
      switch (target) {
      case ArrayBuffer:
         state_ = std::make_shared<state>(data.data(), data.size());
         break;
      case IndexBuffer:
         state_ = std::make_shared<state>(data.data(), data.data_type(), data.elem_count(), data.size());
         break;
      default:
         assert(false && "unrecognised buffer target");
         throw error("unrecognised buffer target");
      }
   }

   buffer_t::buffer_t(static_array_t vertex_data)
      : state_(std::make_shared<state>(vertex_data.data(), vertex_data.size())) {
   }

   buffer_t::buffer_t(static_array_t vertex_data, static_array_t index_data)
      : state_(std::make_shared<state>(vertex_data.data(), vertex_data.size(), index_data.data(), index_data.data_type(), index_data.elem_count(), index_data.size())) {
      assert((index_data.data_type() == ValueType::UInt || index_data.data_type() == ValueType::UShort || index_data.data_type() == ValueType::UByte) && "second buffer parameter must be index buffer (must be integral type)");
   }

   void buffer_t::bind() const {
      if (0 != state_->vertex_id_) {
         glBindBuffer(GL_ARRAY_BUFFER, state_->vertex_id_);
      }
      if (0 != state_->index_id_) {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state_->index_id_);
      }
   }


   unsigned attrib_info::calc_stride_bytes() const{
      return stride_bytes != 0
         ? stride_bytes
         : attrib_atomic_val_bytes(attrib.type()) * count;
   }

   unsigned num_vertices(buffer_spec_t const & packed) {
      auto block_count = [&](attrib_info const & b) {
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

   void bind(buffer_spec_t const & packed) {
      packed.buffer.bind();
      for (auto & attrib_info : packed.attribs) {
         auto gl_type = attrib_atomic_gl_type(attrib_info.attrib.type());

         GL_VERIFY(glVertexAttribPointer(
            attrib_info.attrib.location(), attrib_info.count, gl_type,
            false, attrib_info.stride_bytes, reinterpret_cast<void*>(attrib_info.offset_bytes)));

         GL_VERIFY(glEnableVertexAttribArray(attrib_info.attrib.location()));
      }
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

   void draw(buffer_spec_t const & b, DrawMode mode, unsigned first, unsigned count) {
      bind(b);

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



   buffer_spec_builder_t::buffer_spec_builder_t(buffer_t buffer)
      : state_(std::make_shared<state>(std::move(buffer))) {
   }

   buffer_spec_builder_t buffer_spec_builder_t::attrib(std::string attrib_name, unsigned count) {
      state_->slices_.push_back({ attrib_name, count });

      return *this;
   }

   buffer_spec_builder_t buffer_spec_builder_t::skip_bytes(unsigned bytes) {
      state_->slices_.push_back({ "", bytes });

      return *this;
   }

   buffer_spec_t buffer_spec_builder_t::build(program & prg) const {
      assert(state_->slices_.size() != 0 && "cannot build buffer spec without any attributes");

      buffer_spec_t::attribs_type attribs_sans_stride;

      unsigned pos_bytes = 0;

      for (auto & s : state_->slices_) {
         // process 'skip bytes' requests
         if (s.attrib_name == "") {
            // count is in bytes in this case
            pos_bytes += s.count;
            continue;
         }

         // assume slice is an attrib specification
         auto attrib = prg.attrib(s.attrib_name);

         if (!attrib.is_valid()) {
            // TODO: parse shader files to determine types (might be elided by compiler)

            utils::log(utils::LOG_WARN, "adding an invalid attribute '%s' when rendering\n", attrib.name().c_str());

            // guess how big it is
            auto elem_size_guess
               = attrib.type() == ValueType::Unknown ? 4
               : attrib_atomic_val_bytes(attrib.type());

            pos_bytes += s.count * elem_size_guess;
            continue;
         }

         // assume slice is valid
         auto slice_size = s.count * attrib_atomic_val_bytes(attrib.type());
         attribs_sans_stride.push_back({ std::move(attrib), s.count, 0, pos_bytes });
         pos_bytes += slice_size;
      }

      buffer_spec_t::attribs_type attribs;
      // prototype doesnt have correct stride
      for (auto & a : attribs_sans_stride) {
         attribs.push_back({ a.attrib, a.count, pos_bytes, a.offset_bytes });
      }

      // validate span divides into buffer nicely, warn if not

      if (0 != state_->buffer_.vertex_buffer_size() % pos_bytes) {
         utils::log(utils::LOG_WARN, "vertex buffer is not a multiple of the vertex data size!");
         assert(false && "vertex buffer is not a multiple of the vertex data size!");
      }

      return{ state_->buffer_, std::move(attribs) };
   }

   buffer_spec_builder_t describe_buffer(buffer_t buffer) {
      return{ buffer };
   }


   //buffer_spec_t buffer(buffer_t b, std::initializer_list<attrib_info> attribs) {
   //   return{ b, { std::begin(attribs), std::end(attribs) } };
   //}

   //buffer_spec_t buffer(std::initializer_list<attrib_info> attribs) {
   //   return{ {}, { std::begin(attribs), std::end(attribs) } };
   //}


  /**
   * pass_t
   *
   */


   struct pass_t::state {
      state(program prg) : prg_(prg) {}
      state(pass_t & parent) : prg_(parent.state_->prg_), parent_(new pass_t(parent)) {}
      state(state const &) = delete;
      state & operator=(state const &) = delete;

      program prg_;
      std::unique_ptr<pass_t> parent_;
      std::vector<buffer_spec_t> vertex_buffers_;
      int index_buffer_idx_ = -1;
      std::vector<std::pair<texture_unit_t, texture_t>> texture_bindings_;
      std::vector<std::pair<glpp::uniform, texture_t>> texture_bindings_without_tex_units_;
      std::vector<std::pair<glpp::uniform, uniform_action_t>> uniform_actions_;
   };


   pass_t::pass_t(program prg)
      : state_(std::make_shared<state>(prg)) {
   }

   pass_t::pass_t(extend_tag, pass_t & parent)
      : state_(std::make_shared<state>(parent)) {
   }

   pass_t::~pass_t() {}

   pass_t pass_t::extend() {
      return{ extend_tag(), *this };
   }

   pass_t & pass_t::with(buffer_spec_t buffer_spec) {
      state_->vertex_buffers_.push_back(std::move(buffer_spec));
      auto & b = state_->vertex_buffers_.back();

      // keep track of which buffer has the index data
      if (b.buffer.has_index_data()) {
         assert(!index_buffer_() && "render pass already has an index buffer");
         state_->index_buffer_idx_ = state_->vertex_buffers_.size() - 1;
      }
      return *this;
   }

   pass_t & pass_t::with(buffer_spec_builder_t const & builder) {
      return with(builder.build(state_->prg_));
   }

   template <>
   pass_t & pass_t::set_uniform<texture_t>(std::string const & name, texture_t val) {
      state_->texture_bindings_without_tex_units_.push_back({ uniform(name), val });
      return *this;
   }

   pass_t & pass_t::set_uniform_action(std::string const & name, uniform_action_t action) {
      // overwrite if existing
      for (auto & upair : state_->uniform_actions_) {
         if (upair.first.name() != name) continue;

         upair.second = action;
         return *this;
      }

      // no binding exists, add to end
      state_->uniform_actions_.push_back({ state_->prg_.uniform(name), action });
      return *this;
   }

   pass_t & pass_t::set_texture_unit(texture_unit_t u, texture_t tex) {
      // overwrite if existing
      for (auto & tpair : state_->texture_bindings_) {
         if (tpair.first.id != u.id) continue;

         tpair.second = tex;
         return *this;
      }

      // no binding exists, add to end
      state_->texture_bindings_.push_back({ u, tex });
      return *this;
   }

   buffer_t const * pass_t::index_buffer_() const {
      if (state_->index_buffer_idx_ != -1) return &state_->vertex_buffers_[state_->index_buffer_idx_].buffer;
      if (state_->parent_) return state_->parent_->index_buffer_();
      return nullptr;
   }


   //pass_t & pass_t::validate_attribs(bool validate) {
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

   unsigned pass_t::calc_draw_count_() const {
      auto draw_count = 0U;

      auto * index_buffer = index_buffer_();
      if (nullptr != index_buffer) return index_buffer->index_count();

      // TODO: verify that all buffers have sufficient elements to draw?

      for (auto & v : state_->vertex_buffers_) {
         draw_count = draw_count == 0 ? num_vertices(v) : std::min(draw_count, num_vertices(v));
      }

      return draw_count;
   }

   pass_t & pass_t::draw(DrawMode mode) {
      prepare_draw_();
      draw_(mode, 0, calc_draw_count_());
      return *this;
   }

   pass_t & pass_t::draw(DrawMode mode, unsigned first, unsigned count) {
      prepare_draw_();
      draw_(mode, first, count);
      return *this;
   }

   void pass_t::draw_(DrawMode mode, unsigned first, unsigned count) {
      auto * index_buffer = index_buffer_();
      if (nullptr != index_buffer) {
         auto index_count = index_buffer->index_count();
         assert(count <= index_count && "index count greater than number of indices in buffer while drawing indexed buffer");
         assert(first <= index_count && "first index greater than total number of indices in buffer while drawing indexed buffer");
         assert(first + count <= index_count && "end index greater than total number of indices in buffer while drawing indexed buffer");

         auto gl_type = attrib_atomic_gl_type(index_buffer->index_data_type());
         auto idx_count = count;
         auto idx_start = reinterpret_cast<GLvoid*>(first * attrib_atomic_val_bytes(index_buffer->index_data_type()));

         GL_VERIFY(glDrawElements(gl_draw_mode(mode), idx_count, gl_type, idx_start));
      }
      else {
         GL_VERIFY(glDrawArrays(gl_draw_mode(mode), first, count));
      }
   }

   pass_t & pass_t::draw_batch(render_callback const & cb, DrawMode mode) {
      prepare_draw_();

      auto draw_count = calc_draw_count_();
      while (cb.prepare_next(state_->prg_)) {
         draw_(mode, 0, draw_count);
      }

      return *this;
   }

   pass_t & pass_t::draw_batch(render_callback const & cb, DrawMode mode, unsigned first, unsigned count) {
      prepare_draw_();

      while (cb.prepare_next(state_->prg_)) {
         draw_(mode, first, count);
      }

      return *this;
   }

   uniform pass_t::uniform(std::string const & name) {
      return state_->prg_.uniform(name);
   }

   void pass_t::prepare_draw_() {
      if (state_->parent_) {
         state_->parent_->prepare_draw_();
      }
      else {
         state_->prg_.use();
      }

      for (auto & upair : state_->uniform_actions_) {
         upair.second(upair.first); // invoke action
      }

      // bind textures to the texture units used by this pass
      for (auto & tpair : state_->texture_bindings_) {
         texture_unit_t const & tex_unit = tpair.first;
         texture_t const & tex = tpair.second;

         tex_unit.activate();
         tex.bind();
      }

      if (!state_->texture_bindings_without_tex_units_.empty()) {
         std::vector<int> allocated_texture_units;
         auto add_texture_unit = [&](texture_unit_t const & u) {
            auto id = u.id;
            auto insert_it = std::upper_bound(std::begin(allocated_texture_units), std::end(allocated_texture_units), id);
            allocated_texture_units.insert(insert_it, id);
         };
         auto allocate_texture_unit = [&]() -> texture_unit_t {
            auto it = std::begin(allocated_texture_units);
            auto itEnd = std::end(allocated_texture_units);
            for (auto i = 0; i < 60; i++) {
               // find unallocated (the list is sorted)
               if (it == itEnd || *it != i) {
                  texture_unit_t u{ i };
                  add_texture_unit(u); // track so we dont re-allocate later
                  return u;
               }
               ++it;
            }
            throw error("logic error finding unallocated texture unit");
         };

         for (auto & tpair : state_->texture_bindings_) {
            add_texture_unit(tpair.first);
         }

         // allocate and bind unallocated textures
         for (auto & tpair : state_->texture_bindings_without_tex_units_) {
            auto tex_unit = allocate_texture_unit();
            texture_t const & tex = tpair.second;

            tpair.first.set(tex_unit);
            tex_unit.activate();
            tex.bind();
         }
      }

      for (auto & v : state_->vertex_buffers_) glpp::bind(v);
   }


  /**
   * class program
   *
   */

   program::state::state()
      : id_(glCreateProgram()) {
      GL_CHECK();
   }

   program::state::~state() {
      destroy();
   }

   void program::state::destroy() {
      shaders_.clear();
      for (auto & u : uniforms_) { u.reset(); }
      for (auto & a : attribs_) { a.reset(); }

      glDeleteProgram(id_);
      id_ = 0;
   }

   program::program()
      : state_(std::make_shared<state>()) {
      state_->id_ = glCreateProgram();
      GL_CHECK();
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

   void program::reload(shader s1, shader s2) {
      for (auto & s : state_->shaders_) {
         GL_VERIFY(glDetachShader(state_->id_, s.id()));
      }
      state_->shaders_.clear();

      attach(std::move(s1));
      attach(std::move(s2));

      reload();
   }

   program::~program() {
   }

   void program::reload() {
      auto & _ = *state_;
      decltype(_.uniforms_) old_uniforms;
      old_uniforms.swap(_.uniforms_);

      decltype(_.attribs_) old_attribs;
      old_attribs.swap(_.attribs_);

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

      GL_VERIFY(glLinkProgram(_.id_));

      use();

      GLint num_uniforms;
      GL_VERIFY(glGetProgramiv(_.id_, GL_ACTIVE_UNIFORMS, &num_uniforms));

      for (auto idx = 0; idx < num_uniforms; idx++) {
         int size;
         unsigned int gl_type;
         char name[513];

         GL_VERIFY(glGetActiveUniform(_.id_, idx, 512, nullptr, &size, &gl_type, name));
         auto location = glGetUniformLocation(_.id_, name);
         if (location == -1) {
            utils::log(utils::LOG_WARN, "uniform '%s' location unknown\n", name);
         }
         GL_CHECK();

         auto type = gl_to_value_type(gl_type);
         auto it = find_uniform_with_name(name);
         if (it == old_uniforms.end()) {
            _.uniforms_.push_back({ name, location, size, type });
         }
         else {
            it->reset(location, size, type);
            _.uniforms_.push_back(*it);
            old_uniforms.erase(it);
         }
      }

      GLint num_attribs;
      glGetProgramiv(_.id_, GL_ACTIVE_ATTRIBUTES, &num_attribs);

      for (auto idx = 0; idx < num_attribs; idx++) {
         int size;
         unsigned int gl_type;
         char name[513];

         GL_VERIFY(glGetActiveAttrib(_.id_, idx, 512, nullptr, &size, &gl_type, name));
         auto location = glGetAttribLocation(_.id_, name);
         if (location == -1) {
            utils::log(utils::LOG_WARN, "attribute '%s' location unknown\n", name);
         }
         else {
            assert(idx == location);
         }

         GL_CHECK();

         auto type = gl_to_value_type(gl_type);
         auto it = find_attrib_with_name(name);
         if (it == old_attribs.end()) {
            _.attribs_.push_back({ name, location, size, type });
         }
         else {
            it->reset(location, size, type);
            _.attribs_.push_back(*it);
            old_attribs.erase(it);
         }
      }

      // make sure we keep handles to the old uniforms so existing handles dont just break
      for (auto & u : old_uniforms) {
         u.reset();
         _.uniforms_.push_back(u);
      }
      for (auto & a : old_attribs) {
         a.reset();
         _.attribs_.push_back(a);
      }
   }

   uniform program::uniform(std::string const & name) {
      for (auto & u : state_->uniforms_) {
         if (u.name() == name) return u;
      }

      // not found, create and start tracking an invalid uniform
      auto new_uniform = glpp::uniform{ name };
      state_->uniforms_.push_back(new_uniform);
      return new_uniform;
   }

   attrib program::attrib(std::string const & name) {
      for (auto & a : state_->attribs_) {
         if (a.name() == name) return a;
      }

      // not found, create and start tracking an invalid attrib
      auto new_attrib = glpp::attrib{ name };
      state_->attribs_.push_back(new_attrib);
      return new_attrib;
   }

   void program::use() const {
      GL_VERIFY(glUseProgram(state_->id_));
   }

   void program::destroy() {
      state_->destroy();
   }

   void program::attach(shader s) {
      GL_VERIFY(glAttachShader(state_->id_, s.id()));
      state_->shaders_.push_back(std::move(s));
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
      for (auto & s : state_->shaders_) {
         auto log = s.compile_log();
         if (log.length() == 0) continue;

         logs += type_to_string(s);
         logs += log + "\n";
      }

      return logs;
   }

   pass_t program::pass() {
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

   dim_t window::window_dims() const {
      int width, height;
      glfwGetWindowSize(ctx_.impl_->window_, &width, &height);
      return{ width, height };
   }

   dim_t window::frame_buffer_dims() const {
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
            glpp::from_glfw_key(key), 
            scancode, 
            glpp::from_glfw_key_action(action), 
            glpp::from_glfw_key_mods(mods));
      }
   }

   context::context(key_callback_t key_handler)
   : win_(*this), impl_(nullptr)
   {
      if (!initialized_) { throw error("runtime not initialised - call glpp::init() first"); }

      // some more info on extensions: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-12-opengl-extensions/
      // glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1); 
      // glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
      // GL_ARB_framebuffer_object available in GL ES 2.0?

#ifdef USE_OPENGL_ES_2
      glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
      glfwWindowHint(GLFW_SAMPLES, 8);
      glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

      GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
      if (!window) {
         throw error("could not create GLFW window or context");
      }

      glfwMakeContextCurrent(window);

      window_key_callbacks_.set(window, *this, key_handler);
      glfwSetKeyCallback(window, key_callback);

#ifdef WIN32
      assert(extensionEnabled("GL_ANGLE_framebuffer_multisample"));
#endif

      impl_.reset(new impl{ window });
   }

   context::~context() {
      destroy();
   }

   std::string context::info(bool with_extensions) const {
      std::string buf;
      buf += "GLFW version        : "; buf += glfwGetVersionString();
      buf += "\nGL_VERSION          : "; buf += (const char *)glGetString(GL_VERSION);
      buf += "\nGL_VENDOR           : "; buf += (const char *)glGetString(GL_VENDOR);
      buf += "\nGL_RENDERER         : "; buf += (const char *)glGetString(GL_RENDERER);
#ifdef GL_SHADING_LANGUAGE_VERSION
      buf += "\nGL_SHADING_LANGUAGE_VERSION : "; buf += (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
#endif

      if (with_extensions) {
         std::vector<std::string> extensions;
         auto es = glGetString(GL_EXTENSIONS);
         for (auto i = 0, iTokBegin = 0; es[i]; i++) {
            auto endOfToken = es[i + 1] == ' ' || es[i + 1] == '\0';
            if (!endOfToken) continue;

            auto token = std::string{ es + iTokBegin, es + i + 1 };
            auto tokenIsEmpty = std::string::npos == token.find_last_not_of(" \t");

            if (!tokenIsEmpty)
               extensions.push_back(token);

            iTokBegin = i + 2;
         }

         buf += "\nGL EXTENSIONS [" + std::to_string(extensions.size()) + "]:";
         for (auto e : extensions) buf += "\n - " + e;
      }

      return buf;
   }

   void context::destroy() {
      if (impl_ && impl_->window_) glfwDestroyWindow(impl_->window_);
   }


   //
   // class sprite_sheet
   //

   sprite_sheet::sprite_sheet(glpp::texture_t texture, std::vector<frame_ref> frames)
      : texture_(texture) {
      for (auto & s : frames) {
         if (s.dimensions.x > max_frame_width_) max_frame_width_ = s.dimensions.x;
         if (s.dimensions.y > max_frame_height_) max_frame_height_ = s.dimensions.y;

         frames_.push_back(s);
      }

      frame_count_ = frames_.size();
   }

   namespace {
      std::vector<sprite_sheet::frame_ref> calculate_frames(glpp::texture_t const & texture, int frame_x, int frame_y) {
         auto tex_dims = texture.dims();
         std::vector<sprite_sheet::frame_ref> frames;
         for (auto y = 0; y < tex_dims.y; y += frame_y) {
            for (auto x = 0; x < tex_dims.x; x += frame_x) {
               // tex coords are from top==0, 2d graphics use bottom==0
               auto frame_lower_bound = (tex_dims.y)-(y + frame_y);

               assert(frame_lower_bound >= 0 && "calculated frame lower bound is < 0");
               assert((frame_lower_bound + frame_y) <= tex_dims.y && "calculated frame upper bound is > texture height");

               frames.push_back({ { x, frame_lower_bound }, { frame_x, frame_y } });
            }
         }
         return frames;
      }
   }

   sprite_sheet::sprite_sheet(glpp::texture_t texture, int frame_x, int frame_y)
      : sprite_sheet(texture, calculate_frames(texture, frame_x, frame_y)) {
   }

   sprite_sheet::sprite_sheet(glpp::texture_t texture)
      : texture_(texture)
      , max_frame_width_(texture.dims().x)
      , max_frame_height_(texture.dims().y) {
      frames_.push_back(frame_ref{ { 0, 0 }, { max_frame_width_, max_frame_height_ } });
      frame_count_ = frames_.size();
   }


   //
   // class animation_t
   //

   animation_t::animation_t(sprite_sheet const & frames, std::initializer_list<int> indices)
      : frames_(frames)
      , indices_(std::begin(indices), std::end(indices))
   {}

   glpp::texture_t animation_t::texture() const {
      return frames_.texture();
   }

   std::size_t animation_t::frame_count() const {
      return indices_.size();
   }

   sprite_sheet::frame_ref const & animation_t::frame(std::size_t idx) const {
      return frames_.frame(indices_.at(idx));
   }


   //
   // class sprite_t
   //

   struct sprite_t::state {
      state(std::initializer_list<animation_t> & animations)
         : animations_(std::begin(animations), std::end(animations))
      {}

      std::vector<animation_t> animations_;
   };

   sprite_t::sprite_t(std::initializer_list<animation_t> animations)
      : state_(std::make_shared<state>(animations))
   {}

   sprite_t::~sprite_t() {
   }

   std::size_t sprite_t::animation_count() const {
      return state_->animations_.size();
   }

   animation_t const & sprite_t::animation(std::size_t idx) const {
      return state_->animations_.at(idx);
   }


   //
   // class sprite_cursor_t
   //

   sprite_cursor_t::sprite_cursor_t(sprite_t sprite)
      : sprite_(sprite)
      , time_per_frame_(.1)
      , time_acc_(.0)
      , current_animation_idx_(0)
      , current_frame_idx_(0)
      , current_animation_(&sprite_.animation(current_animation_idx_))
   { }

   void sprite_cursor_t::advance(double time_since_last) {
      time_acc_ += time_since_last;
      while (time_acc_ > time_per_frame_) {
         set_idx(current_frame_idx_ + 1);
         time_acc_ -= time_per_frame_;
      }
   }

   std::size_t sprite_cursor_t::current_animation_idx() const {
      return current_animation_idx_;
   }

   void sprite_cursor_t::set_animation_idx(std::size_t state) {
      if (state == current_animation_idx_) return;
      current_animation_idx_ = state;
      current_frame_idx_ = 0;
      current_animation_ = &sprite_.animation(current_animation_idx_);
   }

   animation_t const & sprite_cursor_t::current_animation() const {
      return *current_animation_;
   }

   std::size_t sprite_cursor_t::idx() const {
      return current_frame_idx_;
   }

   void sprite_cursor_t::set_idx(std::size_t idx) {
      current_frame_idx_ = idx;
      current_frame_idx_ %= current_animation().frame_count();
   }

   sprite_sheet::frame_ref const & sprite_cursor_t::current_frame() const {
      return current_animation().frame(current_frame_idx_);
   }

   std::size_t sprite_cursor_t::current_frame_count() const {
      return current_animation().frame_count();
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
         throw glpp::error(message);
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

   // TODO: ensure program is bound
   void set_uniform(int location, glm::mat4 const & mat) { GL_VERIFY(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
   void set_uniform(int location, glm::mat3 const & mat) { GL_VERIFY(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
   void set_uniform(int location, glm::mat2 const & mat) { GL_VERIFY(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
   void set_uniform(int location, glm::vec4 const & vec) { GL_VERIFY(glUniform4f(location, vec.x, vec.y, vec.z, vec.w)); }
   void set_uniform(int location, glm::vec3 const & vec) { GL_VERIFY(glUniform3f(location, vec.x, vec.y, vec.z)); }
   void set_uniform(int location, glm::vec2 const & vec) { GL_VERIFY(glUniform2f(location, vec.x, vec.y)); }
   void set_uniform(int location, glm::ivec4 const & vec) { GL_VERIFY(glUniform4i(location, vec.x, vec.y, vec.z, vec.w)); }
   void set_uniform(int location, glm::ivec3 const & vec) { GL_VERIFY(glUniform3i(location, vec.x, vec.y, vec.z)); }
   void set_uniform(int location, glm::ivec2 const & vec) { GL_VERIFY(glUniform2i(location, vec.x, vec.y)); }
   void set_uniform(int location, float f) { GL_VERIFY(glUniform1f(location, f)); }
   void set_uniform(int location, int i) { GL_VERIFY(glUniform1i(location, i)); }
   void set_uniform(int location, texture_unit_t tex) { GL_VERIFY(glUniform1i(location, tex.id)); }

   //void set_uniform(GLint location, GLuint i) { glUniform1ui(location, i); }
}