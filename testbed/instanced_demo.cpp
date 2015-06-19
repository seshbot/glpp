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

attribute mediump vec2 mesh_pos;
attribute mediump vec2 pos;
attribute lowp vec4 col;

varying mediump vec3 frag_pos;
varying lowp vec4 frag_col;

void main() {
   gl_Position = vec4(mesh_pos + pos, 0., 1.);
   frag_col = col;

   gl_PointSize = 3.;
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


#include <random>

struct particle_static_info {
   float pos[2];
   float col[4];
};

static std::vector<particle_static_info> create_particle_info(std::size_t n, float minx = -1., float maxx = 1., float miny = -1., float maxy = 1.) {
   std::default_random_engine rnd_engine;
   auto rnd = [&] {
      return (float)((rnd_engine() % 1000) / 1000.);
   };

   float xrange = std::abs(maxx - minx);
   float yrange = std::abs(maxy - miny);

   std::vector<particle_static_info> result;
   for (auto idx = 0U; idx < n; idx++) {
      result.push_back({
         { rnd() * xrange + minx, rnd() * yrange + miny },
         { rnd(), rnd(), rnd(), 1.f }
      });
   }

   return result;
}


void instanced_demo() {

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

   float vertices[] = {
      -.01f, -.01f,
      .01f, -.01f,
      .0f, .01f,
   };

   auto particle_mesh_buffer = glpp::describe_buffer({ vertices })
      .attrib("mesh_pos", 2)
      .build(prg);

   auto particle_positions = create_particle_info(1000);
   auto particle_positions_buffer = glpp::describe_buffer({ { (float*)particle_positions.data(), 6 * particle_positions.size() } })
      .attrib("pos", 2)
      .attrib("col", 4)
      .build(prg);

   gl::clear_color(0.f, 0.f, 0.f, 1.f);

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
      glpp::bind(particle_mesh_buffer);
      glpp::bind(particle_positions_buffer);
      //gl::angle::vertex_attrib_divisor(prg.attrib("mesh_pos").location(), 0);
      gl::vertex_attrib_divisor(prg.attrib("pos").location(), 1);
      gl::vertex_attrib_divisor(prg.attrib("col").location(), 1);
      gl::draw_arrays_instanced(gl::primitive_type_t::triangles, 0, 3, 1000);
      //glpp::draw(particle_positions_buffer, glpp::DrawMode::Points);

      last_tick = this_tick;

      context.win().swap();
   }
   
   glpp::shutdown();
}