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
   gl::shader vert_shader() { return gl::shader::create_from_file("shaders/2d.vert", gl::shader::Vertex); }
   gl::shader frag_shader() { return gl::shader::create_from_file("shaders/2d.frag", gl::shader::Fragment); }

   gl::program create_program() {
      utils::log(utils::LOG_INFO, "compiling shader program... ");

      auto program = gl::program{ vert_shader(), frag_shader() };

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }

      return program;
   };

   void reload_program(gl::program & program) {
      utils::log(utils::LOG_INFO, "reloading shader program... ");

      program.reload(vert_shader(), frag_shader());

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
      
      auto program = create_program();

      auto u_time = program.uniform("t");
      auto u_texture = program.uniform("texture");

      auto a_position = program.attrib("p");
      auto a_tex_coords = program.attrib("tex_coords");

      static const float vertexArray[] = {
         0.0, 0.5, 0.0, 0., 1., 0.,
         -0.5, 0.0, 0.0, .25, 0., 0.,
         0.0, -0.5, 0.0, .5, 0., 1.,
         0.5, 0.0, 0.0, .75, 1., 1.,
         0.0, 0.5, 0.0, 1., 0., 1.,
      };

      static const unsigned short indices[] = {
         0, 1, 2,
         0, 2, 3,
      };

      //auto vert_buffer = gl::describe_static(vertexArray)
      //   .add(a_position, 3)
      //   .add(a_red_color, 1).build();

      auto vertex_buffer = gl::describe_buffer({ vertexArray, indices })
         .add(a_position, 3)
         .skip_bytes(4)
         .add(a_tex_coords, 2).build();

      gl::texture_t texture { "img_test.png" };
      u_texture.set(texture);

      // configure how GL is to display images in general
      GL_VERIFY(glEnable(GL_BLEND));
      //GL_VERIFY(glBlendFunc(GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA));


      // get a GL name for our texture
      // load data into texture
      // bind texture id to texture unit
      // bind texture unit to uniform

      // for masking (mask is a tex_id)
      //glBlendFunc(GL_DST_COLOR, GL_ZERO);				// Blend Screen Color With Zero (Black)
      //glBindTexture(GL_TEXTURE_2D, mask);

      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               ::reload_program(program);
            }
            catch (gl::shader_compile_error const & ex) {
               utils::log(utils::LOG_ERROR, "%s\n", ex.what());
               utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
            }
            should_reload_program = false;
         }

         auto dims = context.win().frame_buffer_dims();

         glClearColor(.9f, .9f, .9f, 1.f);

         glViewport(0, 0, dims.x, dims.y);
         glClear(GL_COLOR_BUFFER_BIT);

         u_time.set(static_cast<float>(gl::get_time()));

         program.pass()
            .with(vertex_buffer)
            .draw(gl::DrawMode::Triangles);

         context.win().swap();
      }

      program.destroy();
      context.destroy();
      gl::shutdown();

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
}
