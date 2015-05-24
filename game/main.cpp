#ifdef _MSC_VER
#  include <windows.h>
#endif

#include <glm/gtc/type_ptr.hpp>

#include <glpp/glpp.h>
#include <glpp/assets.h>
#include <glpp/utils.h>
#include <glpp/scene.h>

#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

#include <algorithm>
#include <array>
#include <cstdlib>
#include <map>

#include "game.h"
#include "render_3d.h"


// TODO:
// - fire mesh
// - grass texture
// - pass lights in uniforms
// - day/night cycle
// - text
// - audio

namespace gl {
#ifdef _MSC_VER
   using namespace gles2;
#else
   using namespace gl2;
#endif
}

namespace {
   // turns key presses into a directional vector. consumed by entity controller
   class player_controls_t {
   public:
      using action_func = std::function < bool(glpp::Key, glpp::KeyAction) > ;

      player_controls_t()
         : dir_flags_(0), direction_(0., 0.) {
      }

      void register_action_handler(glpp::Key k, action_func f) { register_action_handler(k, glpp::KeyAction::KEY_ACTION_PRESS, f); register_action_handler(k, glpp::KeyAction::KEY_ACTION_RELEASE, f); }
      void register_action_handler(glpp::Key k, glpp::KeyAction a, action_func f) { actions_[{k, a}] = f; }
      void register_action_handlers(std::vector<glpp::Key> ks, action_func f) { register_action_handlers(ks, glpp::KeyAction::KEY_ACTION_PRESS, f); register_action_handlers(ks, glpp::KeyAction::KEY_ACTION_RELEASE, f); }
      void register_action_handlers(std::vector<glpp::Key> ks, glpp::KeyAction a, action_func f) { for (auto k : ks) register_action_handler(k, a, f); }

      bool is_moving() const { return dir_flags_ != 0; }
      glm::vec2 const & direction() const { return direction_; }

      void handle_key_action(glpp::Key key, glpp::KeyAction action) {
         auto action_it = actions_.find({ key, action });
         if (actions_.end() != action_it) {
            if (action_it->second(key, action)) return;
         }

         enum class dir { none = 0, up = 1, down = 2, left = 4, right = 8 };

         auto key_dir = [key] {
            switch (key) {
            case glpp::KEY_A: case glpp::KEY_LEFT: return dir::left;
            case glpp::KEY_S: case glpp::KEY_DOWN: return dir::down;
            case glpp::KEY_D: case glpp::KEY_RIGHT: return dir::right;
            case glpp::KEY_W: case glpp::KEY_UP: return dir::up;
            default: return dir::none;
            }
         }();

         if (key_dir == dir::none && action == glpp::KeyAction::KEY_ACTION_PRESS) {
            utils::log(utils::LOG_INFO, "key pressed: %s\n", glpp::to_string(key).c_str());
            return;
         }
         if (action == glpp::KEY_ACTION_PRESS) dir_flags_ |= static_cast<uint16_t>(key_dir);
         if (action == glpp::KEY_ACTION_RELEASE) dir_flags_ &= ~static_cast<uint16_t>(key_dir);

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
      using action_key = std::pair < glpp::Key, glpp::KeyAction > ;
      std::map<action_key, action_func> actions_;
   };
}



#ifdef _MSC_VER
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


   player_controls_t controls;

   try {
      bool should_reload_program = false;
      auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
         if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
            ctx.win().set_should_close();
         if (key == glpp::KEY_R && action == glpp::KEY_ACTION_PRESS)
            should_reload_program = true;

         controls.handle_key_action(key, action);
      };
      
      game::render_context context{ key_handler };
      game::renderer view { context };

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
         sprite_repository(glpp::archive_t const & assets, game::creature_info_table const & creature_db)
            : creature_db_(creature_db)
            , player_scene_(assets.load_scene("dude-anim.fbx"))
         { }

         static unsigned animation_idx_of(game::plan_t const & plan) {
            return 0;
         }

         glpp::animation_timeline_t find_sprite(game::creature_t const & creature, game::plan_t const & plan) const override {
            return player_scene_.create_timeline(player_scene_.animation_names()[0]);
         }

         void creature_updated(std::size_t db_idx, game::creature_t const & creature, glpp::animation_timeline_t & cursor, game::moment_t & moment, game::plan_t & plan) const override {
            auto scene_idx = animation_idx_of(plan);

            // creature doing something new - start a new animation
            if (cursor.scene_idx() != scene_idx) cursor = find_sprite(creature, plan);
         }

         glpp::animation_timeline_t find_sprite(game::particle_t const & particle) const override {
            throw std::runtime_error("unimplemented");
         }

         void particle_updated(std::size_t db_idx, game::particle_t const & particle, glpp::animation_timeline_t & cursor, game::moment_t & moment) const override {
            throw std::runtime_error("unimplemented");
         }

      private:
         game::creature_info_table const & creature_db_;

         glpp::scene_t player_scene_;
      };

      //
      // populate world
      //

      game::creature_info_table creature_db;
      game::particle_info_table particle_db;

      player_controller controller(controls);
      sprite_repository sprite_repository(context.assets, creature_db);

      game::world_t world(creature_db, particle_db, controller);
      game::world_view_t world_view(creature_db, particle_db, sprite_repository);

      for (auto i = 0; i < 20; i++) {
         world.create_creature(game::creature_t::types::person, { game::random_world_location(), {} });
      }

      // world.create_creature(game::creature_t::types::person, { {0., 0.}, {} });

      //
      // set up key bindings
      //

      controls.register_action_handler(glpp::Key::KEY_SPACE, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         auto & player = creature_db.moment(world.player_id());
         auto bullet_pos = player.pos() + glm::vec2(0., 30.f) + player.dir() * 40.f;
         auto bullet_vel = player.vel() + player.dir() * 400.f;
         auto bullet_moment = game::moment_t{ bullet_pos, bullet_vel };

         auto bullet_id = world.create_particle(game::particle_t::bullet, bullet_moment, 2.f);

         return true;
      });
      
      controls.register_action_handler(glpp::Key::KEY_X, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view.debug_do_special_thing();
         return true;
      });
      controls.register_action_handlers({glpp::Key::KEY_KP_ADD, glpp::Key::KEY_EQUAL}, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view.set_view_height(view.get_view_height() + .2f);
         return true;
      });
      controls.register_action_handlers({glpp::Key::KEY_KP_SUBTRACT, glpp::Key::KEY_MINUS}, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view.set_view_height(view.get_view_height() - .2f);
         return true;
      });

      controls.register_action_handlers(glpp::Keys::Numbers, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key k, glpp::KeyAction){
         int selected_item = k - glpp::Key::KEY_0;
         view.debug_show_mesh(selected_item);
         return true;
      });

      controls.register_action_handler(glpp::Key::KEY_TAB, [&](glpp::Key, glpp::KeyAction a){
         auto enable = a == glpp::KeyAction::KEY_ACTION_PRESS;
         view.debug_enable_special_mode(enable);
         return true;
      });


      // get a GL name for our texture
      // load data into texture
      // bind texture id to texture unit
      // bind texture unit to uniform

      // https://open.gl/textures has a good run-down

      // https://github.com/adobe/angle adobe wrapper??

      controls.register_action_handler(glpp::Key::KEY_F, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         context.toggle_fullscreen();
         return true;
      });




      //
      // game loop
      //

      double last_tick = glpp::get_time();

      while (!context.closing())
      {
         if (should_reload_program) {
            context.reload_shaders();
            should_reload_program = false;
         }

         double this_tick = glpp::get_time();
         double time_since_last_tick = this_tick - last_tick;

         const double MAX_TICK_SECONDS = 1. / 15.;
         if (time_since_last_tick > MAX_TICK_SECONDS) time_since_last_tick = MAX_TICK_SECONDS;

         //
         // update world
         //

         world.update(time_since_last_tick);
         world_view.update(time_since_last_tick);


         //
         // render
         //

         view.update_and_render(time_since_last_tick, world_view);

         last_tick = this_tick;

         context.swap();
      }
   }
   catch (glpp::shader_compile_error const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }

   glpp::shutdown();

   exit(EXIT_SUCCESS);
}
