#include <glpp/glpp.h>

#ifdef _MSC_VER
#  include <glpp/gles2.h>
   namespace gl { using namespace gles2; }
#else
#  include <glpp/gl2.h>
   namespace gl { using namespace gl2; }
#endif

#include <glm/gtx/transform.hpp>

#include <glpp/utils.h>

#ifdef DRAW_MANUALLY

static auto VERT_SHADER_SOURCE = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

attribute mediump vec2 pos;
attribute lowp vec4 col;

varying mediump vec3 frag_pos;
varying lowp vec4 frag_col;

void main() {
   gl_Position = vec4(pos, 0., 1.);
   frag_col = col;
}
)";

static auto FRAG_SHADER_SOURCE = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

varying lowp vec4 frag_col;

void main() {
   gl_FragColor = frag_col;
}
)";

#endif

static auto SAMPLE_TEXT = R"(a line of text
another line of text with some data
whats going on another line of text with some data that forms a really really long sentence yeah? yeah!
another line but something
what is going on here
something should really happen here
this is weird)";


void text_demo() {
   auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
      if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
         ctx.win().set_should_close();
      return false;
   };

   glpp::init();
   glpp::context context({ 800, 600 }, key_handler);

#ifdef DRAW_MANUALLY
   glpp::program prg(
      glpp::shader::create_from_source(VERT_SHADER_SOURCE, glpp::shader::Vertex),
      glpp::shader::create_from_source(FRAG_SHADER_SOURCE, glpp::shader::Fragment)
      );

   auto text_buffer = glpp::describe_debug_text_buffer(SAMPLE_TEXT)
      .attrib("pos", 2)
      .skip_bytes(4 * 2) // skip [z, col] words
      .build(prg);

   while (!context.win().closing()) {
      prg.use();
      gl::vertex_attrib_4f(prg.attrib("col").location(), .8f, .8f, .8f, 1.f);
      glpp::draw(text_buffer, glpp::DrawMode::Triangles);

      context.win().swap();
   }

#else

   auto ui_transform = [&] {
      auto xres = (float)context.win().frame_buffer_dims().x / 2.f;
      auto yres = (float)context.win().frame_buffer_dims().y / 2.f;

      utils::log(utils::LOG_INFO, "x: %d y: %d\n", (int)xres, (int)yres);
      auto xpad = xres / 100.f;
      auto ypad = xpad * .75f;
      auto mvp = glm::ortho(0.f, xres, yres, 0.f);
      auto m = glm::translate(glm::vec3{ xpad, ypad, 0.f });

      return mvp * m;
   };

   glpp::program ui_prg = glpp::make_debug_ui_program(); // create_program(context.assets, "ui");
   glpp::pass_t ui_pass = glpp::make_debug_text_pass(SAMPLE_TEXT, ui_prg, ui_transform());

   int count = 100;
   while (!context.win().closing()) {
      gl::clear(gl::clear_buffer_flags_t::color_buffer_bit | gl::clear_buffer_flags_t::depth_buffer_bit);

      ui_pass.draw(glpp::DrawMode::Triangles);

      context.win().swap();
   }
#endif

   //glpp::shutdown();
}