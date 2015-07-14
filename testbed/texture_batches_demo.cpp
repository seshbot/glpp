#include <glpp/glpp.h>
#include <random>

#include <glm/gtx/transform.hpp>

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

uniform mat4 transform;
attribute mediump vec2 tex_coords;
attribute mediump vec2 pos;

varying mediump vec2 frag_tex_coords;

void main() {
   gl_Position = transform * vec4(pos, 0., 1.);
   frag_tex_coords = tex_coords;
}
)";

static auto FRAG_SHADER_SOURCE = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;

varying mediump vec2 frag_tex_coords;

void main() {
   gl_FragColor = texture2D(texture, frag_tex_coords);
}
)";


void texture_batches_demo() {

   bool should_reload_program = false;
   auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
      if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
         ctx.win().set_should_close();
      if (key == glpp::KEY_R && action == glpp::KEY_ACTION_PRESS)
         should_reload_program = true;
      return false;
   };

   glpp::init();
   glpp::context context({ 800, 600 }, key_handler);

   glpp::program prg(
      glpp::shader::create_from_source(VERT_SHADER_SOURCE, glpp::shader::Vertex),
      glpp::shader::create_from_source(FRAG_SHADER_SOURCE, glpp::shader::Fragment)
      );

   float screen_vertices[] = {
      -1.f, 1.f,     0., 1.,
      -1.f, -1.f,    0., 0.,
      1.f, 1.f,      1., 1.,
      1.f, 1.f,      1., 1.,
      -1.f, -1.f,    0., 0.,
      1.f, -1.f,     1., 0.,
   };

   float half_screen_vertices[] = {
      -.5f, .5f,     0., 1.,
      -.5f, -.5f,    0., 0.,
      .5f, .5f,      1., 1.,
      .5f, .5f,      1., 1.,
      -.5f, -.5f,    0., 0.,
      .5f, -.5f,     1., 0.,
   };

   glpp::texture_unit_t tex1_unit{ 1 };
   glpp::texture_unit_t tex2_unit{ 2 };
   glpp::texture_t tex1{ glpp::image_t{ "../../res/ground-64x64.png" } };
   glpp::texture_t tex2{ glpp::image_t{ "../../res/test-100x100.png" } };

   tex1_unit.activate();
   tex1.bind();

   tex2_unit.activate();
   tex2.bind();

   auto screen_vert_buffer = glpp::buffer_attrib_mappings_t()
      .push_attrib("pos", 2)
      .push_attrib("tex_coords", 2)
      .map_buffer(prg, { screen_vertices });

   auto half_screen_vert_buffer = glpp::buffer_attrib_mappings_t()
      .push_attrib("pos", 2)
      .push_attrib("tex_coords", 2)
      .map_buffer(prg, { half_screen_vertices });

   auto set_tex1_cb = [&](glpp::uniform & u) { tex1_unit.activate(); tex1.bind();  u.set(tex1_unit); };
   auto set_tex2_cb = [&](glpp::uniform & u) { tex2_unit.activate(); tex2.bind();  u.set(tex2_unit); };
   auto pass1 = prg.pass()
      .with(screen_vert_buffer)
      .set_uniform("transform", glm::mat4{})
      .set_uniform("texture", tex1_unit);

   auto pass2 = prg.pass()
      .with(screen_vert_buffer)
      .set_uniform("transform", glm::scale(glm::mat4{}, glm::vec3{ .5, .75, 1. }))
      .set_uniform("texture", tex2_unit);

   gl::clear_color(0.f, 0.f, 0.f, 1.f);

   struct render_batch_callback_t : public glpp::pass_t::render_batch_callback {
      static std::vector<glm::mat4> generate_transforms() {
         auto gen = [] {
            std::random_device rnd;
            std::vector<glm::mat4> transforms;

            // transforms.push_back(glm::translate(glm::mat4{}, glm::vec3{.5, .0, .0}));
            // transforms.push_back(glm::scale(glm::translate(glm::mat4{}, glm::vec3{-.5, .0, .0}), glm::vec3{.5}));
            // return transforms;

            for (auto y = -1.f; y < 1.f; y += .4f) {
               for (auto x = -1.f; x < 1.f; x += .4f) {
                  float scale = (rnd() % 100) / 100.f;
                  scale = .1f + scale * .1f;
                  glm::vec3 center_pos{ x, y, 0. };

                  auto translated = glm::translate(glm::mat4{}, center_pos);
                  auto transform = glm::scale(translated, glm::vec3{ scale, scale, 1. });

                  transforms.push_back(transform);
               }
            }
            return transforms;
         };

         static std::vector<glm::mat4> transforms = gen();

         return transforms;
      }

      std::vector<glm::mat4> transforms = generate_transforms();
      mutable std::vector<glm::mat4>::const_iterator it = transforms.begin();

      render_batch_callback_t(glm::vec2 const & local_offset, float local_scale) {
         auto translated = glm::translate(glm::mat4{}, glm::vec3{ local_offset.x, local_offset.y, 0.f });
         auto transform = glm::scale(translated, glm::vec3{ local_scale, local_scale, 1.f });

         for (auto idx = 0U; idx < transforms.size(); idx++) {
            transforms[idx] = transforms[idx] * transform;
         }
      }

      bool prepare_next(glpp::program & p) const override {
         if (it == transforms.end()) return false;

         p.uniform("transform").set(*it);

         ++it;
         return true;
      }
   };

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

      float oscillating = (float)std::abs(std::fmod(this_tick, 2.) - 1.);

      const double MAX_TICK_SECONDS = 1. / 15.;
      if (time_since_last_tick > MAX_TICK_SECONDS) time_since_last_tick = MAX_TICK_SECONDS;

      //
      // render
      //
      prg.use();

      pass1.draw(glpp::DrawMode::Triangles);
      float body_scale = .1f;
      pass2.draw_batch(render_batch_callback_t{ { .0, .0 }, body_scale }, glpp::DrawMode::Triangles);
      pass2.draw_batch(render_batch_callback_t{ { .0, body_scale * 1.5f }, body_scale * .5f }, glpp::DrawMode::Triangles);
      pass2.draw_batch(render_batch_callback_t{ { -1.25f * body_scale, body_scale * oscillating }, body_scale * .25f }, glpp::DrawMode::Triangles);
      pass2.draw_batch(render_batch_callback_t{ { 1.25f * body_scale, body_scale * (1.f - oscillating) }, body_scale * .25f }, glpp::DrawMode::Triangles);

      last_tick = this_tick;

      context.win().swap();
   }

   //glpp::shutdown();
}