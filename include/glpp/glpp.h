#ifndef PCX_GL__H
#define PCX_GL__H

// TODO:
// - create VAO abstraction that simulates VAOs where not available
// - allow multiple contexts
//   - allow glpp:: invocations via contexts? context.functions()?
// - thread safety (use TLS?)

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <memory>

#include <glpp/input.h>
#include <glpp/types.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#ifndef _MSC_VER
   // for now lets always enable debugging on non-windows
#  define _DEBUG 1
#endif

#if defined(_DEBUG) && !defined(GLPP_NO_DEBUG_OPENGL)
#  ifndef GLPP_DEBUG_OPENGL
#    define GLPP_DEBUG_OPENGL 1
#  endif
#endif

#ifdef GLPP_DEBUG_OPENGL
#  define GL_VERIFY(stmt) do { stmt; glpp::checkOpenGLError(#stmt, __FUNCTION__, __FILE__, __LINE__); } while (0)
#  define GL_CHECK() do { glpp::checkOpenGLError(__FUNCTION__, __FILE__, __LINE__); } while (0)
#  define GL_IGNORE(stmt) do { GL_CHECK(); stmt; glGetError(); } while (0)
#else
#  define GL_VERIFY(stmt) stmt
#  define GL_CHECK()
#  define GL_IGNORE(stmt) stmt
#endif


namespace glpp {
	using id_t = gl::uint_t;

   struct dim_t { int x; int y; };
   inline bool operator==(dim_t const & d1, dim_t const & d2) { return d1.x == d2.x && d1.y == d2.y; }
   inline bool operator!=(dim_t const & d1, dim_t const & d2) { return !(d1 == d2); }

   //const char * openGlErrorString(id_t err);
   void checkOpenGLError(const char* function, const char* file, int line);
   void checkOpenGLError(const char* stmt, const char* function, const char* file, int line);

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
   
   enum class texture_format_t {
      RGB,
      RGBA,
#ifdef _MSC_VER
      BGRA,
#endif
      DEPTH,
   };

   class image_t {
   public:
      image_t(image_t const &) = default;
      image_t & operator=(image_t const &) = default;
      image_t(image_t &&) = default;
      image_t & operator=(image_t &&) = default;
      explicit image_t(std::string const & filename);
      ~image_t();

      int width() const;
      int height() const;
      int channels() const;
      unsigned char * data() const;

   private:
      struct impl;
      std::shared_ptr<const impl> impl_;
   };

   class texture_t {
   public:
      using id_type = uint32_t;

      texture_t(image_t image);
      texture_t(dim_t const & dims, texture_format_t format = texture_format_t::RGBA);

      id_type id() const { return state_->id_; }

      dim_t const & dims() const { return state_->dims_; }

      // TODO: this belongs in a image_file_t class or something
      void save_current_framebuffer(std::string const & filename) const;

      void bind() const;

   private:
      friend class cube_map_texture_t;

      enum Target {
         TEXTURE_2D,
         TEXTURE_CUBE_MAP,
      };
      static unsigned int to_gl(Target target);

      struct state {
         state(image_t image, Target target);
         state(dim_t const & dims, Target target, texture_format_t format);
         ~state();
         id_type id_;
         dim_t dims_;
         texture_format_t format_;
      };

      std::shared_ptr<state> state_;

      static void save_current_framebuffer_(state const & s, std::string const & filename);
   };

   class cube_map_texture_t {
   public:
      using id_type = texture_t::id_type;

      cube_map_texture_t(int side_length, texture_format_t format = texture_format_t::RGBA);

      id_type id() const { return state_->id_; }
      dim_t const & face_dims() const { return state_->dims_; }

      void bind() const;

      // TODO: these are clunky, put this on the framebuffer?
      void save_current_framebuffer(std::string const & filename) const;

   private:
      std::shared_ptr<texture_t::state> state_;
   };

   struct texture_unit_t {
      int id;
      void activate() const;
   };

   class frame_buffer_t {
   public:
      frame_buffer_t(frame_buffer_t const &) = delete;
      frame_buffer_t & operator=(frame_buffer_t const &) = delete;

      frame_buffer_t(texture_t const & tex); // implied samples == 0
      frame_buffer_t(cube_map_texture_t const & tex);
      frame_buffer_t(glpp::dim_t dims, unsigned samples = 0);
      ~frame_buffer_t();

      id_t id() const { return fbo_id_; }
      glpp::dim_t dims() const { return dims_; }

      enum CubeFace { // this enum is aligned with GL_TEXTURE_CUBE_MAP_POSITIVE_*
         POSITIVE_X = 0x8515,
         NEGATIVE_X,
         POSITIVE_Y,
         NEGATIVE_Y,
         POSITIVE_Z,
         NEGATIVE_Z,
      };
      enum BindTarget { Read, Draw, ReadDraw };
      void bind(BindTarget target = ReadDraw) const;
      void bind(CubeFace face, BindTarget target = ReadDraw) const;
      void unbind(BindTarget target = ReadDraw) const;

      void blit_to_draw_buffer() const;
      void blit_to_screen() const;

      void validate() const { check_fbo(); }

   private:
      void bind_(BindTarget target = ReadDraw) const;
      void check_fbo() const;

      texture_t::id_type cube_map_texture_id_ = 0;
      dim_t dims_;
      unsigned samples_;
      id_t fbo_id_;
      id_t colour_rbo_id_; // render buffer MSAA colour data
      id_t depth_rbo_id_; // render buffer for stencil/depth
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

   template <typename T> ValueType value_type();
   template <> ValueType value_type<int8_t>();
   template <> ValueType value_type<uint8_t>();
   template <> ValueType value_type<int16_t>();
   template <> ValueType value_type<uint16_t>();
   template <> ValueType value_type<int32_t>();
   template <> ValueType value_type<uint32_t>();
   template <> ValueType value_type<float>();
   template <> ValueType value_type<const int8_t>();
   template <> ValueType value_type<const uint8_t>();
   template <> ValueType value_type<const int16_t>();
   template <> ValueType value_type<const uint16_t>();
   template <> ValueType value_type<const int32_t>();
   template <> ValueType value_type<const uint32_t>();
   template <> ValueType value_type<const float>();
   template <> ValueType value_type<glm::ivec2>();
   template <> ValueType value_type<glm::ivec3>();
   template <> ValueType value_type<glm::ivec4>();
   template <> ValueType value_type<glm::vec2>();
   template <> ValueType value_type<glm::vec3>();
   template <> ValueType value_type<glm::vec4>();
   template <> ValueType value_type<glm::mat2>();
   template <> ValueType value_type<glm::mat3>();
   template <> ValueType value_type<glm::mat4>();
   template <> ValueType value_type<texture_unit_t>();


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
      void set(std::vector<glm::vec4> const & vec_vec);
      void set(std::vector<glm::mat4> const & mat_vec);
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

   // TODO: specify buffers as arrays of user-defined structures

   class static_array_t {
   public:
      template <typename T, unsigned N>
      static_array_t(const T(&data)[N]) 
         : static_array_t((void*)data, value_type<T>(), N, N * sizeof(T)) {}

      template <typename T>
      static_array_t(T *data, std::size_t elem_count)
         : static_array_t((void*)data, value_type<T>(), elem_count, elem_count * sizeof(T)) {}

      void * const data() const { return data_; }
      ValueType data_type() const { return data_type_; }
      std::size_t size() const { return byte_size_; }
      std::size_t elem_count() const { return elem_count_; }

      friend bool operator==(static_array_t const & lhs, static_array_t const & rhs) { return lhs.data() == rhs.data(); }
      friend bool operator!=(static_array_t const & lhs, static_array_t const & rhs) { return !(lhs == rhs); }

   private:
      static_array_t(void* data, ValueType data_type, std::size_t elem_count, std::size_t byte_size)
         : data_(data), data_type_(data_type), elem_count_(elem_count), byte_size_(byte_size) { }

      void * const data_;
      ValueType data_type_;
      std::size_t elem_count_;
      std::size_t byte_size_;
   };

   class buffer_t {
   public:
      enum class Target { ArrayBuffer, IndexBuffer };
      enum class Usage { Static, Stream, Dynamic };

      buffer_t(Usage usage = Usage::Static);
      buffer_t(Target target, static_array_t data, Usage usage = Usage::Static);

      // convenience: assume array buffer
      buffer_t(static_array_t vertex_data, Usage usage = Usage::Static);
      // convenience: assume array and index buffers
      buffer_t(static_array_t vertex_data, static_array_t indices, Usage usage = Usage::Static);

      void update(Target target, static_array_t data);

      // convenience: assume array buffer
      void update(static_array_t vertex_data);
      // convenience: assume array and index buffers
      void update(static_array_t vertex_data, static_array_t indices);

      std::size_t vertex_count() const { return state_->vertex_count_; }
      std::size_t vertex_buffer_size() const { return state_->vertex_buffer_size_; }

      id_t vertex_buffer_id() const { return state_->vertex_id_; }
      id_t index_buffer_id() const { return state_->index_id_; }

      bool has_vertex_data() const { return state_->vertex_id_ != 0; }
      bool has_index_data() const { return state_->index_id_ != 0; }
      ValueType index_data_type() const { return state_->index_data_type_; }
      unsigned index_count() const { return state_->index_count_; }

      void bind() const;
      void unbind() const;

      friend bool operator==(buffer_t const & lhs, buffer_t const & rhs) { return lhs.vertex_buffer_id() == rhs.vertex_buffer_id() && lhs.index_buffer_id() == rhs.index_buffer_id(); }
      friend bool operator!=(buffer_t const & lhs, buffer_t const & rhs) { return !(lhs == rhs); }

   private:
      struct state {
         state(Usage usage);
         state(void* vertex_data, std::size_t vertex_count, std::size_t vertex_byte_size, Usage usage);
         state(void* vertex_data, std::size_t vertex_count, std::size_t vertex_byte_size, void* index_data, unsigned index_count, std::size_t index_byte_size, ValueType index_data_type, Usage usage);
         state(void* index_data, unsigned index_count, std::size_t index_byte_size, ValueType index_data_type, Usage usage);
         ~state();

         void assign(void* vertex_data, std::size_t vertex_count, std::size_t vertex_byte_size);
         void assign(void* vertex_data, std::size_t vertex_count, std::size_t vertex_byte_size, void* index_data, unsigned index_count, std::size_t index_byte_size, ValueType index_data_type);
         void assign(void* index_data, unsigned index_count, std::size_t index_byte_size, ValueType index_data_type);

         Usage usage_;
         id_t vertex_id_ = 0;
         id_t index_id_ = 0;
         std::size_t vertex_count_ = 0;
         std::size_t vertex_buffer_size_ = 0;
         ValueType index_data_type_ = Unknown;
         unsigned index_count_ = 0;
      };

      std::shared_ptr<state> state_;
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

   struct mapped_buffer_t {
      using attrib_container = std::vector<attrib_info>;

      buffer_t buffer;
      attrib_container attribs;
   };

   class program;
   class buffer_attrib_mappings_t {
   public:
      buffer_attrib_mappings_t();

      buffer_attrib_mappings_t push_attrib(std::string attrib_name, unsigned count);
      buffer_attrib_mappings_t skip_bytes(unsigned bytes);

      mapped_buffer_t map_buffer(program & prg, buffer_t buffer) const;

   private:
      struct slice_info {
         slice_info(std::string const & n, int elem_sz, unsigned elem_cnt) : attrib_name(n), elem_size(elem_sz), count(elem_cnt) {}
         std::string attrib_name; // "" means skip count bytes
         int elem_size;
         unsigned count;
      };

      struct state {
         std::vector<slice_info> slices_;
      };

      std::shared_ptr<state> state_;
   };

   mapped_buffer_t map_buffer(program & prg, buffer_t buffer, buffer_attrib_mappings_t const & spec);

   enum class DrawMode {
      Points,
      Lines, LineLoop,
      Triangles, TriangleStrip, TriangleFan,
   };

   unsigned num_vertices(mapped_buffer_t const & b);
   void bind(mapped_buffer_t const & b);
   void unbind(mapped_buffer_t const & b);
   void draw(mapped_buffer_t const & b, DrawMode mode);
   void draw(mapped_buffer_t const & b, DrawMode mode, unsigned first, unsigned count);

   class program;
   class pass_t {
      using uniform_action_t = std::function < void(uniform & u) > ;

   public:
      struct render_batch_callback {
         virtual ~render_batch_callback() {}

         virtual bool prepare_next(program & p) const = 0;
      };

      ~pass_t();

      // create a pass that uses this pass as a parent - primitives added to child will not affect parent
      pass_t extend();

      // TODO: set_attribute() calls glAttribxx()

      template <typename T>
      pass_t & set_uniform(std::string const & name, T val) {
         return set_uniform_action(name, [val](glpp::uniform & u){ u.set(val); });
      }

      pass_t & set_uniform_action(std::string const & name, uniform_action_t action);

      pass_t & set_texture_unit(texture_unit_t u, texture_t tex);

      pass_t & with(mapped_buffer_t buffer_desc);
      pass_t & with(buffer_t buffer, buffer_attrib_mappings_t const & buffer_spec);

      //pass_t & validate_attribs(bool validate = true);

      pass_t & draw(DrawMode mode);
      pass_t & draw(DrawMode mode, unsigned first, unsigned count);

      pass_t & draw_batch(render_batch_callback const & cb, DrawMode mode);
      pass_t & draw_batch(render_batch_callback const & cb, DrawMode mode, unsigned first, unsigned count);

   private:
      glpp::uniform uniform(std::string const & name);
      buffer_t const * index_buffer_() const;
      unsigned calc_draw_count_() const;
      void prepare_draw_(); // bind program, uniforms and textures
      void unprepare_draw_();
      void draw_(DrawMode mode, unsigned first, unsigned count); // must call prepare_draw_ first

      friend class program;
      pass_t(program prg);

      struct extend_tag {};
      pass_t(extend_tag, pass_t & parent);

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

      glpp::uniform uniform(std::string const & name);
      glpp::attrib attrib(std::string const & name);

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
         std::vector<glpp::uniform> uniforms_;
         std::vector<glpp::attrib> attribs_;
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
      dim_t window_pos() const;
      dim_t frame_buffer_dims() const;

      void set_fullscreen(bool enable);
      bool is_fullscreen() const;

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

      std::vector<std::string> extensions() const;
      std::string info(bool with_extensions = true) const;

      void destroy(); // optional, will be invoked by dtor

      window const & win() const { return *win_; }
      window & win() { return *win_; }

      void * platform_handle() const;

   private:
      void recreate_window(bool fullscreen);

      key_callback_t key_handler_;
      bool fullscreen_;

      friend class window;
      std::unique_ptr<window> win_;

      struct impl;
      std::unique_ptr<impl> impl_;
	};

   void init();
   void shutdown();

   double get_time();

   void set_uniform(int location, std::vector<glm::vec4> const & vecs);
   void set_uniform(int location, std::vector<glm::mat4> const & mats);
   void set_uniform(int location, glm::mat4 const * mats, unsigned count);
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

      sprite_sheet(glpp::texture_t texture, std::vector<frame_ref> frames);
      sprite_sheet(glpp::texture_t texture, int frame_x, int frame_y);
      sprite_sheet(glpp::texture_t texture);

      glpp::texture_t texture() const { return texture_; }

      std::size_t frame_count() const { return frame_count_; }
      frame_ref const & frame(std::size_t idx) const { return frames_.at(idx); }
      glm::ivec2 const & frame_pos(int idx) const { return frames_.at(idx).position; }
      glm::ivec2 const & frame_dims(int idx) const { return frames_.at(idx).dimensions; }

   private:
      glpp::texture_t texture_;

      int max_frame_width_;
      int max_frame_height_;
      std::size_t frame_count_;

      std::vector<frame_ref> frames_;
   };

   class sprite_animation_t {
   public:
      sprite_animation_t(sprite_sheet const & frames, std::initializer_list<int> indices);

      glpp::texture_t texture() const;
      std::size_t frame_count() const;

      sprite_sheet::frame_ref const & frame(std::size_t idx) const;

   private:
      sprite_sheet const & frames_;
      std::vector<int> indices_;
   };

   class sprite_t {
   public:
      sprite_t(std::initializer_list<sprite_animation_t> animations);
      ~sprite_t();

      std::size_t animation_count() const;
      sprite_animation_t const & animation(std::size_t idx) const;

   private:
      struct state;
      std::shared_ptr<state> state_;
   };

   class sprite_cursor_t {
   public:
      sprite_cursor_t(sprite_t sprite);

      std::size_t current_animation_idx() const;
      void set_animation_idx(std::size_t state);

      sprite_animation_t const & current_animation() const;

      std::size_t idx() const;
      void set_idx(std::size_t idx);

      sprite_sheet::frame_ref const & current_frame() const;
      std::size_t current_frame_count() const;

      void advance(double time_since_last);

   private:
      sprite_t sprite_;

      double time_per_frame_;
      double time_acc_;
      std::size_t current_animation_idx_;
      std::size_t current_frame_idx_;
      sprite_animation_t const * current_animation_;
   };

   //
   // debug UI
   //

   program make_debug_ui_program();

   // buffer [x:float, y:float, z:float, r:ubyte, g: ubyte, b: ubyte, a: ubyte]
   // hint: render using orthographic projection of screen size
   buffer_t build_debug_text_buffer(
      std::string const & text, float leftmost = 0., float topmost = 0., float scale_factor = 1.);


   glm::mat4 make_debug_text_projection(int viewport_width, int viewport_height, int left_pad = 0, int top_pad = 0, float scale = 1.f);

   // e.g.: 
   // auto proj_tx = glm::ortho(0.f, 800.f, 600.f, 0.);
   // auto model_tx = glm::translate(glm::vec3{30.f, 20.f, 0.});
   // auto pass = make_debug_text_pass("xxx", prg, proj_tx * model_tx);
   // pass.draw(glpp::DrawMode::Triangles);
   pass_t make_debug_text_pass(std::string const & text, program & prg, glm::mat4 const & mvp);
}


#endif
