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


   struct sprite_render_callback_t : public gl::pass_t::render_callback {
      sprite_render_callback_t(game::entity_sprites_t const & sprites)
         : tex_sprite_lookup_idx_(0), sprites_(sprites) {
         for (auto idx = 0U; idx < sprites_.count(); idx++) {
            auto & s = sprites_.sprite_at(idx);
            sprite_indices_by_tex_id.push_back({s.current_animation().texture().id(), idx});
         }

         auto cmp_textures = [](tex_sprite_index const & a, tex_sprite_index const & b) {
            return a.tex_id < b.tex_id;
         };

         std::sort(std::begin(sprite_indices_by_tex_id), std::end(sprite_indices_by_tex_id), cmp_textures);
      }

      virtual bool prepare_next(gl::program & p) const {
         if (tex_sprite_lookup_idx_ == sprite_indices_by_tex_id.size()) return false;
         auto tex_id = sprite_indices_by_tex_id[tex_sprite_lookup_idx_].tex_id;
         auto sprite_idx = sprite_indices_by_tex_id[tex_sprite_lookup_idx_].sprite_idx;

         bool set_texture = 0 == tex_sprite_lookup_idx_ || (tex_id != sprite_indices_by_tex_id[tex_sprite_lookup_idx_ - 1].tex_id);

         auto & e = sprites_.entity_at(sprite_idx);// entities.entity(player_entity_id);
         auto & s = sprites_.sprite_at(sprite_idx);// sprites.entity_sprite(player_entity_id);

         p.uniform("model").set(e.transform());

         if (set_texture) {
            auto sprite_tex = s.current_animation().texture();
            p.uniform("texture_wh").set(glm::vec2(sprite_tex.width(), sprite_tex.height()));
            gl::texture_unit_t tex_unit{ 1 };
            tex_unit.activate();
            sprite_tex.bind();
            p.uniform("texture").set(tex_unit);
         }
         p.uniform("sprite_xy").set(s.current_frame().position);
         p.uniform("sprite_wh").set(s.current_frame().dimensions);

         tex_sprite_lookup_idx_++;
         return true;
      }

      mutable std::size_t tex_sprite_lookup_idx_;
      game::entity_sprites_t const & sprites_;
      struct tex_sprite_index { gl::texture_t::id_type tex_id; std::size_t sprite_idx; };
      std::vector<tex_sprite_index> sprite_indices_by_tex_id;
   };
}




//struct bullet_sprite_engine {
//   bullet_sprite_engine(gl::program & prg)
//      : bullet_sprite_sheet_({ "../res/bullet.png" })
//   {
//      static const float sprite_verts[]; = {
//         -35., 93., 0., 1.,
//         34., 93., 1., 1.,
//         -35., 0., 0., 0.,
//         34., 0., 1., 0.,
//      };
//
//      static const unsigned short sprite_indices[] = {
//         0, 2, 1,
//         1, 2, 3,
//      };
//
//      auto sprite_vertices_spec = gl::describe_buffer({ sprite_verts, sprite_indices })
//         .attrib("p", 2)
//         .attrib("tex_coords", 2);
//
//      auto sprite_tex = sprite_sheet.texture();
//
//      auto sprite_pass = prg_sprite.pass()
//         .with(sprite_vertices_spec)
//         .set_uniform("proj", glm::ortho<float>(0., 800., 0., 600.));
//
//      auto create_bullet_sprite = [&bullet_sprite_sheet]()->gl::sprite_t {
//         return{
//            { bullet_sprite_sheet, { 0 } }
//         };
//      };
//
//   }
//
//   gl::sprite_sheet bullet_sprite_sheet_;
//   gl::pass_t pass_;
//};

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

      gl::sprite_sheet sprite_sheet({ "../res/kenney_platformer_graphics/Player/p1_walk/p1_walk.png" }, {
         { { 0, 420 }, { 66, 92 } },
         { { 66, 419 }, { 66, 92 } },
         { { 133, 420 }, { 66, 92 } },
         { { 0, 326 }, { 66, 92 } },
         { { 133, 420 }, { 66, 92 } },
         { { 66, 420 }, { 66, 92 } },
      });

      gl::sprite_sheet bullet_sprite_sheet({ "../res/bullet.png" });

      //
      // load game data
      //


      struct player_entity_controller : public game::entities_t::controller {
         player_controls_t const & controls_;

         player_entity_controller(player_controls_t const & controls) : controls_(controls) { }

         virtual void update(double t, game::entities_t & entities, game::entity_id_t eid, game::entity_moment_t & e) {
            e.set_dir(controls_.direction());
            auto target_vel = controls_.direction() * (static_cast<float>(t)* 500.f);
            e.set_vel(utils::lerp(e.vel(), target_vel, 7.f * static_cast<float>(t)));
            e.update(t);

            auto pos = e.pos();
            if (pos.x < -400.) pos.x = -400.;
            if (pos.x > 400.) pos.x = 400.;
            if (pos.y < -300.) pos.y = -300.;
            if (pos.y > 300.) pos.y = 300.;
            e.set_pos(pos);
         }

         static std::unique_ptr<player_entity_controller> create(player_controls_t const & c) { return std::unique_ptr<player_entity_controller>(new player_entity_controller(c)); }
      };

      struct player_sprite_controller : public game::entity_sprites_t::controller {
         virtual void update(double t, game::entity_moment_t const & e, gl::sprite_cursor_t & s) {
            if (!e.is_moving()) s.set_animation_idx(0);
            else s.set_animation_idx(1);

            s.advance(t);
         }

         static std::unique_ptr<player_sprite_controller> create() { return std::unique_ptr<player_sprite_controller>(new player_sprite_controller); }
      };


      struct bullet_entity_controller : public game::entities_t::controller {
         virtual void update(double t, game::entities_t & entities, game::entity_id_t eid, game::entity_moment_t & e) {
            e.update(t);
            if (glm::length(e.pos()) > 600.f) {
               entities.destroy_entity(eid);
            }
         }

         static std::unique_ptr<bullet_entity_controller> create() { return std::unique_ptr<bullet_entity_controller>(new bullet_entity_controller()); }
      };

      game::entities_t entities;
      game::entity_sprites_t sprites(entities);
      entities.register_controller("player", player_entity_controller::create(controls));
      entities.register_controller("bullet", bullet_entity_controller::create());
      sprites.register_controller("player", player_sprite_controller::create());
      sprites.register_controller("bullet", sprites.simple_controller());

      auto create_player_sprite = [&sprite_sheet]()->gl::sprite_t {
         return{
            { sprite_sheet, { 0 } },
            { sprite_sheet, { 1, 2, 3, 4, 5 } }
         };
      };

      auto create_bullet_sprite = [&bullet_sprite_sheet]()->gl::sprite_t {
         return{
            { bullet_sprite_sheet, {0} }
         };
      };

      auto player_entity_id = entities.create_entity({}, "player");
      sprites.register_entity_sprite(player_entity_id, create_player_sprite(), "player");

      controls.register_action_handler(gl::Key::KEY_SPACE, gl::KeyAction::KEY_ACTION_PRESS, [&](gl::Key, gl::KeyAction){
         auto & player_entity = entities.entity(player_entity_id);
         auto bullet_pos = player_entity.pos() + player_entity.dir() * 40.f;
         auto bullet_vel = player_entity.vel() + player_entity.dir() * 0.5f;
         auto bullet_moment = game::entity_moment_t{bullet_pos, bullet_vel};
         auto bullet_id = entities.create_entity(bullet_moment, "bullet");
         sprites.register_entity_sprite(bullet_id, create_bullet_sprite(), "bullet");

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

      auto sprite_tex = sprite_sheet.texture();

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

         entities.update(time_since_last_tick);
         sprites.update(time_since_last_tick);

         glClearColor(.7f, .87f, .63f, 1.);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glViewport(0, 0, dims.x, dims.y);

         fbo->bind();
         bg_pass.draw(gl::DrawMode::Triangles);
         sprite_pass.draw_batch(sprite_render_callback_t{ sprites }, gl::DrawMode::Triangles);
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
