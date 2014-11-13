#ifndef PCX_GL__H
#define PCX_GL__H

#include <string>
#include <stdexcept>
#include <functional>
#include <memory>

#include "input.h"

namespace gl {
   struct error : public std::runtime_error {
      error() : std::runtime_error("") { }
      error(std::string message) : std::runtime_error(message) { }
   };

	using id_t = unsigned int;
	using name_t = std::string;

   // attr types: GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, or GL_FLOAT_MAT4
   // attr { name, idx, location, size, type }

	class program {
	public:
		program(program && other);
		program & operator=(program && other);

      id_t id() const { return id_; }

	private:
      program(id_t id);
      ~program();
      static program create();

      program(program const &) = delete;
      program & operator=(program const &) = delete;

      id_t id_;
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
