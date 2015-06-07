#ifdef _MSC_VER
#  include <windows.h>
#endif

#include <glpp/glpp.h>
#include <glpp/assets.h>
#include <glpp/utils.h>
#include <glpp/scene.h>

#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

#include <algorithm>
#include <array>
#include <cstdlib>
#include <map>


namespace gl {
#ifdef _MSC_VER
   using namespace gles2;
#else
   using namespace gl2;
#endif
}


auto VERT_SHADER_SOURCE = R"(
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

auto FRAG_SHADER_SOURCE = R"(
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


#ifdef _MSC_VER
int CALLBACK WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR lpCmdLine,
   int nCmdShow
)
#else
int main()
#endif
{
   utils::log(utils::LOG_INFO, "starting (cwd: %s)\n", utils::getcwd().c_str());


   try {
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
      auto vert_buffer = glpp::describe_buffer({ verts })
         .attrib("pos", 2)
         .attrib("col", 3)
         .build(prg);

      auto SAMPLE_TEXT = R"(a line of text
another line of text with some data
whats going on another line of text with some data
another line but something
what is going on here
something should really happen here
this is weird)";

      auto text_buffer = glpp::describe_debug_text_buffer(SAMPLE_TEXT, -.95f, .95f, .0025f)
         .attrib("pos", 2)
         .skip_bytes(4 * 2) // skip [z, col] words
         .build(prg);

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
         gl::vertex_attrib_4f(prg.attrib("col").location(), .8, .8, .8, 1.);
         glpp::draw(text_buffer, glpp::DrawMode::Triangles);

         last_tick = this_tick;

         context.win().swap();
      }
   }
   catch (glpp::shader_compile_error const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }

   glpp::shutdown();

   exit(EXIT_SUCCESS);
}
