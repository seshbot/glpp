#include <glpp/ui.h>
#include <glpp/utils.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

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
#include <glpp/imgui.h>

namespace {
   struct context_data_t {
      glpp::context & gl_context;
      double current_time;
      glpp::program prg;
      glpp::texture_unit_t texture_unit;
      glpp::texture_t texture;
      glpp::mapped_buffer_t mapped_buffer;
   };
}
struct glpp::imgui::ui_context_t::impl {
   context_data_t data;
};

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

   void render_callback(void * context, ImDrawList** const cmd_lists, int cmd_lists_count) {
      auto * ui_context = static_cast<context_data_t*>(context);

      if (cmd_lists_count == 0)
         return;


      // Setup orthographic projection matrix
      ImGuiIO& io = ImGui::GetIO();
      const float width = io.DisplaySize.x;
      const float height = io.DisplaySize.y;

      ui_context->prg.use();

      ui_context->texture_unit.activate();
      ui_context->texture.bind();
      ui_context->prg.uniform("Texture").set(ui_context->texture_unit);
      ui_context->prg.uniform("ProjMtx").set(glm::ortho(0.f, width, height, 0.f));

      // Grow our buffer according to what we need
      size_t total_vtx_count = 0;
      for (int n = 0; n < cmd_lists_count; n++)
         total_vtx_count += cmd_lists[n]->vtx_buffer.size();

      std::vector<ImDrawVert> buffer_data{ total_vtx_count };

      auto buffer_end_idx = 0U;
      for (int n = 0; n < cmd_lists_count; n++) {
         const ImDrawList* cmd_list = cmd_lists[n];
         memcpy(buffer_data.data() + buffer_end_idx, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
         buffer_end_idx += cmd_list->vtx_buffer.size();
      }

      ui_context->mapped_buffer.buffer.update({ (float*)buffer_data.data(), (buffer_data.size() * sizeof(ImDrawVert)) / sizeof(float) });

#if 0
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
#endif

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
               gl_::scissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
               glpp::draw(ui_context->mapped_buffer, glpp::DrawMode::Triangles, vtx_offset, pcmd->vtx_count);
            }
            vtx_offset += pcmd->vtx_count;
         }
         cmd_offset = vtx_offset;
      }

      // Restore modified state
      gl_::disable(gl_::enable_cap_t::scissor_test);
   }

   GLFWwindow * get_win_(context_data_t * data) {
      return reinterpret_cast<GLFWwindow*>(data->gl_context.platform_handle());
   }

   GLFWwindow * get_win_(void * context) {
      if (nullptr == context) return nullptr;
      auto * data = reinterpret_cast<context_data_t*>(context);
      return get_win_(data);
   }

   bool g_MousePressed[3] = { false, false, false };
   float g_MouseWheel = 0.0f;

   void init_callbacks(context_data_t & ui_context) {
      ImGuiIO& io = ImGui::GetIO();
      io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                 // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
      io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
      io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
      io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
      io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
      io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
      io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
      io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
      io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
      io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
      io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
      io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
      io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
      io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
      io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
      io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
      io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

      io.CallbackContext = (void*)&ui_context;
      io.RenderDrawListsFn = render_callback;

      io.SetClipboardTextFn = [](void* c, const char* text) { if (auto * w = get_win_(c)) glfwSetClipboardString(w, text); };
      io.GetClipboardTextFn = [](void* c) { if (auto * w = get_win_(c)) return glfwGetClipboardString(w); return ""; };

      auto * glfw_win = get_win_(&ui_context);
#ifdef _MSC_VER
      io.ImeWindowHandle = glfwGetWin32Window(glfw_win);
#endif

      auto mouse_button_callback = [](glpp::context&, int button, int action, int /*mods*/) -> bool {
         if (action == GLFW_PRESS && button >= 0 && button < 3)
            g_MousePressed[button] = true;
         return false;
      };

      auto mouse_scroll_callback = [](glpp::context&, double /*xoffset*/, double yoffset) -> bool {
         g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
         return false;
      };

      auto key_callback = [](glpp::context&, glpp::Key key, int, glpp::KeyAction action, int mods) -> bool {
         ImGuiIO& io = ImGui::GetIO();
         if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
         if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

         (void)mods; // Modifiers are not reliable across systems
         io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
         io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
         io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
         return false;
      };

      auto char_callback = [](glpp::context&, unsigned int c) -> bool {
         ImGuiIO& io = ImGui::GetIO();
         if (c > 0 && c < 0x10000)
            io.AddInputCharacter((unsigned short)c);
         return false;
      };

      auto & gl_ctx = ui_context.gl_context;
      gl_ctx.add_key_callback(key_callback, true);
      gl_ctx.add_mouse_scroll_callback(mouse_scroll_callback, true);
      gl_ctx.add_mouse_button_callback(mouse_button_callback, true);
      gl_ctx.add_char_callback(char_callback, true);
   }
}

namespace glpp {
namespace imgui {

   ui_context_t::ui_context_t(std::unique_ptr<impl> i)
      : impl_(std::move(i)) {
   }

   ui_context_t::~ui_context_t() = default;

   ui_context_t init(context & ctx, glpp::texture_unit_t ui_texture_unit)
   {
      glpp::program prg(
         glpp::shader::create_from_source(VERTEX_SHADER, glpp::shader::Vertex),
         glpp::shader::create_from_source(FRAG_SHADER, glpp::shader::Fragment));

      auto buffer_attrib_map = glpp::buffer_attrib_mappings_t()
         .push_attrib("Position", 2)
         .push_attrib("UV", 2)
         .push_attrib_typed<gl::ubyte_t>("Color", 4);

      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->AddFontDefault();

      unsigned char* pixels;
      int width, height;
      io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

      auto texture = glpp::texture_t{ {pixels, width, height}, false };

      // Store our identifier
      io.Fonts->TexID = (void *)(intptr_t)texture.id();

      // Cleanup (don't clear the input data if you want to append new fonts later)
      io.Fonts->ClearInputData();
      io.Fonts->ClearTexData();

      auto ui_context_data = context_data_t{
         ctx, 0.,
         prg,
         ui_texture_unit, texture,
         glpp::map_buffer(prg, glpp::buffer_t(glpp::buffer_t::Usage::Stream), buffer_attrib_map) 
      };

      auto ui_context_impl = std::unique_ptr<ui_context_t::impl>(new ui_context_t::impl{ ui_context_data });
      init_callbacks(ui_context_impl->data);

      return{ std::move(ui_context_impl) };
   }

   void new_frame(ui_context_t & ui_context) {
      auto & context_data = ui_context.impl_->data;
      auto * glfw_win = get_win_(&context_data);
      ImGuiIO& io = ImGui::GetIO();

      // Setup display size (every frame to accommodate for window resizing)
      int w, h;
      int display_w, display_h;
      glfwGetWindowSize(glfw_win, &w, &h);
      glfwGetFramebufferSize(glfw_win, &display_w, &display_h);
      io.DisplaySize = ImVec2((float)display_w, (float)display_h);

      // Setup time step
      double current_time =  glfwGetTime();
      io.DeltaTime = context_data.current_time > 0.0 ? (float)(current_time - context_data.current_time) : (float)(1.0f/60.0f);
      context_data.current_time = current_time;

      // Setup inputs
      // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
      if (glfwGetWindowAttrib(glfw_win, GLFW_FOCUSED)) {
         double mouse_x, mouse_y;
         glfwGetCursorPos(glfw_win, &mouse_x, &mouse_y);
         mouse_x *= (float)display_w / w;                        // Convert mouse coordinates to pixels
         mouse_y *= (float)display_h / h;
         io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
      }
      else {
         io.MousePos = ImVec2(-1,-1);
      }

      for (int i = 0; i < 3; i++) {
         io.MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(glfw_win, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
         g_MousePressed[i] = false;
      }

      io.MouseWheel = g_MouseWheel;
      g_MouseWheel = 0.f;

      // Hide OS mouse cursor if ImGui is drawing it
      glfwSetInputMode(glfw_win, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

      // Start the frame
      ImGui::NewFrame();
   }

   void render(ui_context_t & ui_context) {
      ImGuiIO& io = ImGui::GetIO();
      gl_::viewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

      gl_::enable(gl_::enable_cap_t::blend);
      gl_::blend_equation(gl_::blend_equation_mode_t::func_add);
      gl_::blend_func(gl_::blending_factor_src_t::src_alpha, gl_::blending_factor_dest_t::one_minus_src_alpha);
      gl_::disable(gl_::enable_cap_t::cull_face);
      gl_::disable(gl_::enable_cap_t::depth_test);
      gl_::enable(gl_::enable_cap_t::scissor_test);

      ImGui::Render();
   }

} // namespace imgui
} // namespace glpp
