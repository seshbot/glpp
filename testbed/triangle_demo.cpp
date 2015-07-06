#include <glpp/glpp.h>

#ifdef _MSC_VER
#  include <glpp/gles2.h>
namespace gl { using namespace gles2; }
#else
#  include <glpp/gl2.h>
namespace gl { using namespace gl2; }
#endif

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


void triangle_demo() {

   bool should_reload_program = false;
   auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
      if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
         ctx.win().set_should_close();
      if (key == glpp::KEY_R && action == glpp::KEY_ACTION_PRESS)
         should_reload_program = true;
   };

   glpp::init();
   glpp::context context(key_handler);

   glpp::program prg(
      glpp::shader::create_from_source(VERT_SHADER_SOURCE, glpp::shader::Vertex),
      glpp::shader::create_from_source(FRAG_SHADER_SOURCE, glpp::shader::Fragment)
      );

   const float verts[] = {
      0.f, .5f,    1.f, 0.f, 0.f,
      -.5f, -.5f,  1.f, 1.f, 0.f,
      .5f, -.5f,   0.f, 0.f, 1.f,
   };
   auto vert_buffer = glpp::buffer_attrib_mappings_t()
      .push_attrib("pos", 2)
      .push_attrib("col", 3)
      .map_buffer(prg, { verts });

   gl::clear_color(1.f, 1.f, 1.f, 1.f);

   //
   // main loop
   //

   double last_tick = glpp::get_time();

   while (!context.win().closing())
   {
      if (should_reload_program) {
         // TODO - reload shaders
         should_reload_program = false;
      }

      double this_tick = glpp::get_time();
      double time_since_last_tick = this_tick - last_tick;

      const double MAX_TICK_SECONDS = 1. / 15.;
      if (time_since_last_tick > MAX_TICK_SECONDS) time_since_last_tick = MAX_TICK_SECONDS;

      //
      // render
      //
      prg.use();
      glpp::draw(vert_buffer, glpp::DrawMode::Triangles);

      last_tick = this_tick;

      context.win().swap();
   }

   //glpp::shutdown();
}