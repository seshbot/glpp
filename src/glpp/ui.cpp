#include <glpp/ui.h>

#define EXCLUDE 1
#if !EXCLUDE

#ifdef _MSC_VER
#  include <glpp/gles2.h>
namespace gl_ {
   using namespace gles2;
   using namespace angle;
}
#else
#  include <glpp/gl2.h>
namespace gl_ {
   using namespace gl2;
   using namespace osx;
}
#endif

#include <glm/gtx/transform.hpp>
#include "detail/imgui/imgui.h"

namespace glpp {
   namespace ui {
      struct ui_context_t {
         glpp::context & gl_context;
         glpp::texture_unit_t texture_unit;
         glpp::program prg;
         glpp::mapped_buffer_t mapped_buffer;
      };
   }
}

namespace {
   const auto * VERTEX_SHADER = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mat4 ProjMtx;

attribute mediump vec2 Position;
attribute mediump vec2 UV;
attribute lowp vec4 Color;

varying mediump vec2 Frag_UV;
varying mediump vec4 Frag_Color;

void main() {
   Frag_UV = UV;
   Frag_Color = Color;
   gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)";

   const auto * FRAG_SHADER = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D Texture;

varying mediump vec2 Frag_UV;
varying lowp vec4 Frag_Color;

void main() {
   gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);
}
)";

   void render_callback(void * context, ImDrawList** const cmd_lists, int cmd_lists_count)
   {
      auto * ui_context = static_cast<glpp::ui::ui_context_t*>(context);

      if (cmd_lists_count == 0)
         return;

      //// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
      //GLint last_program, last_texture;
      //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
      //glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
      gl_::enable(gl_::enable_cap_t::blend);
      gl_::blend_equation(gl_::blend_equation_mode_t::func_add);
      gl_::blend_func(gl_::blending_factor_src_t::src_alpha, gl_::blending_factor_dest_t::one_minus_src_alpha);
      gl_::disable(gl_::enable_cap_t::cull_face);
      gl_::disable(gl_::enable_cap_t::depth_test);
      gl_::enable(gl_::enable_cap_t::scissor_test);

      // Setup orthographic projection matrix
      const float width = ImGui::GetIO().DisplaySize.x;
      const float height = ImGui::GetIO().DisplaySize.y;

      ui_context->prg.use();
      ui_context->prg.uniform("Texture").set(ui_context->texture_unit);
      ui_context->prg.uniform("ProjMtx").set(glm::ortho(0.f, width, height, 0.f));

      ui_context->texture_unit.activate();

      //glUseProgram(g_ShaderHandle);
      //glUniform1i(g_AttribLocationTex, 0);
      //glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

      // Grow our buffer according to what we need
      size_t total_vtx_count = 0;
      for (int n = 0; n < cmd_lists_count; n++)
         total_vtx_count += cmd_lists[n]->vtx_buffer.size();
      glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
      size_t needed_vtx_size = total_vtx_count * sizeof(ImDrawVert);
      if (g_VboSize < needed_vtx_size)
      {
         g_VboSize = needed_vtx_size + 5000 * sizeof(ImDrawVert);  // Grow buffer
         glBufferData(GL_ARRAY_BUFFER, g_VboSize, NULL, GL_STREAM_DRAW);
      }

      // Copy and convert all vertices into a single contiguous buffer
      unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
      if (!buffer_data)
         return;
      for (int n = 0; n < cmd_lists_count; n++)
      {
         const ImDrawList* cmd_list = cmd_lists[n];
         memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
         buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
      }
      glUnmapBuffer(GL_ARRAY_BUFFER);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(g_VaoHandle);

      int cmd_offset = 0;
      for (int n = 0; n < cmd_lists_count; n++)
      {
         const ImDrawList* cmd_list = cmd_lists[n];
         int vtx_offset = cmd_offset;
         const ImDrawCmd* pcmd_end = cmd_list->commands.end();
         for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
         {
            if (pcmd->user_callback)
            {
               pcmd->user_callback(cmd_list, pcmd);
            }
            else
            {
               glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
               glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
               glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            }
            vtx_offset += pcmd->vtx_count;
         }
         cmd_offset = vtx_offset;
      }

      // Restore modified state
      glBindVertexArray(0);
      glUseProgram(last_program);
      glDisable(GL_SCISSOR_TEST);
      glBindTexture(GL_TEXTURE_2D, last_texture);
   }

}

namespace glpp {
namespace ui {

   ui_context_t init(context & ctx)
   {
      glpp::program prg(
         glpp::shader::create_from_source(VERTEX_SHADER, glpp::shader::Vertex),
         glpp::shader::create_from_source(FRAG_SHADER, glpp::shader::Fragment));

      auto buffer_attrib_map = glpp::buffer_attrib_mappings_t()
         .push_attrib("Position", 2)
         .push_attrib("UV", 2)
         .push_attrib("Color", 1);

      g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
      g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");



      ImGui_ImplGlfwGL3_CreateFontsTexture();

      return{ctx, prg, glpp::map_buffer(prg, glpp::buffer_t(glpp::buffer_t::Usage::Stream), buffer_attrib_map)};
   }

}

}

#endif

