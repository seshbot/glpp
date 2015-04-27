#ifdef _MSC_VER
#  include <windows.h>
#endif

#include <glm/gtc/type_ptr.hpp>

#include <map>

#include "game.h"
#include <glpp/glpp.h>
#include <glpp/utils.h>
#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

#include <algorithm>
#include <array>
#include <cstdlib>

#include <glpp/scene.h>

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
   glpp::shader vert_shader(std::string name) { return glpp::shader::create_from_file(utils::fmt("../../shaders/%s.vert", name.c_str()), glpp::shader::Vertex); }
   glpp::shader frag_shader(std::string name) { return glpp::shader::create_from_file(utils::fmt("../../shaders/%s.frag", name.c_str()), glpp::shader::Fragment); }

   glpp::program create_program(std::string name) {
      utils::log(utils::LOG_INFO, "compiling '%s' shader programs... ", name.c_str());

      auto program = glpp::program{ vert_shader(name), frag_shader(name) };

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }

      return program;
   };

   void reload_program(glpp::program & program, std::string name) {
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

   // TODO: lifetime (create/delete), movement (update), render (vertices, buffer) policies
   template <typename CreatePolicy, typename UpdatePolicy, typename DeletePolicy>
   class particle_emitter_buffer_t
      : public CreatePolicy   // provides create_particles(emitter, t)
      , public UpdatePolicy   // provides particle_created(emitter, idx, t), update_particle(emitter, idx, t)
      , public DeletePolicy { // provides should_delete(emitter, idx)
   public:
      using cont_t = std::vector < glm::vec3 >;
      using idx_t = std::size_t;

      void update(double time_since_last) {
         current_time_ += time_since_last;

         auto prev_top = count();
         CreatePolicy::create_particles(*this, time_since_last);

         // glm likes floats
         auto t = static_cast<float>(time_since_last);
         auto new_top = count();

         for (auto idx = prev_top; idx < new_top; idx++) {
            UpdatePolicy::particle_created(*this, idx);
         }

         auto idx = idx_t(0);
         while (idx < count()) {
            UpdatePolicy::update_particle(*this, idx, t);

            if (DeletePolicy::should_delete(*this, idx)) {
               del_at(idx);
            }
            else {
               // only increment if we didnt delete an element
               idx++;
            }
         }

         if (particle_positions_.size() > 0)
            buffer_.update({ glm::value_ptr(particle_positions_[0]), particle_positions_.size() * 3 });
      }

      glpp::buffer_t buffer() { return buffer_; }

      double time() const { return current_time_; }
      idx_t count() const { return particle_velocities_.size(); }
      idx_t add(glm::vec3 pos, glm::vec3 vel = {}) { particle_create_times_.push_back(current_time_);  particle_positions_.push_back(pos); particle_velocities_.push_back(vel); return count() - 1; }
      void del_at(idx_t idx) { del_at_(particle_create_times_, idx);  del_at_(particle_positions_, idx); del_at_(particle_velocities_, idx); }
      double create_time_at(idx_t idx) { return particle_create_times_[idx]; }
      glm::vec3 & pos_at(idx_t idx) { return particle_positions_[idx]; }
      glm::vec3 & vel_at(idx_t idx) { return particle_velocities_[idx]; }

   private:
      glpp::buffer_t buffer_ = {glpp::buffer_t::Usage::Dynamic};
      double current_time_ = 0.;
      std::vector<double> particle_create_times_;
      std::vector<glm::vec3> particle_positions_;
      std::vector<glm::vec3> particle_velocities_;

      template <typename T>
      static void del_at_(std::vector<T> & vec, idx_t idx) { auto last = vec.size() - 1; if (idx != last) std::swap(vec[idx], vec[last]); vec.erase(vec.begin() + last); }
   };

   template <unsigned CREATE_PER_SEC>
   class constant_create_policy_t {
   protected:
      const double TIME_BETWEEN_CREATE = (double)1. / CREATE_PER_SEC;
      double next_create_time_ = 0.;

      template <typename T>
      void create_particle(T & emitter) {
         auto pos = glm::vec3{
            (float)(std::rand() % 1400) - 300.f, // (float)(std::rand() % 460), // - 230.,
            800.,
            800.f - (float)(std::rand() % 1200) };// (float)(std::rand() % 350) - 250. };

         emitter.add(pos);
      }

      template <typename T>
      void create_particles(T & emitter, double time_since_last) {
         auto now = emitter.time();
         while (next_create_time_ < now) {
            create_particle(emitter);
            next_create_time_ += TIME_BETWEEN_CREATE;
         }
      }
   };

   class constant_update_policy_t {
   protected:
      using idx_t = std::size_t;

      const glm::vec3 PARTICLE_VEL = { 0., -1000., 0. };

      template <typename T>
      void particle_created(T & emitter, idx_t idx) {
         emitter.vel_at(idx) = PARTICLE_VEL;
      }

      template <typename T>
      void update_particle(T & emitter, idx_t idx, float time_since_last) {
         auto & pos = emitter.pos_at(idx);
         pos += emitter.vel_at(idx) * time_since_last;
      }
   };

   template <unsigned TTL_MS>
   class constant_time_delete_policy_t {
   public:
      const double TTL_SECS = TTL_MS / 1000.;
      using idx_t = std::size_t;

      template <typename T>
      bool should_delete(T & emitter, idx_t idx) {
         return emitter.time() - emitter.create_time_at(idx) > TTL_SECS;
      }
   };

   template <unsigned TTL_MS>
   class constant_depth_delete_policy_t {
   public:
      const double TTL_SECS = TTL_MS / 1000.;
      using idx_t = std::size_t;

      template <typename T>
      bool should_delete(T & emitter, idx_t idx) {
         return emitter.pos_at(idx).y <= 0.;
      }
   };

   using constant_particle_emitter_buffer_t = particle_emitter_buffer_t <
      constant_create_policy_t<6000>,
      constant_update_policy_t,
      constant_depth_delete_policy_t<0> >;
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
      glpp::init();
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      exit(EXIT_FAILURE);
   }

   auto model_dude = glpp::scene_t::load_from_file("../../res/dude-anim.fbx");
   auto model_campfire = glpp::scene_t::load_from_file("../../res/campfire.fbx");

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

      glpp::context context{ key_handler };
      auto init_context = [&] {
         gl::enable(gl::enable_cap_t::depth_test);
         gl::enable(gl::enable_cap_t::dither);
         gl::enable(gl::enable_cap_t::cull_face);
         //gl::enable(gl::enable_cap_t::multisample);
         gl::enable(gl::enable_cap_t::blend);

#ifndef WIN32
         gl::point_size(3.);
         gl::enable(gl::enable_cap_t::point_smooth);
#endif

         gl::blend_func(gl::blending_factor_src_t::src_alpha, gl::blending_factor_dest_t::one_minus_src_alpha);

         gl::cull_face(gl::cull_face_mode_t::back);
      };

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

      auto extensions = context.extensions();
      utils::log(utils::LOG_INFO, "%s\n", context.info(false).c_str());
      utils::log(utils::LOG_INFO, "GL EXTENSIONS [%s]:\n", std::to_string(extensions.size()).c_str());
      for (auto e : extensions)
         utils::log(utils::LOG_INFO, " - %s\n", e.c_str());

      init_context();

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
            , player_sprite_sheet_({ "../../res/dude-walk.png" }, 64, 72)
            , bullet_sprite_sheet_({ "../../res/sprites.png" }, {
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

         glpp::sprite_t const & find_creature_sprite(game::creature_t const & creature) const override {
            return player_sprite_;
         }

         float creature_sprite_updating(std::size_t db_idx, game::creature_t const & creature, glpp::sprite_cursor_t & cursor, game::moment_t & moment) const override {
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

         glpp::sprite_t const & find_particle_sprite(game::particle_t const & particle) const override {
            return bullet_sprite_;
         }

         float particle_sprite_updating(std::size_t db_idx, game::particle_t const & particle, glpp::sprite_cursor_t & cursor, game::moment_t & moment) const override {
            return 1.;
         }

      private:
         game::creature_info_table const & creature_db_;

         glpp::sprite_sheet player_sprite_sheet_;
         glpp::sprite_sheet bullet_sprite_sheet_;
         glpp::sprite_t player_sprite_;
         glpp::sprite_t bullet_sprite_;
         glpp::sprite_t big_rock_sprite_;
      };

      //
      // populate world
      //

      game::creature_info_table creature_db;
      game::particle_info_table particle_db;

      player_controller controller(controls);
      sprite_repository sprite_repository(creature_db);

      game::world_t world(creature_db, particle_db, controller);
      game::world_view_t world_view(creature_db, particle_db, sprite_repository);

      for (auto i = 0; i < 20; i++) {
         world.create_creature(game::creature_t::types::person, { game::random_world_location(), {} });
      }

      // world.create_creature(game::creature_t::types::person, { {0., 0.}, {} });

      struct sprite_render_callback_t : public glpp::pass_t::render_callback {
         sprite_render_callback_t(game::world_view_t::iterator itBegin, game::world_view_t::iterator itEnd)
         : itBegin_(itBegin)
         , itEnd_(itEnd)
         , it_(itBegin_) {
         }

         virtual bool prepare_next(glpp::program & p) const override {
            if (it_ == itEnd_) return false;

            auto & current_render_info = *it_;
            auto tex_id = current_render_info.tex_id;

            bool should_set_texture = (it_ == itBegin_) || (tex_id != current_tex_id_);
            current_tex_id_ = tex_id;

            auto & sprite = *current_render_info.sprite;
            auto & moment = *current_render_info.moment;

            p.uniform("model").set(moment.sprite_transform());

            if (should_set_texture) {
               auto sprite_tex = sprite.current_animation().texture();
               p.uniform("texture_wh").set(glm::vec2(sprite_tex.dims().x, sprite_tex.dims().y));
               glpp::texture_unit_t tex_unit{ 1 };
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
         mutable glpp::texture_t::id_type current_tex_id_ = 0;

         sprite_render_callback_t(sprite_render_callback_t const &) {}
         sprite_render_callback_t & operator=(sprite_render_callback_t const &) { return *this; }
      };


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
      
      bool do_special_thing = false;
      controls.register_action_handler(glpp::Key::KEY_X, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         do_special_thing = true;
         return true;
      });
      float view_height = 1.;
      controls.register_action_handlers({glpp::Key::KEY_KP_ADD, glpp::Key::KEY_EQUAL}, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view_height += .2f;
         view_height = std::min(1.f, view_height);
         return true;
      });
      controls.register_action_handlers({glpp::Key::KEY_KP_SUBTRACT, glpp::Key::KEY_MINUS}, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view_height -= .2f;
         view_height = std::max(0.f, view_height);
         return true;
      });

      int selected_item = 0;
      controls.register_action_handlers(glpp::Keys::Numbers, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key k, glpp::KeyAction){
         selected_item = k - glpp::Key::KEY_0;
         return true;
      });

      bool special_mode_enabled = false;
      controls.register_action_handler(glpp::Key::KEY_TAB, [&](glpp::Key, glpp::KeyAction a){
         special_mode_enabled = a == glpp::KeyAction::KEY_ACTION_PRESS;
         return true;
      });

      // 
      // load shaders
      //

      auto prg_3d = create_program("3d");
      auto prg_3d_particle = create_program("3d_particle");
      auto prg_3d_shadow = create_program("3d_shadow");
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

      auto screen_vertices_spec = glpp::describe_buffer({ unit_square_verts, unit_square_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);

      // get a GL name for our texture
      // load data into texture
      // bind texture id to texture unit
      // bind texture unit to uniform

      // https://open.gl/textures has a good run-down

      // https://github.com/adobe/angle adobe wrapper??

      auto set_time_cb = [](glpp::uniform & u){u.set(static_cast<float>(glpp::get_time())); };
      auto set_dims_cb = [&context](glpp::uniform & u){
         auto dims = context.win().frame_buffer_dims();
         u.set(glm::vec2{ dims.x, dims.y });
      };

      auto shadow_tex = std::unique_ptr<glpp::cube_map_texture_t>();
      auto post_tex = std::unique_ptr<glpp::texture_t>();
      auto shadow_fbo = std::unique_ptr<glpp::frame_buffer_t>();
      auto tex_fbo = std::unique_ptr<glpp::frame_buffer_t>();
      auto msaa_fbo = std::unique_ptr<glpp::frame_buffer_t>();

      controls.register_action_handler(glpp::Key::KEY_F, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         context.win().set_fullscreen(!context.win().is_fullscreen());
         init_context();

         // force deallocation of all FBOs after context reinit to get around this ANGLE bug:
         // https://code.google.com/p/angleproject/issues/detail?id=979
         // ideally we would let the resize-handling code re-allocate the FBOs
         shadow_fbo.reset();
         tex_fbo.reset();
         msaa_fbo.reset();
         return true;
      });

      auto set_post_tex_cb = [&post_tex](glpp::uniform & u) { glpp::texture_unit_t tu{ 2 }; tu.activate(); post_tex->bind();  u.set(tu); };
      auto post_pass = prg_post.pass()
         .with(screen_vertices_spec)
         .set_uniform_action("texture", set_post_tex_cb)
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

      auto sprite_vertices_spec = glpp::describe_buffer({ sprite_verts, sprite_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);

      auto sprite_pass = prg_sprite.pass()
         .with(sprite_vertices_spec)
         .set_uniform("proj", glm::ortho<float>(0., 800., 0., 600., -1., 1.));


      //
      // state callbacks
      //

      auto get_camera_pos_cb = [&] {
         auto center_2d = game::center_world_location();
         auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
         auto eye = center + glm::vec3{ 0.f, 800.f * view_height, 800.f };
         return eye;
      };

      auto get_view_cb = [&] {
         auto center_2d = game::center_world_location();
         auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
         auto eye = get_camera_pos_cb();
         auto result = glm::lookAt(eye, center, glm::vec3{ 0., 1., 0. });
         return result;
      };

      auto get_proj_cb = [] {
         // 0, 0 is the bottom left of the lookAt target!
         return glm::ortho<float>(-400., 400., -300., 300., 100., 2000.);
         //return glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1000.f);
      };

      auto set_view_cb = [&get_view_cb](glpp::uniform & u) {
         u.set(get_view_cb());
      };

      auto add_3d_mesh = [&prg_3d, &prg_3d_shadow](glpp::mesh_t const & mesh, std::vector<glpp::pass_t> & passes_3d, std::vector<glpp::pass_t> & passes_3d_shadow) {
         auto verts = glpp::describe_buffer({ { mesh.vertices().buffer, mesh.vertices().count }, { mesh.indices().buffer, mesh.indices().count } })
            .attrib("p", 3);
         auto normals = glpp::describe_buffer({ { mesh.normals().buffer, mesh.normals().count } })
            .attrib("normal", 3);
         auto bone_indices = glpp::describe_buffer({ { mesh.bone_indices().buffer, mesh.bone_indices().count } })
            .attrib("bone_indices", 4);
         auto bone_weights = glpp::describe_buffer({ { mesh.bone_weights().buffer, mesh.bone_weights().count } })
            .attrib("bone_weights", 4);


         passes_3d.push_back(
            prg_3d.pass()
            .with(verts)
            .with(normals)
            .with(bone_indices)
            .with(bone_weights)
            .set_uniform("colour", mesh.material().diffuse_colour));

         passes_3d_shadow.push_back(
            prg_3d_shadow.pass()
            .with(verts)
            .with(bone_indices)
            .with(bone_weights));

         auto & pass_3d = passes_3d.back();
         auto & pass_3d_shadow = passes_3d_shadow.back();

         if (mesh.is_animated()) {
            pass_3d
               .set_uniform_action("bones[0]", [&](glpp::uniform & u) { u.set(mesh.bone_transforms()); });
            pass_3d_shadow
               .set_uniform_action("bones[0]", [&](glpp::uniform & u) { u.set(mesh.bone_transforms()); });
         }
         else {
            pass_3d
               .set_uniform("bones[0]", mesh.bone_transforms());
            pass_3d_shadow
               .set_uniform("bones[0]", mesh.bone_transforms());
         }
      };

      //
      // create dude animation meshes
      //

      std::vector<glpp::pass_t> d3_body_passes;
      std::vector<glpp::pass_t> d3_body_shadow_passes;

      // NOTE
      auto animation_name = model_dude.animation_names()[0];
      auto animation_snapshot = model_dude.start_animation(animation_name);
      std::vector<std::string> d3_body_mesh_names;

      utils::log(utils::LOG_INFO, "== Animation '%s' Meshes ==\n", animation_name.c_str());
      for (auto & mesh : animation_snapshot.meshes()) {
         utils::log(utils::LOG_INFO, " - mesh %s: %d bones, %d transforms\n", mesh.name().c_str(), mesh.bone_count(), mesh.bone_transforms().size());
         d3_body_mesh_names.push_back(mesh.name());
         add_3d_mesh(mesh, d3_body_passes, d3_body_shadow_passes);
      }

      //
      // create campfire static meshes
      //

      std::vector<glpp::pass_t> d3_campfire_passes;
      std::vector<glpp::pass_t> d3_campfire_shadow_passes;

      for (auto & mesh : model_campfire.meshes()) {
         d3_body_mesh_names.push_back(mesh.name());
         add_3d_mesh(mesh, d3_campfire_passes, d3_campfire_shadow_passes);
      }


      static const float ground_verts[] = {
         0.,   0., -900.,   0., 1., 0.,
         800., 0., -900.,   0., 1., 0.,
         0.,   0.,  0.,     0., 1., 0.,
         800., 0.,  0.,     0., 1., 0.,
      };
      static const unsigned short ground_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };
      static const float ground_bone_indices[] = {
         0., 0., 0., 0.,
         0., 0., 0., 0.,
         0., 0., 0., 0.,
         0., 0., 0., 0.,
      };
      static const float ground_bone_weights[] = {
         1., 0., 0., 0.,
         1., 0., 0., 0.,
         1., 0., 0., 0.,
         1., 0., 0., 0.,
      };
      static const std::vector<glm::mat4> ground_bones = { glm::mat4{} };

      auto ground_buffer_desc = glpp::describe_buffer({ground_verts, ground_indices})
         .attrib("p", 3)
         .attrib("normal", 3);
      auto ground_bone_indices_buffer_desc = glpp::describe_buffer(glpp::static_array_t{ ground_bone_indices })
         .attrib("bone_indices", 4);
      auto ground_bone_weights_buffer_desc = glpp::describe_buffer(glpp::static_array_t{ ground_bone_weights })
         .attrib("bone_weights", 4);

      auto ground_pass = prg_3d.pass()
         .with(ground_buffer_desc)
         .with(ground_bone_indices_buffer_desc)
         .with(ground_bone_weights_buffer_desc)
         .set_uniform("colour", glm::vec4(.8f, .8f, .16f, 1.f))
         .set_uniform("model", glm::mat4{})
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_proj_cb() * get_view_cb()); })
//         .set_uniform("normal_matrix", glm::mat4{})
         .set_uniform_action("t", set_time_cb)
         .set_uniform("bones[0]", ground_bones);


      constant_particle_emitter_buffer_t emitter;
      auto set_screensize_cb = [&](glpp::uniform & u) {
         auto dims = context.win().frame_buffer_dims();
         u.set(glm::vec2{dims.x, dims.y});
      };


      auto emitter_buffer_desc = glpp::describe_buffer(emitter.buffer())
         .attrib("position", 3);
      auto particle_pass = prg_3d_particle.pass()
         .with(emitter_buffer_desc)
         .set_uniform_action("screen_size", set_screensize_cb)
         .set_uniform("proj", glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1500.f))
         .set_uniform_action("view", set_view_cb);


      struct shadow_render_callback_t : public glpp::pass_t::render_callback {
        shadow_render_callback_t(
           game::world_view_t::iterator itBegin,
           game::world_view_t::iterator itEnd,
           glm::mat4 const & view_matrix,
           glm::mat4 const & proj_matrix)
           : itEnd_(itEnd)
           , it_(itBegin)
           , proj_view_matrix_(proj_matrix * view_matrix) {
        }

        bool prepare_next(glpp::program & p) const override {
           if (it_ == itEnd_) return false;

           auto & current_render_info = *it_;
           auto & moment = *current_render_info.moment;

           auto model_transform = moment.mesh_transform();
           p.uniform("model").set(model_transform);
           p.uniform("mvp").set(proj_view_matrix_ * model_transform);

           it_++;
           return true;
        }

      private:
        shadow_render_callback_t(shadow_render_callback_t const &) {}
        shadow_render_callback_t & operator=(shadow_render_callback_t const &) { return *this; }

        game::world_view_t::iterator itEnd_;
        mutable game::world_view_t::iterator it_;
        glm::mat4 proj_view_matrix_;
      };

      struct mesh_render_callback_t : public glpp::pass_t::render_callback {
         mesh_render_callback_t(
            game::world_view_t::iterator itBegin,
            game::world_view_t::iterator itEnd,
            glm::mat4 const & view_matrix,
            glm::mat4 const & proj_matrix)
            : itEnd_(itEnd)
            , it_(itBegin)
            , proj_view_matrix_(proj_matrix * view_matrix) {
         }

         bool prepare_next(glpp::program & p) const override {
            if (it_ == itEnd_) return false;

            auto & current_render_info = *it_;
            auto & moment = *current_render_info.moment;

            auto model_transform = moment.mesh_transform();
            p.uniform("model").set(model_transform);
            p.uniform("mvp").set(proj_view_matrix_ * model_transform);
//            p.uniform("normal_matrix").set(glm::transpose(glm::inverse(model_transform)));

            it_++;
            return true;
         }

      private:
         mesh_render_callback_t(mesh_render_callback_t const &) {}
         mesh_render_callback_t & operator=(mesh_render_callback_t const &) { return *this; }

         game::world_view_t::iterator itEnd_;
         mutable game::world_view_t::iterator it_;
         glm::mat4 proj_view_matrix_;
      };


      //
      // debug diamond
      //

      // verts are the same as normals! (we can reuse the same buffer for each)
      const float diamond_mesh_verts[] = {
         0., 1., 0.,  // 0: top 
         0., 0., 1.,   1., 0., 0.,    0., 0., -1.,    -1., 0., 0.,   // 1-4: mid band
         0., -1., 0., // 5: bottom
      };

      const unsigned short diamond_mesh_indices[] = {
         0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 1, // top
         1, 5, 2,   2, 5, 3,   3, 5, 4,   4, 5, 1, // bottom
      };

      auto get_diamond_model_matrix = [] {
         auto moment = game::moment_t{ {400.f, 300.f}, {} };
         moment.set_dir({0., 1.});
         moment.rotate((float)glpp::get_time());
         return moment.mesh_transform();
      };

      auto diamond_model = get_diamond_model_matrix();

      auto diamond_vert_buffer = glpp::describe_buffer({ diamond_mesh_verts, diamond_mesh_indices })
         .attrib("p", 3);
      auto diamond_normal_buffer = glpp::describe_buffer({ diamond_mesh_verts })
         .attrib("normal", 3);
      auto diamond_pass = prg_3d.pass()
         .with(diamond_vert_buffer)
         .with(diamond_normal_buffer)
         .set_uniform("colour", glm::vec4(.1f, .8f, .2f, 1.f))
         .set_uniform_action("model", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); })
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_proj_cb() * get_view_cb() * get_diamond_model_matrix()); });
//         .set_uniform_action("normal_matrix", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); });


      //
      // game loop
      //

      double last_tick = glpp::get_time();

      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               ::reload_program(prg_3d, "3d");
               ::reload_program(prg_3d_particle, "3d_particle");
               ::reload_program(prg_3d_shadow, "3d_shadow");
               ::reload_program(prg_post, "post");
               ::reload_program(prg_sprite, "sprite");
            }
            catch (glpp::shader_compile_error const & ex) {
               utils::log(utils::LOG_ERROR, "%s\n", ex.what());
               utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
            }
            should_reload_program = false;
         }

         auto dims = context.win().frame_buffer_dims();

         const int shadow_texture_width = 100;
         auto shadow_texture_dims = glpp::dim_t{ shadow_texture_width, shadow_texture_width };

#ifdef WIN32 
         const glpp::texture_format_t tex_fmt = glpp::texture_format_t::BGRA;
#else
         const glpp::texture_format_t tex_fmt = glpp::texture_format_t::RGBA;
#endif
         if (!shadow_fbo || shadow_fbo->dims() != shadow_texture_dims) {
            shadow_tex.reset(new glpp::cube_map_texture_t(shadow_texture_width, tex_fmt));
            shadow_fbo.reset(new glpp::frame_buffer_t(*shadow_tex));
         }
         if (!tex_fbo || tex_fbo->dims() != dims) {
            post_tex.reset(new glpp::texture_t(dims, tex_fmt));
            tex_fbo.reset(new glpp::frame_buffer_t(*post_tex));
         }
         if (!msaa_fbo || msaa_fbo->dims() != dims) {
            msaa_fbo.reset(new glpp::frame_buffer_t(dims, 8));
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
         emitter.update(time_since_last_tick);

         // TODO: we should have one of these for each visible entity
         animation_snapshot.advance_to(glpp::get_time());


         //
         // render
         //

         // gl::clear_color(.7f, .87f, .63f, 1.);
         gl::clear_color(1.f, 1.f, 1.f, 1.);
         gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit);


         //
         // draw to shadow texture
         //

         // const PositionalLight c_light1 = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), vec3(.9, .8, .1), .1);
         struct face_info {
            glpp::frame_buffer_t::CubeFace face;
            glm::vec3 view_direction;
            glm::vec3 up_direction;
            std::string name;
         };

         const face_info faces[6] = {
            { glpp::frame_buffer_t::POSITIVE_X, { 1., 0., 0. }, { 0., -1., 0. }, "x_pos" },
            { glpp::frame_buffer_t::NEGATIVE_X, { -1., 0., 0. }, { 0., -1., 0. }, "x_neg" },
            { glpp::frame_buffer_t::POSITIVE_Y, { 0., 1., 0. }, { 0., 0., 1. }, "y_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Y, { 0., -1., 0. }, { 0., 0., -1. }, "y_neg" },
            { glpp::frame_buffer_t::POSITIVE_Z, { 0., 0., 1. }, { 0., -1., 0. }, "z_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Z, { 0., 0., -1. }, { 0., -1., 0. }, "z_neg" },
         };

         const auto light_pos = glm::vec3{ 400., 30., -300. };
         const auto light_proj = glm::perspective((float)glm::radians(90.), 1.f, 10.f, 400.f);
         const auto light_view = glm::lookAt(light_pos, light_pos + faces[0].view_direction, faces[0].up_direction);

         gl::disable(gl::enable_cap_t::blend);
         gl::cull_face(gl::cull_face_mode_t::front);
         gl::clear_color(1., 1., 1., 1.);

         // HOLY CRAP this took me ages to figure out people
         gl::viewport(0, 0, shadow_texture_width, shadow_texture_width);

         for (auto face_idx = 0; face_idx < 6; face_idx++) {
           auto & face = faces[face_idx];
           const auto view = glm::lookAt(light_pos, light_pos + face.view_direction, face.up_direction);

           shadow_fbo->bind(face.face);

           GL_VERIFY(gl::clear(
              gl::clear_buffer_flags_t::color_buffer_bit |
              gl::clear_buffer_flags_t::depth_buffer_bit));

           for (auto & pass : d3_body_shadow_passes) {
              pass.draw_batch(
                 shadow_render_callback_t{
                 world_view.creatures_begin(),
                 world_view.creatures_end(),
                 view, light_proj },
                 glpp::DrawMode::Triangles);
           }

           if (do_special_thing) {
              shadow_tex->save_current_framebuffer(std::string("screen_" + face.name + ".png"));
           }
         }
         shadow_fbo->unbind();
         gl::cull_face(gl::cull_face_mode_t::back);
         gl::enable(gl::enable_cap_t::blend);

         prg_3d.use();
         glpp::texture_unit_t tu{ 3 };
         tu.activate();
         shadow_tex->bind();
         prg_3d.uniform("shadow_texture").set(tu);

         //
         // draw to anti-aliasing frame buffer
         //

         gl::viewport(0, 0, dims.x, dims.y);
         msaa_fbo->bind();
         {
            gl::clear_color(1., 1., 1., 1.);
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);

            static bool show_light_perspective = false;
            if (do_special_thing) show_light_perspective = !show_light_perspective;

            if (!show_light_perspective)
               ground_pass.draw(glpp::DrawMode::Triangles);

            //sprite_pass.draw_batch(
            //   sprite_render_callback_t{
            //      world_view.creatures_begin(),
            //      world_view.creatures_end() },
            //   glpp::DrawMode::Triangles);

            //gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
            auto view = show_light_perspective ? light_view : get_view_cb();
            auto proj = show_light_perspective ? light_proj : get_proj_cb();
            static int prev_selected_item = -1;
            if (selected_item != prev_selected_item) {
               prev_selected_item = selected_item;
               utils::log(utils::LOG_INFO, " !!! Selecting mesh '%s'\n", (selected_item < (int)d3_body_mesh_names.size()) ? d3_body_mesh_names[selected_item].c_str() : "INVALID");
            }
            auto item = 0;
            for (auto & pass : d3_body_passes) {
               if (!special_mode_enabled || item == selected_item) {
                  pass.draw_batch(
                     mesh_render_callback_t{
                        world_view.creatures_begin(),
                        world_view.creatures_end(),
                        view, proj},
   //                     get_view_cb(), get_proj_cb()},
                     glpp::DrawMode::Triangles);
               }
               item++;
            }

            diamond_pass.draw(glpp::DrawMode::Triangles);

            gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);

            particle_pass.draw(glpp::DrawMode::Points);
         }
         // TODO: tex_fbo should be a non-MSAA renderbuffer (not texture)
         tex_fbo->bind(glpp::frame_buffer_t::Draw);
         msaa_fbo->bind(glpp::frame_buffer_t::Read);
         {
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);

            msaa_fbo->blit_to_draw_buffer();
         }
         tex_fbo->unbind();
         msaa_fbo->unbind();

         post_pass.draw(glpp::DrawMode::Triangles);

         last_tick = this_tick;

         do_special_thing = false;

         context.win().swap();
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
