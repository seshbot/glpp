#ifndef PCX_GL__H
#define PCX_GL__H

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <memory>

#include "input.h"

namespace gl {
	using id_t = unsigned int;
	using name_t = std::string;

   struct error : public std::runtime_error {
      error() : std::runtime_error("") { }
      error(std::string message) : std::runtime_error(message) { }
   };

   struct shader_compile_error : public error {
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

	class program {
   public:
      program(shader s);
      program(shader s1, shader s2);
      ~program();

		program(program && other);
		program & operator=(program && other);

      void use() const;

      void destroy();
      id_t id() const { return id_; }
      void attach(shader s);

      std::string compile_logs() const;

   private:
      program();
      program(program const &) = delete;
      program & operator=(program const &) = delete;

      id_t id_;
      std::vector<shader> shaders_;
   };

   class context;
   using key_callback_t = std::function < void(context &, Key, int, KeyAction, int) >; // key, scancode, action, mods

	class context {
   public:
      context(key_callback_t key_handler);
      context(context const &) = delete;
      context & operator=(context const &) = delete;
      ~context();

      std::string info() const;

      void destroy(); // optional, will be invoked by dtor

      void set_should_close(bool close = true);
      bool closing() const;

      void swap();

      struct dim_t { int x; int y; };
      dim_t window_dims() const;
      dim_t frame_buffer_dims() const;

   private:
      struct impl;
      std::unique_ptr<impl> impl_;
	};

   void init();
   void shutdown();
}


#endif
