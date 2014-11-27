#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <cstdlib>
#include <map>
#include "utils.h"
#include "gl.h"
#include "game.h"

// TODO: remove this
#include <glm/gtc/matrix_transform.hpp>


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


   // turns key presses into a directional vector. consumed by entity controller
   class player_controls_t {
   public:
      player_controls_t()
         : dir_flags_(0), direction_(0., 0.) {
      }

      bool is_moving() const { return dir_flags_ != 0; }
      glm::vec2 const & direction() const { return direction_; }

      void handle_key_action(gl::Key key, gl::KeyAction action) {
         enum class dir { none = 0, up = 1, down = 2, left = 4, right = 8 };

         auto key_dir = [key] {
            switch (key) {
            case gl::KEY_A: case gl::KEY_LEFT: return dir::left;
            case gl::KEY_S: case gl::KEY_DOWN: return dir::down;
            case gl::KEY_D: case gl::KEY_RIGHT: return dir::right;
            case gl::KEY_W: case gl::KEY_UP: return dir::up;
            default: return dir::none;
            }
         }();

         if (key_dir == dir::none) return;
         if (action == gl::KEY_ACTION_PRESS) dir_flags_ |= static_cast<uint16_t>(key_dir);
         if (action == gl::KEY_ACTION_RELEASE) dir_flags_ &= ~static_cast<uint16_t>(key_dir);

         float horiz_vec = 0.;
         if (dir_flags_ & static_cast<uint16_t>(dir::left)) horiz_vec -= 1.f;
         if (dir_flags_ & static_cast<uint16_t>(dir::right)) horiz_vec += 1.f;
         float vert_vec = 0.;
         if (dir_flags_ & static_cast<uint16_t>(dir::up)) vert_vec += 1.f;
         if (dir_flags_ & static_cast<uint16_t>(dir::down)) vert_vec -= 1.f;

         auto dir = glm::vec2(horiz_vec, vert_vec);
         direction_ = glm::length(dir) == 0. ? dir : glm::normalize(dir);
      }

   private:
      std::uint16_t dir_flags_;
      glm::vec2 direction_;
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


   player_controls_t controls;

   try {
      bool should_reload_program = false;
      bool should_next = false;
      auto key_handler = [&](gl::context & ctx, gl::Key key, int scancode, gl::KeyAction action, int mods) {
         if (key == gl::KEY_ESCAPE && action == gl::KEY_ACTION_PRESS)
            ctx.win().set_should_close();
         if (key == gl::KEY_R && action == gl::KEY_ACTION_PRESS)
            should_reload_program = true;
         if (key == gl::KEY_N && action == gl::KEY_ACTION_PRESS)
            should_next = true;

         controls.handle_key_action(key, action);
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
      // load sprite data
      //

      gl::sprite_sheet sprites({ "../res/kenney_platformer_graphics/Player/p1_walk/p1_walk.png" }, {
         { { 0, 420 }, { 66, 92 } },
         { { 66, 419 }, { 66, 92 } },
         { { 133, 420 }, { 66, 92 } },
         { { 0, 326 }, { 66, 92 } },
         { { 133, 420 }, { 66, 92 } },
         { { 66, 420 }, { 66, 92 } },
      });


      //
      // load game data
      //

      game::game_t game;

      auto player_controller = [&controls](double t, game::entity_t & e) {
         e.set_dir(controls.direction());
         e.set_vel(controls.direction() * (static_cast<float>(t) * 400.f));
         e.update(t);
      };

      auto player_sprite_controller = [&controls](double t, game::entity_t const & e, gl::sprite_t & s) {
         if (!e.is_moving()) s.set_animation_idx(0);
         else s.set_animation_idx(1);

         s.update(t);
      };

      auto create_player_sprite = [&sprites]()->gl::sprite_t {
         return{
            { sprites, { 0 } },
            { sprites, { 1, 2, 3, 4, 5 } }
         };
      };

      auto player_entity_id = game.add_entity({}, player_controller);
      game.add_sprite(player_entity_id, create_player_sprite(), player_sprite_controller);


      // 
      // load shaders
      //

      auto prg_post = create_program("post");
      auto prg_2d = create_program("2d");


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
         .set_uniform("texture", gl::texture_t{ "bg_green.png" });

      auto fbo = std::unique_ptr<gl::frame_buffer_t>(new gl::frame_buffer_t(context.win().frame_buffer_dims()));

      auto post_pass = prg_post.pass()
         .with(screen_vertices_spec)
         .set_uniform("texture", fbo->texture())
         .set_uniform_action("t", set_time_cb);

      static const float sprite_verts[] = {
         -35., 93., 0., 1.,
         34., 93., 1., 1.,
         -35., 0., 0., 0.,
         34., 0., 1., 0.,
      };

      static const unsigned short sprite_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto sprite_vertices_spec = gl::describe_buffer({ sprite_verts, sprite_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);

      auto sprite_tex = sprites.texture();

      auto prg_sprite = create_program("sprite");
      auto sprite_pass = prg_sprite.pass()
         .with(sprite_vertices_spec)
         .set_uniform("proj", glm::ortho<float>(0., 800., 0., 600.))
         .set_uniform("texture_wh", glm::vec2(sprite_tex.width(), sprite_tex.height()))
         .set_uniform("texture", sprite_tex);

      //
      // game loop
      //

      double last_tick = gl::get_time();

      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               ::reload_program(prg_post, "post");
               ::reload_program(prg_2d, "2d");
               ::reload_program(prg_sprite, "sprite");
            }
            catch (gl::shader_compile_error const & ex) {
               utils::log(utils::LOG_ERROR, "%s\n", ex.what());
               utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
            }
            should_reload_program = false;
         }

         //if (should_next) {
         //   auto & sprite = player_entity.sprite();
         //   auto new_idx = (sprite.idx() + 1) % sprite.current_frame_count();
         //   player_entity.sprite().set_idx(new_idx);

         //   utils::log(utils::LOG_INFO, "sprite[%d]\n", new_idx);

         //   should_next = false;
         //}

         auto dims = context.win().frame_buffer_dims();

         if (!fbo || fbo->dims() != dims) {
            fbo = std::unique_ptr<gl::frame_buffer_t>(new gl::frame_buffer_t(dims));
         }

         double this_tick = gl::get_time();
         double time_since_last_tick = this_tick - last_tick;

         game.update(time_since_last_tick);

         glClearColor(.7f, .87f, .63f, 1.);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glViewport(0, 0, dims.x, dims.y);

         fbo->bind();
         bg_pass.draw(gl::DrawMode::Triangles);

         game.for_each_sprite([&sprite_pass](game::entity_t const & entity, gl::sprite_t const & sprite) {
            sprite_pass
               .set_uniform("model", entity.transform())
               .set_uniform("sprite_xy", sprite.current_frame().position)
               .set_uniform("sprite_wh", sprite.current_frame().dimensions)
               .draw(gl::DrawMode::Triangles);
         });

         fbo->unbind();

         post_pass.draw(gl::DrawMode::Triangles);

         last_tick = this_tick;

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
