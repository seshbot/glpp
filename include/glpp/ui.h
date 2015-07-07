#ifndef GLPP_UI__H
#define GLPP_UI__H

#include <string>
#include <memory>
#include <glpp/glpp.h>
#include <glpp/imgui.h>

namespace glpp {
namespace imgui {
   struct ui_context_t {
   public:
      ui_context_t(ui_context_t &&) = default;
      ui_context_t & operator=(ui_context_t &&) = default;
      ~ui_context_t();

   private:
      struct impl;
      std::unique_ptr<impl> impl_;

      ui_context_t(std::unique_ptr<impl> i);

      friend ui_context_t init(context & ctx, glpp::texture_unit_t ui_texture_unit);
      friend void new_frame(ui_context_t & ui_context);
      friend void render(ui_context_t & ui_context);
   };

   ui_context_t init(glpp::context & ctx, glpp::texture_unit_t ui_texture_unit);
   void new_frame(ui_context_t & ui_context);
   void render(ui_context_t & ui_context);
}
}

#endif // #ifndef GLPP_UI__H