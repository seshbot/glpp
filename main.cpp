#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include "utils.h"
#include "gl.h"

namespace {
   gl::shader vert_shader(std::string name) { return gl::shader::create_from_file(utils::fmt("../shaders/%s.vert", name.c_str()), gl::shader::Vertex); }
   gl::shader frag_shader(std::string name) { return gl::shader::create_from_file(utils::fmt("../shaders/%s.frag", name.c_str()), gl::shader::Fragment); }

   gl::program create_program(std::string name) {
      utils::log(utils::LOG_INFO, "compiling '%s' shader programs... ", name.c_str());

      auto program = gl::program{ vert_shader(name), frag_shader(name) };

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }

      return program;
   };

   void reload_program(gl::program & program, std::string name) {
      utils::log(utils::LOG_INFO, "reloading '%s' shader programs... ", name.c_str());

      program.reload(vert_shader(name), frag_shader(name));

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }
   };
}


#ifdef WIN32
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
      gl::init();
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      exit(EXIT_FAILURE);
   }

   try {
      bool should_reload_program = false;
      auto key_handler = [&](gl::context & ctx, gl::Key key, int scancode, gl::KeyAction action, int mods) {
         if (key == gl::KEY_ESCAPE && action == gl::KEY_ACTION_PRESS)
            ctx.win().set_should_close();
         if (key == gl::KEY_R && action == gl::KEY_ACTION_PRESS)
            should_reload_program = true;
      };

      gl::context context { key_handler };

#ifdef USE_GLEW
      GLenum err = glewInit();
      if (err != GLEW_OK)
      {
         utils::log(utils::LOG_ERROR, "GLEW init error\n");
         exit(EXIT_FAILURE);
      }

      if (!GLEW_VERSION_2_1)
      {
         utils::log(utils::LOG_ERROR, "GLEW v2.1 not available!\n");
         exit(EXIT_FAILURE);
      }
#endif

      utils::log(utils::LOG_INFO, "%s\n", context.info().c_str());


      // 
      // load shaders
      //

      auto prg_2d = create_program("2d");
      auto prg_sprite = create_program("sprite");
      auto prg_post = create_program("post");


      //
      // load vertex data
      //

      static const float screen_verts[] = {
         -1., 1., 0., 1.,
         1., 1., 1., 1.,
         -1., -1., 0., 0.,
         1., -1., 1., 0.,
      };

      static const unsigned short screen_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto screen_vertices_spec = gl::describe_buffer({ screen_verts, screen_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);
      
      // get a GL name for our texture
      // load data into texture
      // bind texture id to texture unit
      // bind texture unit to uniform

      // https://open.gl/textures has a good run-down

      // https://github.com/adobe/angle adobe wrapper??

      auto set_time_cb = [](gl::uniform & u){u.set(static_cast<float>(gl::get_time())); };
      auto set_dims_cb = [&context](gl::uniform & u){
         auto dims = context.win().frame_buffer_dims();
         u.set(glm::vec2{ dims.x, dims.y });
      };


      auto set_proj_cb = [](gl::uniform & u){u.set(static_cast<float>(gl::get_time())); };
      auto bg_pass = prg_2d.pass()
         .with(screen_vertices_spec)
         .set_uniform("proj", glm::ortho(-1.f, 1.f, -1.f, 1.f))
         .set_uniform("texture", gl::texture_t{ "bg_green.png" });

      auto fbo = std::make_unique<gl::frame_buffer_t>(context.win().frame_buffer_dims());

      auto post_pass = prg_post.pass()
         .with(screen_vertices_spec)
         .set_uniform("texture", fbo->texture())
         .invoke_uniform_action("t", set_time_cb);


      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               ::reload_program(prg_post, "post");
               ::reload_program(prg_2d, "2d");
            }
            catch (gl::shader_compile_error const & ex) {
               utils::log(utils::LOG_ERROR, "%s\n", ex.what());
               utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
            }
            should_reload_program = false;
         }

         auto dims = context.win().frame_buffer_dims();

         if (!fbo || fbo->dims() != dims) {
            fbo = std::make_unique<gl::frame_buffer_t>(dims);
         }

         glClearColor(1.f, 0.f, 1.f, 1.);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glViewport(0, 0, dims.x, dims.y);

         bg_pass.draw_to(*fbo, gl::DrawMode::Triangles);
         post_pass.draw(gl::DrawMode::Triangles);

         context.win().swap();
      }

      exit(EXIT_SUCCESS);
   }
   catch (gl::shader_compile_error const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());

      gl::shutdown();
      exit(EXIT_FAILURE);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());

      gl::shutdown();
      exit(EXIT_FAILURE);
   }

   gl::shutdown();
}
