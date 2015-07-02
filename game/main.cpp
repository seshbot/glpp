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
// - fire particles
// - grass texture
// - pass lights in uniforms
// - day/night cycle
// - text
// - audio
// - bloom

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
      
      auto assets = glpp::archive_t::load_from_directory(glpp::path_t{ "../../res/" });
      game::render_context context{ assets, key_handler };

      game::model_repository scene_repo{ assets };
      game::renderer view { scene_repo, context };

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
         struct sprite_animations {
            glpp::animation_t const & standing;
            glpp::animation_t const & walking;

            static sprite_animations find_scene_animations(glpp::scene_t const & scene) {
               auto names = scene.animation_names();
               auto find_animation = [&](auto name_fragment) -> glpp::animation_t const & {
                  for (auto idx = 0U; idx < names.size(); idx++) {
                     auto & name = names[idx];
                     if (name.find(name_fragment) != std::string::npos) { return scene.animation(name); }
                  }
                  return scene.default_animation();
               };
               return{
                  find_animation("Armature|Dude.stand"),
                  find_animation("Armature|Dude.walk")
               };
            }

            static sprite_animations find_scene_animations(glpp::scene_t const & scene, std::string const & child_name) {
               auto & sprite = scene.default_animation();
               for (auto idx = 0U; idx < sprite.child_count(); idx++) {
                  auto & child_sprite = sprite.child(idx);
                  if (child_name == child_sprite.name()) return{child_sprite, child_sprite};
               }
               return{ sprite, sprite};
            }
         };
         struct scene_info {
            scene_info(glpp::scene_t const & scene)
               : scene{ scene }, animations(sprite_animations::find_scene_animations(scene)) {}
            scene_info(glpp::scene_t const & scene, std::string const & child_name)
               : scene{ scene }, animations(sprite_animations::find_scene_animations(scene, child_name)) {}
            glpp::scene_t const & scene;
            sprite_animations animations;
         };
      public:
         sprite_repository(game::model_repository const & scene_repo, game::creature_info_table const & creature_db, game::prop_info_table const & prop_db)
            : scene_repo_(scene_repo)
            , creature_db_(creature_db)
            , prop_db_(prop_db)
         {
            scenes_.push_back({ scene_repo.find_scene_by_name("dude")});
            scenes_.push_back({ scene_repo.find_scene_by_name("campfire") });
            scenes_.push_back({ scene_repo.find_scene_by_name("props"), "Tree2Trunk" });
            scenes_.push_back({ scene_repo.find_scene_by_name("props"), "Rock1Big" });
            scenes_.push_back({ scene_repo.find_scene_by_name("weapons"), "torch" });
         }

         glpp::animation_timeline_t find_sprite(game::creature_t const & entity, game::moment_t & moment, game::plan_t const & plan) const override {
            auto & scene_info = scene_info_of(entity);
            auto & animation = animation_of(scene_info, moment);
            auto sprite = animation.create_timeline();
            sprite.advance_by(std::rand() / 60.);
            return sprite;
         }

         void animate(std::size_t db_idx, game::creature_t const & entity, glpp::animation_timeline_t & cursor, game::moment_t & moment, game::plan_t & plan, double time_since_last) const override {
            auto & new_animation = animation_of(entity, moment, plan);
            if (cursor.animation() != new_animation) {
               // creature doing something new - start a new animation
               cursor = new_animation.create_timeline();
            }

            cursor.advance_by(time_since_last);
         }

         glpp::animation_timeline_t find_sprite(game::prop_t const & entity, game::moment_t & moment) const override {
            auto & scene_info = scene_info_of(entity);
            auto & animation = animation_of(scene_info, moment);
            auto sprite = animation.create_timeline();
            return sprite;
         }

         void animate(std::size_t db_idx, game::prop_t const & entity, glpp::animation_timeline_t & cursor, game::moment_t & moment, double time_since_last) const override {
            cursor.advance_by(time_since_last);
         }

         glpp::animation_timeline_t find_sprite(game::particle_t const & entity, game::moment_t & moment) const override {
            throw std::runtime_error("unimplemented");
         }

         void animate(std::size_t db_idx, game::particle_t const & entity, glpp::animation_timeline_t & cursor, game::moment_t & moment, double time_since_last) const override {
            throw std::runtime_error("unimplemented");
         }

      private:
         scene_info const & scene_info_of(game::creature_t const & entity) const {
            switch (entity.type) {
            case game::creature_t::person:
            case game::creature_t::monster:
            default:
               return scenes_[0];
            }
         }
         scene_info const & scene_info_of(game::prop_t const & entity) const {
            switch (entity.type) {
            case game::prop_t::tree:
               return scenes_[2];
            case game::prop_t::rock:
               return scenes_[3];
            case game::prop_t::torch:
               return scenes_[4];
            case game::prop_t::campfire:
            default:
               return scenes_[1];
            }
         }
         glpp::scene_t const & scene_of(game::creature_t const & entity) const {
            return scene_info_of(entity).scene;
         }
         glpp::animation_t const & animation_of(scene_info const & scene, game::moment_t & moment) const {
            if (moment.is_moving()) return scene.animations.walking;
            return scene.animations.standing;
         }
         glpp::animation_t const & animation_of(game::creature_t const & entity, game::moment_t & moment, game::plan_t const & plan) const {
            return animation_of(scene_info_of(entity), moment);
         }
         glpp::animation_t const & animation_of(game::prop_t const & entity, game::moment_t & moment) const {
            return animation_of(scene_info_of(entity), moment);
         }

         game::model_repository const & scene_repo_;
         game::creature_info_table const & creature_db_;
         game::prop_info_table const & prop_db_;

         std::vector<scene_info> scenes_;
      };

      //
      // populate world
      //

      game::creature_info_table creature_db;
      game::prop_info_table prop_db;
      game::particle_info_table particle_db;

      player_controller controller(controls);
      sprite_repository sprite_repository(scene_repo, creature_db, prop_db);

      game::world_t world(creature_db, prop_db, particle_db, controller);
      game::world_view_t world_view(creature_db, prop_db, particle_db, sprite_repository);

#if 0
      for (auto i = 0; i < 5; i++) {
         world.create_creature(game::creature_t::types::person, { game::random_world_location(), {} });
      }
#endif

      world.create_prop(game::prop_t::campfire, { { 400., 424. }, {} });

      auto place_props = [&world](game::prop_t::types prop_type, float scale, unsigned count, float min_dist) {
         auto planted = 0U;
         while (planted < count) {
            auto loc = game::random_world_location();
            auto dist = glm::distance(loc, game::center_world_location());
            if (dist < min_dist) continue;
            float variation = static_cast<float>((float)(std::rand() % 1000) / 1000. - .5) * .5f;
            float varied_scale = scale + variation * scale;
            world.create_prop(prop_type, { loc, {}, game::random_direction(), varied_scale });
            planted++;
         }
      };

      place_props(game::prop_t::tree, 3., 15, 250);
      place_props(game::prop_t::rock, .5, 20, 100);

      //world.create_creature(game::creature_t::types::person, { game::center_world_location(), {} });

      //
      // set up key bindings
      //

      controls.register_action_handler(glpp::Key::KEY_SPACE, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         auto & player = creature_db.moment(world.player_id());
         auto bullet_pos = player.pos() + glm::vec2(0., 30.f) + player.dir() * 40.f;
         auto bullet_vel = player.vel() + player.dir() * 400.f;
         auto bullet_moment = game::moment_t{ bullet_pos, bullet_vel };

         //auto bullet_id = world.create_particle(game::particle_t::bullet, bullet_moment, 2.f);

         return true;
      });

      controls.register_action_handler(glpp::Key::KEY_X, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view.debug_do_special_thing();
         return true;
      });
      controls.register_action_handler(glpp::Key::KEY_P, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view.debug_set_ortho(!view.ortho);
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

      controls.register_action_handler(glpp::Key::KEY_F, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         context.toggle_fullscreen();
         return true;
      });


      // TODO: create 'config' files for controls, game creatures and repository models


      static auto SAMPLE_TEXT = R"(a line of text
another line of text with some data
whats going on another line of text with some data
another line but something
what is going on here
something should really happen here
this is weird)";



      auto ui_transform = [&] {
         auto xres = (float)context.context.win().frame_buffer_dims().x / 2.f;
         auto yres = (float)context.context.win().frame_buffer_dims().y / 2.f;

         auto xpad = xres / 100.f;
         auto ypad = xpad * .75f;
         auto mvp = glm::ortho(0.f, xres, yres, 0.f);
         auto m = glm::translate(glm::vec3{ xpad, ypad, 0.f });

         return mvp * m;
      };
      glpp::program ui_prg = glpp::make_debug_ui_program(); // create_program(context.assets, "ui");
      glpp::pass_t ui_pass = glpp::make_debug_text_pass(SAMPLE_TEXT, ui_prg, ui_transform());


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

         auto player_moment = creature_db.moment(world.player_id());
         auto torch_world_pos = player_moment.pos() + 45.f * glm::rotate(player_moment.dir(), -.5f);
         auto torch_3d_pos = glm::vec3{ torch_world_pos.x, 45., -torch_world_pos.y };
         view.set_player_light_position(torch_3d_pos);
         view.update_and_render(time_since_last_tick, world_view);

         gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
         ui_pass.draw(glpp::DrawMode::Triangles);

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
