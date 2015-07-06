#include <glpp/glpp.h>

#ifdef _MSC_VER
#  include <glpp/gles2.h>
   namespace gl { using namespace gles2; }
#else
#  include <glpp/gl2.h>
   namespace gl { using namespace gl2; }
#endif

#include <glpp/ui.h>
#include <glpp/utils.h>

void ui_demo() {
   auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
      if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
         ctx.win().set_should_close();
   };

   glpp::init();
   glpp::context context(key_handler);

   auto ui_context = glpp::imgui::init(context);

   while (!context.win().closing()) {
      gl::clear_color(0.f, 0.f, 0.f, 1.f);
      gl::clear(gl::clear_buffer_flags_t::color_buffer_bit);

      glpp::imgui::new_frame(ui_context);

      static bool show_test_window = true;
      ImGui::ShowTestWindow(&show_test_window);

      ImGui::Text("Hello, world!");
      if (ImGui::Button("test button label")) {
         utils::log(utils::LOG_INFO, "BUTTON PRESSED\n");
      }

      glpp::imgui::render(ui_context);
      context.win().swap();
   }

   //glpp::shutdown();
}