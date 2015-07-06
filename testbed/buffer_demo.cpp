#include <glpp/glpp.h>

#ifdef _MSC_VER
#  include <glpp/gles2.h>
   namespace gl { using namespace gles2; }
#else
#  include <glpp/gl2.h>
   namespace gl { using namespace gl2; }
#endif


#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <glpp/utils.h>

static auto VERT_SHADER_SOURCE = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;

attribute mediump vec2 pos;
attribute lowp vec4 col;

varying mediump vec3 frag_pos;
varying lowp vec4 frag_col;

void main() {
   mediump vec2 new_pos = vec2(pos.x * cos(t) - pos.y * sin(t), pos.y * cos(t) + pos.x * sin(t));
   gl_Position = vec4(new_pos, 0., 1.);
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

namespace {

   struct vert_info {
      glm::vec2 pos;
      glm::vec4 col;
   };

   struct shape_info_t {
      std::vector<vert_info> verts;
      std::vector<gl::ushort_t> indices;
   };

   shape_info_t create_shape(int iterations) {
      shape_info_t result;
      // add center vert
      result.verts.push_back({ { 0.f, 0.f },{ 1.f, 1.f, 1.f, 1.f } });
      gl::ushort_t last_idx = 0;

      std::function<void(vert_info, vert_info, int)> recurse = [&](vert_info v1, vert_info v2, int iter) {
         if (iter == 0) {
            // v1 should be the last vertex created
            result.verts.push_back(v2);
            // new tri: center -> v1 -> v2
            result.indices.push_back(0);
            result.indices.push_back(last_idx++);
            result.indices.push_back(last_idx);
            return;
         }

         auto angle = glm::orientedAngle(v1.pos, v2.pos);
         auto pmid = glm::rotate(v1.pos, angle / 2.f);
         auto vmid = vert_info{ pmid, (v1.col + v2.col) / 2.f };
         recurse(v1, vmid, iter - 1);
         recurse(vmid, v2, iter - 1);
      };

      static std::vector<vert_info> quad_outer_verts = {
         { { 0.f, 1.f },{ 1.f, 0.1f, 0.1f, 1.f } },
         { { -1.f, 0.f },{ 0.1f, 1.f, 0.1f, 1.f } },
         { { 0.f, -1.f },{ 0.1f, 0.1f, 1.f, 1.f } },
         { { 1.f, 0.f },{ 1.f, 1.f, 1.f, 1.f } },
      };

      for (auto idx = 0U; idx < quad_outer_verts.size(); idx++) {
         auto & v1 = quad_outer_verts[idx];
         auto & v2 = quad_outer_verts[(idx + 1) % quad_outer_verts.size()];

         // recursion function assumes previous triangle's last vertex already exists
         result.verts.push_back(v1);
         last_idx++;

         recurse(v1, v2, iterations);
      }

      return result;
   };
}

void buffer_demo() {
   auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
      if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
         ctx.win().set_should_close();
   };

   glpp::init();
   glpp::context context(key_handler);
   gl::disable(gl::enable_cap_t::cull_face);

   glpp::program prg(
      glpp::shader::create_from_source(VERT_SHADER_SOURCE, glpp::shader::Vertex),
      glpp::shader::create_from_source(FRAG_SHADER_SOURCE, glpp::shader::Fragment)
      );

   auto buffer = glpp::buffer_t{ glpp::buffer_t::Usage::Stream };

   //auto buffer = glpp::buffer_t{ { (float*)shape_verts.data(), (sizeof(vert_info) * shape_verts.size()) / 4 }, glpp::buffer_t::Usage::Stream };
   auto mapped_buffer =
      glpp::buffer_attrib_mappings_t()
      .push_attrib("pos", 2)
      .push_attrib("col", 4)
      .map_buffer(prg, buffer);

   const int WAVE_PERIOD = 6;

   auto current_iters = -1;
   auto start_time = glpp::get_time();
   while (!context.win().closing()) {
      gl::clear(gl::clear_buffer_flags_t::color_buffer_bit);

      auto time_segment = (int)((glpp::get_time() - start_time) * 8.);
      auto new_iters = WAVE_PERIOD - std::abs(time_segment % (2 * WAVE_PERIOD) - WAVE_PERIOD);
      if (new_iters != current_iters) {
         utils::log(utils::LOG_INFO, "now showing %d iterations\n", new_iters);
         auto shape_info = create_shape(new_iters);

         buffer.update(
            { (float*)shape_info.verts.data(), (sizeof(vert_info) * shape_info.verts.size()) / 4 },
            { shape_info.indices.data(), shape_info.indices.size() });

         current_iters = new_iters;
      }
      prg.use();
      prg.uniform("t").set((float)(glpp::get_time() - start_time));
      glpp::draw(mapped_buffer, glpp::DrawMode::Triangles);

      context.win().swap();
   }

   //glpp::shutdown();
}