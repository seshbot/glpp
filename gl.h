#ifndef PCX_GL__H
#define PCX_GL__H

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <iterator>
#include <memory>

#include "input.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>


#ifdef DEBUG
   void checkOpenGLError(const char* function, const char* file, int line);
   void checkOpenGLError(const char* stmt, const char* function, const char* file, int line);

#  define GL_VERIFY(stmt) do { stmt; checkOpenGLError(#stmt, __FUNCTION__, __FILE__, __LINE__); } while (0)
#  define GL_CHECK() do { checkOpenGLError(__FUNCTION__, __FILE__, __LINE__); } while (0)
#  define GL_IGNORE(stmt) do { GL_CHECK(); stmt; glGetError(); } while (0)
#else
#  define GL_VERIFY(stmt) stmt
#  define GL_CHECK()
#  define GL_IGNORE(stmt) stmt
#endif


namespace gl {
	using id_t = unsigned int;

   struct dim_t { int x; int y; };
   inline bool operator==(dim_t const & d1, dim_t const & d2) { return d1.x == d2.x && d1.y == d2.y; }
   inline bool operator!=(dim_t const & d1, dim_t const & d2) { return !(d1 == d2); }

   const char * openGlErrorString(id_t err);

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


   class texture_t {
   public:
      using id_type = uint32_t;

      texture_t(std::string const & filename);
      texture_t(int width, int height);

      id_type id() const { return state_->id_; }

      int width() const { return state_->width_; }
      int height() const { return state_->height_; }

      void save(std::string const & filename) const;

      void bind() const;

   private:
      struct state {
         state(std::string const & filename);
         state(int width, int height);
         ~state();
         id_type id_;
         int width_;
         int height_;
      };

      std::shared_ptr<state> state_;
   };

   struct texture_unit_t {
      int id;
      void activate() const;
   };

   class frame_buffer_t {
   public:
      frame_buffer_t(frame_buffer_t const &) = delete;
      frame_buffer_t & operator=(frame_buffer_t const &) = delete;

      frame_buffer_t(gl::dim_t dims);
      ~frame_buffer_t();

      id_t id() const { return fbo_id_; }
      gl::dim_t dims() const { return dims_; }

      void bind() const;
      void unbind() const;

      gl::texture_t & texture() { return colour_buffer_; }

   private:
      void check_fbo() const;

      dim_t dims_;
      id_t fbo_id_;
      id_t rbo_id_; // render buffer for stencil/depth
      gl::texture_t colour_buffer_;
   };

   id_t current_frame_buffer();
   void bind_frame_buffer(id_t new_fbo);

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
      void set(glm::ivec4 const & vec);
      void set(glm::ivec3 const & vec);
      void set(glm::ivec2 const & vec);
      void set(texture_unit_t tex);

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

   class static_array_t {
   public:
      template <typename T, unsigned N>
      static_array_t(const T(&data)[N]) 
         : static_array_t((void*)data, N * sizeof(T)) {}

      static_array_t(void* data, std::size_t byte_size)
         : state_(std::make_shared<state>(data, byte_size)) { }

      void * const data() const { return state_->data_; }
      std::size_t size() const { return state_->byte_size_; }

      friend bool operator==(static_array_t const & lhs, static_array_t const & rhs) { return lhs.data() == rhs.data(); }
      friend bool operator!=(static_array_t const & lhs, static_array_t const & rhs) { return !(lhs == rhs); }

   private:
      struct state {
         state(void* d, std::size_t sz) : data_(d), byte_size_(sz) { }
         void * const data_; std::size_t byte_size_;
      };

      std::shared_ptr<state const> state_;
   };

   class buffer_t {
   public:
      struct idx_array_t {
         template <unsigned N>
         idx_array_t(const uint32_t(&data)[N]) : idx_array_t((void*)data, N, ValueType::UInt, N * sizeof(uint32_t)) {}
         template <unsigned N>
         idx_array_t(const uint16_t(&data)[N]) : idx_array_t((void*)data, N, ValueType::UShort, N * sizeof(uint16_t)) {}
         template <unsigned N>
         idx_array_t(const uint8_t(&data)[N]) : idx_array_t((void*)data, N, ValueType::UByte, N * sizeof(uint8_t)) {}
         idx_array_t(void* data, unsigned count, ValueType data_type, std::size_t byte_size);

         void* data;
         unsigned count;
         ValueType data_type;
         std::size_t byte_size;
      };

      buffer_t(static_array_t vertex_data);
      buffer_t(static_array_t vertex_data, idx_array_t indices);

      std::size_t vertex_buffer_size() const { return state_->vertex_buffer_size_; }

      id_t vertex_buffer_id() const { return state_->vertex_id_; }
      id_t index_buffer_id() const { return state_->index_id_; }

      bool has_index_data() const { return state_->index_id_ != 0; }
      ValueType index_data_type() const { return state_->index_data_type_; }
      unsigned index_count() const { return state_->index_count_; }

      void use() const;

      friend bool operator==(buffer_t const & lhs, buffer_t const & rhs) { return lhs.vertex_buffer_id() == rhs.vertex_buffer_id() && lhs.index_buffer_id() == rhs.index_buffer_id(); }
      friend bool operator!=(buffer_t const & lhs, buffer_t const & rhs) { return !(lhs == rhs); }

   private:
      struct state {
         state(void* vertex_data, std::size_t vertex_byte_size);
         state(void* vertex_data, std::size_t vertex_byte_size, void* index_data, unsigned index_count, std::size_t index_byte_size, ValueType index_data_type);
         ~state();

         id_t vertex_id_ = 0;
         id_t index_id_ = 0;
         std::size_t vertex_buffer_size_;
         ValueType index_data_type_ = Unknown;
         unsigned index_count_ = 0;
      };

      std::shared_ptr<state const> state_;
   };


   struct attrib_info {
      attrib_info(attrib attrib, unsigned count, unsigned stride_bytes = 0, unsigned offset_bytes = 0)
         : attrib(std::move(attrib)), count(count), stride_bytes(stride_bytes), offset_bytes(offset_bytes) {}

      // estimate block size (stride) based on attrib specifications
      unsigned calc_stride_bytes() const;

      attrib attrib;
      unsigned count;
      unsigned stride_bytes;
      unsigned offset_bytes;
   };

   struct buffer_spec_t {
      using attribs_type = std::vector<attrib_info>;

      buffer_t buffer;
      attribs_type attribs;
   };

   class program;
   class buffer_spec_builder_t {
   public:
      buffer_spec_builder_t(buffer_t buffer);

      buffer_spec_builder_t attrib(std::string attrib_name, unsigned count);
      buffer_spec_builder_t skip_bytes(unsigned bytes);

      buffer_spec_t build(program & prg) const;

   private:
      struct slice_info {
         std::string attrib_name; // "" means skip count bytes
         unsigned count;
      };

      struct state {
         state(buffer_t buffer) : buffer_(std::move(buffer)) {}

         buffer_t buffer_;
         std::vector<slice_info> slices_;
      };

      std::shared_ptr<state> state_;
   };

   buffer_spec_builder_t describe_buffer(buffer_t buffer);


   enum class DrawMode {
      Points,
      Lines, LineLoop,
      Triangles, TriangleStrip, TriangleFan,
   };

   unsigned num_vertices(buffer_spec_t const & b);
   void use(buffer_spec_t const & b);
   void draw(buffer_spec_t const & b, DrawMode mode);
   void draw(buffer_spec_t const & b, DrawMode mode, unsigned first, unsigned count);


   class program;
   class pass_t {
      using uniform_action_t = std::function < void(uniform & u) > ;

   public:
      struct render_callback {
         virtual ~render_callback() {}

         virtual bool prepare_next(program & p) const = 0;
      };

      ~pass_t();

      template <typename T>
      pass_t & set_uniform(std::string const & name, T val) {
         return set_uniform_action(name, [val](gl::uniform & u){ u.set(val); });
      }

      pass_t & set_uniform_action(std::string const & name, uniform_action_t action);

      pass_t & set_texture_unit(texture_unit_t u, texture_t tex);

      pass_t & with(buffer_spec_t buffer_spec);
      pass_t & with(buffer_spec_builder_t const & buffer_spec_builder);

      //pass_t & validate_attribs(bool validate = true);

      pass_t & draw(DrawMode mode);
      pass_t & draw(DrawMode mode, unsigned first, unsigned count);

      pass_t & draw_batch(render_callback const & cb, DrawMode mode);
      pass_t & draw_batch(render_callback const & cb, DrawMode mode, unsigned first, unsigned count);

   private:
      gl::uniform uniform(std::string const & name);
      void prepare_draw(); // bind program, uniforms and textures

      friend class program;
      pass_t(program prg);

      struct state;
      std::shared_ptr<state> state_;
   };

   template <>
   pass_t & pass_t::set_uniform<texture_t>(std::string const & name, texture_t val);


	class program {
   public:
      program(shader s1, shader s2);
      ~program();

      void reload(shader s1, shader s2);

      gl::uniform uniform(std::string const & name);
      gl::attrib attrib(std::string const & name);

      void use() const;

      void destroy();
      id_t id() const { return state_->id_; }
      void attach(shader s);

      std::string compile_logs() const;

      pass_t pass();

   private:
      program();

      void reload();

      struct state {
         state();
         ~state();
         void destroy();

         id_t id_;
         std::vector<shader> shaders_;
         std::vector<gl::uniform> uniforms_;
         std::vector<gl::attrib> attribs_;
      };

      std::shared_ptr<state> state_;
   };

   class context;

   class window {
   public:
      void set_should_close(bool close = true);
      bool closing() const;

      void swap();

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

   void set_uniform(int location, glm::mat4 const & mat);
   void set_uniform(int location, glm::mat3 const & mat);
   void set_uniform(int location, glm::mat2 const & mat);
   void set_uniform(int location, glm::vec4 const & vec);
   void set_uniform(int location, glm::vec3 const & vec);
   void set_uniform(int location, glm::vec2 const & vec);
   void set_uniform(int location, glm::ivec4 const & vec);
   void set_uniform(int location, glm::ivec3 const & vec);
   void set_uniform(int location, glm::ivec2 const & vec);
   void set_uniform(int location, float f);
   void set_uniform(int location, int i);
   void set_uniform(int location, texture_unit_t tex);
   //void set_uniform(GLint location, GLuint i);




   class sprite_sheet {
   public:
      struct frame_ref {
         glm::ivec2 position;
         glm::ivec2 dimensions;
      };

      sprite_sheet(gl::texture_t texture, std::initializer_list<frame_ref> frames)
         : texture_(texture) {
         for (auto & s : frames) {
            if (s.dimensions.x > max_frame_width_) max_frame_width_ = s.dimensions.x;
            if (s.dimensions.y > max_frame_height_) max_frame_height_ = s.dimensions.y;

            frames_.push_back(s);
         }

         frame_count_ = frames_.size();
      }

      sprite_sheet(gl::texture_t texture)
         : texture_(texture)
         , max_frame_width_(texture.width())
         , max_frame_height_(texture.height()) {
         frames_.push_back(frame_ref{ { 0, 0 }, { max_frame_width_, max_frame_height_ } });
         frame_count_ = frames_.size();
      }

      gl::texture_t texture() const { return texture_; }

      std::size_t frame_count() const { return frame_count_; }
      frame_ref const & frame(std::size_t idx) const { return frames_.at(idx); }
      glm::ivec2 const & frame_pos(int idx) const { return frames_.at(idx).position; }
      glm::ivec2 const & frame_dims(int idx) const { return frames_.at(idx).dimensions; }

   private:
      gl::texture_t texture_;

      int max_frame_width_;
      int max_frame_height_;
      std::size_t frame_count_;

      std::vector<frame_ref> frames_;
   };

   class animation_t {
   public:
      animation_t(sprite_sheet const & frames, std::initializer_list<int> indices)
         : frames_(frames)
         , indices_(std::begin(indices), std::end(indices)) {}

      gl::texture_t texture() const { return frames_.texture(); }
      std::size_t frame_count() const { return indices_.size(); }

      sprite_sheet::frame_ref const & frame(std::size_t idx) const { return frames_.frame(indices_.at(idx)); }

   private:
      sprite_sheet const & frames_;
      std::vector<int> indices_;
   };

   class sprite_t {
   public:
      sprite_t(std::initializer_list<animation_t> animations)
         : state_(std::make_shared<state>(animations))
      {}

      std::size_t animation_count() const { return state_->animations_.size(); }
      animation_t const & animation(std::size_t idx) const { return state_->animations_.at(idx); }

   private:
      struct state {
         state(std::initializer_list<animation_t> & animations)
            : animations_(std::begin(animations), std::end(animations))
         {}

         std::vector<animation_t> animations_;
      };

      std::shared_ptr<state> state_;
   };

   class sprite_cursor_t {
   public:
      sprite_cursor_t(sprite_t sprite)
         : sprite_(sprite)
         , time_per_frame_(.1)
         , time_acc_(.0)
         , current_animation_idx_(0)
         , current_frame_idx_(0)
         , current_animation_(&sprite_.animation(current_animation_idx_)) {}

      void advance(double time_since_last) {
         time_acc_ += time_since_last;
         while (time_acc_ > time_per_frame_) {
            set_idx(current_frame_idx_ + 1);
            time_acc_ -= time_per_frame_;
         }
      }

      std::size_t current_animation_idx() const { return current_animation_idx_; }
      void set_animation_idx(std::size_t state) {
         if (state == current_animation_idx_) return;
         current_animation_idx_ = state;
         current_frame_idx_ = 0;
         current_animation_ = &sprite_.animation(current_animation_idx_);
      }

      animation_t const & current_animation() const { return *current_animation_; }

      std::size_t idx() const { return current_frame_idx_; }
      void set_idx(std::size_t idx) { current_frame_idx_ = idx; current_frame_idx_ %= current_animation().frame_count(); }

      sprite_sheet::frame_ref const & current_frame() const { return current_animation().frame(current_frame_idx_); }
      std::size_t current_frame_count() const { return current_animation().frame_count(); }

   private:
      sprite_t sprite_;

      double time_per_frame_;
      double time_acc_;
      std::size_t current_animation_idx_;
      std::size_t current_frame_idx_;
      animation_t const * current_animation_;
   };
}


#endif
