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
#include <algorithm>

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
      using action_func = std::function < bool(gl::Key, gl::KeyAction) > ;

      player_controls_t()
         : dir_flags_(0), direction_(0., 0.) {
      }

      void register_action_handler(gl::Key k, gl::KeyAction a, action_func f) { actions_[{k, a}] = f; }

      bool is_moving() const { return dir_flags_ != 0; }
      glm::vec2 const & direction() const { return direction_; }

      void handle_key_action(gl::Key key, gl::KeyAction action) {
         auto action_it = actions_.find({ key, action });
         if (actions_.end() != action_it) {
            if (action_it->second(key, action)) return;
         }

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
      using action_key = std::pair < gl::Key, gl::KeyAction > ;
      std::map<action_key, action_func> actions_;
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
      auto key_handler = [&](gl::context & ctx, gl::Key key, int scancode, gl::KeyAction action, int mods) {
         if (key == gl::KEY_ESCAPE && action == gl::KEY_ACTION_PRESS)
            ctx.win().set_should_close();
         if (key == gl::KEY_R && action == gl::KEY_ACTION_PRESS)
            should_reload_program = true;

         controls.handle_key_action(key, action);
      };

      gl::context context{ key_handler };

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
      // load game data
      //

      struct player_controller : public game::world_t::player_controller_t {
         player_controls_t const & controls_;
         player_controller(player_controls_t const & controls) : controls_(controls) {}

         virtual glm::vec2 get_relative_velocity() const override {
            if (!controls_.is_moving()) return{};
            return controls_.direction();
         }
      };

      class sprite_repository : public game::world_view_t::sprite_repository_t {
      public:
         sprite_repository(game::creature_info_table const & creature_db)
            : creature_db_(creature_db)
            , player_sprite_sheet_({ "../res/dude-walk.png" }, 64, 72)
            , bullet_sprite_sheet_({ "../res/sprites.png" }, {
               { { 0, 608 }, { 32, 32 } },
               { { 0, 544 }, { 64, 64 } },
            })
            , player_sprite_(
            {
               { player_sprite_sheet_, { 0, 1, 2, 3, 4, 5, 6, 7 } },
               { player_sprite_sheet_, { 8, 9, 10, 11, 12, 13, 14, 15 } },
               { player_sprite_sheet_, { 16, 17, 18, 19, 20, 21, 22, 23 } },
               { player_sprite_sheet_, { 24, 25, 26, 27, 28, 29, 30, 31 } },
               { player_sprite_sheet_, { 32, 33, 34, 35, 36, 37, 38, 39 } },
               { player_sprite_sheet_, { 40, 41, 42, 43, 44, 45, 46, 47 } },
               })
            , bullet_sprite_(
            {
               { bullet_sprite_sheet_, {0} }
            })
            , big_rock_sprite_(
            {
               { bullet_sprite_sheet_, {1} }
            })
         { }

         gl::sprite_t const & find_creature_sprite(game::creature_t const & creature) const override {
            return player_sprite_;
         }

         float creature_sprite_updating(std::size_t db_idx, game::creature_t const & creature, gl::sprite_cursor_t & cursor, game::moment_t & moment) const override {
            auto const & plan = creature_db_.plan(db_idx);
            if (plan.type == game::plan_t::type_t::walk_on_spot) return 1.;

            if (moment.speed() < 10.) {
               cursor.set_idx(0);
               return 0.;
            }

            auto y = moment.dir().y;
            auto abs_x = std::abs(moment.dir().x);
            if (abs_x > 0.25f) {
               if (y < -.1f) cursor.set_animation_idx(1);
               else if (y > 0.1f) cursor.set_animation_idx(3);
               else cursor.set_animation_idx(2);
            }
            else if (moment.dir().y > 0.5f) cursor.set_animation_idx(4);
            else cursor.set_animation_idx(0);

            return 1.;
         }

         gl::sprite_t const & find_particle_sprite(game::particle_t const & particle) const override {
            return bullet_sprite_;
         }

         float particle_sprite_updating(std::size_t db_idx, game::particle_t const & particle, gl::sprite_cursor_t & cursor, game::moment_t & moment) const override {
            return 1.;
         }

      private:
         game::creature_info_table const & creature_db_;

         gl::sprite_sheet player_sprite_sheet_;
         gl::sprite_sheet bullet_sprite_sheet_;
         gl::sprite_t player_sprite_;
         gl::sprite_t bullet_sprite_;
         gl::sprite_t big_rock_sprite_;
      };

      game::creature_info_table creature_db;
      game::particle_info_table particle_db;

      player_controller controller(controls);
      sprite_repository sprite_repository(creature_db);

      game::world_t world(creature_db, particle_db, controller);
      game::world_view_t world_view(creature_db, particle_db, sprite_repository);

      for (auto i = 0; i < 10; i++) {
         auto loc = glm::vec2(std::rand() % 700 - 350, std::rand() % 500 - 250);
         world.create_creature(game::creature_t::types::person, { loc, {} });
      }

      struct render_callback_t : public gl::pass_t::render_callback {
         render_callback_t(game::world_view_t::iterator itBegin, game::world_view_t::iterator itEnd)
         : itBegin_(itBegin)
         , itEnd_(itEnd)
         , it_(itBegin_) {
         }

         virtual bool prepare_next(gl::program & p) const override {
            if (it_ == itEnd_) return false;

            auto & current_render_info = *it_;
            auto tex_id = current_render_info.tex_id;

            bool should_set_texture = (it_ == itBegin_) || (tex_id != current_tex_id_);
            current_tex_id_ = tex_id;

            auto & sprite = *current_render_info.sprite;
            auto & moment = *current_render_info.moment;

            p.uniform("model").set(moment.transform());

            if (should_set_texture) {
               auto sprite_tex = sprite.current_animation().texture();
               p.uniform("texture_wh").set(glm::vec2(sprite_tex.width(), sprite_tex.height()));
               gl::texture_unit_t tex_unit{ 1 };
               tex_unit.activate();
               sprite_tex.bind();
               p.uniform("texture").set(tex_unit);
            }
            p.uniform("sprite_xy").set(sprite.current_frame().position);
            p.uniform("sprite_wh").set(sprite.current_frame().dimensions);

            it_++;
            return true;
         }

      private:
         game::world_view_t::iterator itBegin_;
         game::world_view_t::iterator itEnd_;
         mutable game::world_view_t::iterator it_;
         mutable gl::texture_t::id_type current_tex_id_ = 0;

         render_callback_t(render_callback_t const &) {}
         render_callback_t & operator=(render_callback_t const &) { return *this; }
      };


      controls.register_action_handler(gl::Key::KEY_SPACE, gl::KeyAction::KEY_ACTION_PRESS, [&](gl::Key, gl::KeyAction){
         auto & player = creature_db.moment(world.player_id());
         auto bullet_pos = player.pos() + glm::vec2(0., 30.f) + player.dir() * 40.f;
         auto bullet_vel = player.vel() + player.dir() * 400.f;
         auto bullet_moment = game::moment_t{ bullet_pos, bullet_vel };

         auto bullet_id = world.create_particle(game::particle_t::bullet, bullet_moment, 2.f);

         return true;
      });


      // 
      // load shaders
      //

      auto prg_2d = create_program("2d");
      auto prg_sprite = create_program("sprite");
      auto prg_post = create_program("post");


      //
      // load vertex data
      //

      static const float unit_square_verts[] = {
         -1., 1., 0., 1.,
         1., 1., 1., 1.,
         -1., -1., 0., 0.,
         1., -1., 1., 0.,
      };

      static const unsigned short unit_square_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto screen_vertices_spec = gl::describe_buffer({ unit_square_verts, unit_square_indices })
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

      auto bg_pass = prg_2d.pass()
         .with(screen_vertices_spec)
         .set_uniform("texture", gl::texture_t{ "bg_green.png" });

      auto fbo = std::unique_ptr<gl::frame_buffer_t>(new gl::frame_buffer_t(context.win().frame_buffer_dims()));

      auto post_pass = prg_post.pass()
         .with(screen_vertices_spec)
         .set_uniform("texture", fbo->texture())
         .set_uniform_action("t", set_time_cb);

      static const float sprite_verts[] = {
         -.5, 1., 0., 1.,
         .5, 1., 1., 1.,
         -.5, 0., 0., 0.,
         .5, 0., 1., 0.,
      };

      static const unsigned short sprite_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto sprite_vertices_spec = gl::describe_buffer({ sprite_verts, sprite_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);

      auto sprite_pass = prg_sprite.pass()
         .with(sprite_vertices_spec)
         .set_uniform("proj", glm::ortho<float>(0., 800., 0., 600.));


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

         auto dims = context.win().frame_buffer_dims();

         if (!fbo || fbo->dims() != dims) {
            fbo = std::unique_ptr<gl::frame_buffer_t>(new gl::frame_buffer_t(dims));
            post_pass.set_uniform("texture", fbo->texture());
         }

         double this_tick = gl::get_time();
         double time_since_last_tick = this_tick - last_tick;

         world.update(time_since_last_tick);
         world_view.update(time_since_last_tick);

         glClearColor(.7f, .87f, .63f, 1.);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glViewport(0, 0, dims.x, dims.y);

         fbo->bind();
         {
            bg_pass.draw(gl::DrawMode::Triangles);

            sprite_pass.draw_batch(
               render_callback_t{
                  world_view.creatures_begin(),
                  world_view.creatures_end() },
                  gl::DrawMode::Triangles);

            sprite_pass.draw_batch(
               render_callback_t{
               world_view.particles_begin(),
               world_view.particles_end() },
               gl::DrawMode::Triangles);
         }
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
