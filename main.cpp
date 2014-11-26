#ifdef WIN32
#  include <windows.h>
#  include <GLES2/gl2.h>
#else  // LINUX, OSX
#  include <GL/glew.h>

#  define USE_GLEW
#endif

#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <map>
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

   //
   // game classes
   //

   // turns key presses into a directional vector. consumed by entity controller
   class player_controls_t {
   public:
      enum class dir { none = 0, up = 1, down = 2, left = 4, right = 8 };

      player_controls_t() : dir_flags_(0), direction_(0., 0.) {
      }

      void handle_key_action(gl::Key key, gl::KeyAction action) {
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

      bool is_moving() const { return dir_flags_ != 0; }

      glm::vec2 const & direction() const { return direction_; }

   private:
      std::uint16_t dir_flags_;
      glm::vec2 direction_;
   };

   // holds geometric (pos, dir) info of a game entity
   class game_entity_t {
   public:
      game_entity_t() : state_(std::make_shared<state>()) {}

      glm::mat4 transform() const {
         float x = 400.f + pos().x;
         float y = 300.f + pos().y;

         auto moved = glm::translate(glm::mat4{}, glm::vec3{ x, y, 0. });

         if (dir().x < 0) return glm::scale(moved, glm::vec3{ -1., 1., 1. });
         return moved;
      }

      glm::vec2 const & dir() const { return state_->dir_; }
      glm::vec2 const & pos() const { return state_->pos_; }

      bool is_moving() const { return glm::length(dir()) > 0.1; }

      void set_dir(glm::vec2 const & dir) { state_->dir_ = dir; }
      void set_pos(glm::vec2 const & pos) { state_->pos_ = pos; }

      void update(double time_since_last) {
         auto & _ = *state_;
         _.pos_ += _.dir_ * (static_cast<float>(time_since_last)* _.speed_per_sec_);

         if (_.pos_.x < -400.) _.pos_.x = -400.;
         if (_.pos_.x > 400.) _.pos_.x = 400.;
         if (_.pos_.y < -300.) _.pos_.y = -300.;
         if (_.pos_.y > 300.) _.pos_.y = 300.;
      }

   private:
      struct state {
         state() : speed_per_sec_(400.) {}

         float speed_per_sec_;
         glm::vec2 pos_;
         glm::vec2 dir_;

      };

      std::shared_ptr<state> state_;
   };

   // updates an entity based on various inputs (user input or AI)
   class game_entity_controller_t {
   public:
      game_entity_controller_t(game_entity_t entity)
         : state_(new state(std::move(entity), default_plan())) {}
      game_entity_controller_t(game_entity_t entity, player_controls_t const & controls)
         : state_(new state(std::move(entity), player_plan(controls))) {}

      game_entity_t const & entity() { return state_->entity_; }

      void update(double time_since_last) {
         auto & _ = *state_;
         _.plan_->update(time_since_last, _.entity_);
      }

   private:
      struct plan_t {
         virtual ~plan_t() {}
         virtual void update(double time_since_last, game_entity_t & entity) = 0;
      };

      static std::unique_ptr<plan_t> default_plan() {
         struct plan : public plan_t {
            virtual void update(double time_since_last, game_entity_t & entity) {
               entity.update(time_since_last);
            }
         };
         return std::unique_ptr<plan_t>(new plan);
      }

      static std::unique_ptr<plan_t> player_plan(player_controls_t const & controls) {
         struct plan : public plan_t {
            plan(player_controls_t const & controls) : controls_(controls) {}
            virtual void update(double time_since_last, game_entity_t & entity) {
               entity.set_dir(controls_.direction());
               entity.update(time_since_last);
            }
            player_controls_t const & controls_;
         };
         return std::unique_ptr<plan_t>(new plan(controls));
      }

      struct state {
         state(game_entity_t entity, std::unique_ptr<plan_t> plan)
            : entity_(std::move(entity))
            , plan_(std::move(plan))
         {}

         game_entity_t entity_;
         std::unique_ptr<plan_t> plan_;
      };

      std::shared_ptr<state> state_;
   };

   // syncs a sprite representation based on the state of an entity (play walk animation when entity is moving)
   class game_entity_sprite_binding_t {
   public:
      using update_sprite_f = std::function < void(double, gl::sprite_t &, game_entity_t &) >;

      template <typename FuncT>
      game_entity_sprite_binding_t(gl::sprite_t sprite, game_entity_t entity, FuncT func)
         : state_(std::make_shared<state>(std::move(sprite), std::move(entity), func)) {
      }

      gl::sprite_t const & sprite() const { return state_->sprite_; }
      game_entity_t const & entity() const { return state_->entity_; }

      void update(double t) {
         state_->update_func_(t, state_->sprite_, state_->entity_);
      }

   private:
      struct state {
         template <typename FuncT>
         state(gl::sprite_t sprite, game_entity_t entity, FuncT func)
            : sprite_(std::move(sprite))
            , entity_(std::move(entity))
            , update_func_(func) {
         }

         gl::sprite_t sprite_;
         game_entity_t entity_;
         update_sprite_f update_func_;
      };

      std::shared_ptr<state> state_;
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

      auto create_player_sprite = [&sprites]()->gl::sprite_t {
         return{
            { sprites, { 0 } },
            { sprites, { 1, 2, 3, 4, 5 } }
         };
      };


      //
      // load game data
      //

      auto player_entity = game_entity_t{};

      std::vector<game_entity_controller_t> entity_controllers;
      entity_controllers.emplace_back(player_entity, controls);

      std::vector<game_entity_sprite_binding_t> sprite_bindings;
      sprite_bindings.emplace_back(create_player_sprite(), player_entity, [](double t, gl::sprite_t & s, game_entity_t & e){
         if (!e.is_moving()) s.set_animation_idx(0);
         else s.set_animation_idx(1);

         s.update(t);
      });


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

         for (auto & e : entity_controllers) { e.update(time_since_last_tick); }
         for (auto & b : sprite_bindings) { b.update(time_since_last_tick); }

         glClearColor(.7f, .87f, .63f, 1.);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glViewport(0, 0, dims.x, dims.y);

         bg_pass.draw_to(*fbo, gl::DrawMode::Triangles);

         for (auto & s : sprite_bindings) {
            sprite_pass
               .set_uniform("model", s.entity().transform())
               .set_uniform("sprite_xy", s.sprite().current_frame().position)
               .set_uniform("sprite_wh", s.sprite().current_frame().dimensions)
               .draw_to(*fbo, gl::DrawMode::Triangles);
         }
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
