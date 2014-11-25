#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

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
      bool should_save = false;
      auto key_handler = [&](gl::context & ctx, gl::Key key, int scancode, gl::KeyAction action, int mods) {
         if (key == gl::KEY_ESCAPE && action == gl::KEY_ACTION_PRESS)
            ctx.win().set_should_close();
         if (key == gl::KEY_R && action == gl::KEY_ACTION_PRESS)
            should_reload_program = true;
         if (key == gl::KEY_S && action == gl::KEY_ACTION_PRESS)
            should_save = true;
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

      //auto prg_prime = create_program("prime");
      auto prg_2d = create_program("2d");
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

      //auto prime_screen_vertices = gl::describe_buffer({ screen_verts, screen_indices })
      //   .add(a_prime_position, 2)
      //   .add(a_prime_tex_coords, 2).build();

      auto screen_vertices = gl::describe_buffer({ screen_verts, screen_indices })
         .add(prg_2d.attrib("p"), 2)
         .add(prg_2d.attrib("tex_coords"), 2).build();

      auto post_screen_vertices = gl::describe_buffer({ screen_verts, screen_indices })
         .add(prg_post.attrib("p"), 2)
         .add(prg_post.attrib("tex_coords"), 2).build();

      //gl::texture_t prime_tex{ "bg_small.png" };
      gl::texture_t bg_tex{ "bg_green.png" };
      
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

      //auto prime_pass = prg_prime.pass()
      //   .with(prime_screen_vertices)
      //   .set_uniform("texture", prime_tex)
      //   .set_uniform("texture_dims", glm::vec2{ prime_tex.width(), prime_tex.height() })
      //   .set_uniform("sub_tex[0].startx", 0.f)
      //   .set_uniform("sub_tex[0].starty", 0.f)
      //   .set_uniform("sub_tex[0].width", 32.f)
      //   .set_uniform("sub_tex[0].height", 32.f)
      //   //.set_uniform("sub_tex[1].texture", prime_tex)
      //   .set_uniform("sub_tex[1].startx", 33.f)
      //   .set_uniform("sub_tex[1].starty", 14.f)
      //   .set_uniform("sub_tex[1].width", 23.f)
      //   .set_uniform("sub_tex[1].height", 17.f)
      //   //.set_uniform("sub_tex[2].texture", prime_tex)
      //   .set_uniform("sub_tex[2].startx", 33.f)
      //   .set_uniform("sub_tex[2].starty", 0.f)
      //   .set_uniform("sub_tex[2].width", 19.f)
      //   .set_uniform("sub_tex[2].height", 13.f)
      //   .set_uniform("sub_tex_count", 3)
      //   .invoke_uniform_action("win_dims", set_dims_cb);

      auto bg_pass = prg_2d.pass()
         .with(screen_vertices)
         .set_uniform("texture", bg_tex)
         .set_uniform("offset", 0.f)
         .invoke_uniform_action("t", set_time_cb);

      auto fbo = std::make_unique<gl::frame_buffer_t>(context.win().frame_buffer_dims());

      auto post_pass = prg_post.pass()
         .with(post_screen_vertices)
         .set_uniform("texture", fbo->texture())
         .invoke_uniform_action("t", set_time_cb);

      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               //::reload_program(prg_prime, "prime");
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

         //prime_pass.draw(gl::DrawMode::Triangles);
         bg_pass.draw_to(*fbo, gl::DrawMode::Triangles);
         post_pass.draw(gl::DrawMode::Triangles);

         if (should_save) {
            fbo->texture().save("test.png");
            should_save = false;
         }

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
