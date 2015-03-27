#ifdef _MSC_VER
#  include <windows.h>
#endif

#include <map>
#include "game.h"
#include <glpp/glpp.h>
#include <glpp/utils.h>
#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

// TODO: remove this
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cstdlib>
#include <float.h> // for FLT_MAX


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

      void register_action_handler(glpp::Key k, glpp::KeyAction a, action_func f) { actions_[{k, a}] = f; }

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

         if (key_dir == dir::none) return;
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


   template <typename CreatePolicy, typename UpdatePolicy, typename DeletePolicy>
   class particle_emitter_buffer_t
      : public CreatePolicy   // CreatePolicy provides create_particles(emitter, t)
      , public UpdatePolicy   // UpdatePolicy provides update_particle(emitter, idx, t)
      , public DeletePolicy { // DeletePolicy provides should_delete(emitter, idx)
   public:
      using cont_t = std::vector < glm::vec3 >;
      using idx_t = std::size_t;

      glm::vec3 vel_{0., -200., 0.};

      void create_particle() {
         auto pos = glm::vec3{
            (float)(std::rand() % 460) - 230.,
            100.,
            (float)(std::rand() % 350) - 250. };

         add(pos, vel_);
      }

      void update(double time_since_last) {
         current_time_ += time_since_last;
         CreatePolicy::create_particles(*this, time_since_last);
         // glm likes floats
         auto t = static_cast<float>(time_since_last);

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
      idx_t add(glm::vec3 pos, glm::vec3 vel) { particle_create_times_.push_back(current_time_);  particle_positions_.push_back(pos); particle_velocities_.push_back(vel); return count() - 1; }
      void del_at(idx_t idx) { del_at_(particle_create_times_, idx);  del_at_(particle_positions_, idx); del_at_(particle_velocities_, idx); }
      double create_time_at(idx_t idx) { return particle_create_times_[idx]; }
      glm::vec3 & pos_at(idx_t idx) { return particle_positions_[idx]; }
      glm::vec3 & vel_at(idx_t idx) { return particle_velocities_[idx]; }

   private:
      glpp::buffer_t buffer_ = {glpp::buffer_t::Usage::Stream};
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
      void create_particles(T & emitter, double time_since_last) {
         auto now = emitter.time();
         while (next_create_time_ < now) {
            emitter.create_particle();
            next_create_time_ += TIME_BETWEEN_CREATE;
         }
      }
   };

   class constant_update_policy_t {
   protected:
      using idx_t = std::size_t;

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
      constant_create_policy_t<8000>,
      constant_update_policy_t,
      constant_depth_delete_policy_t<0> >;
}

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

void print_node_info(aiNode const & node, int depth = 0) {
   auto indent = std::string(depth * 2, '-');
   utils::log(utils::LOG_INFO, " %s- %s %d children, %d meshes\n", indent.c_str(), node.mName.C_Str(), node.mNumChildren, node.mNumMeshes);

   for (auto idx = 0U; idx < node.mNumChildren; idx++) {
      print_node_info(*node.mChildren[idx], depth + 1);
   }
}

glm::mat4 to_mat4(aiMatrix4x4 const & from) {
   glm::mat4 to;
   to[0][0] = from.a1; to[1][0] = from.a2;
   to[2][0] = from.a3; to[3][0] = from.a4;
   to[0][1] = from.b1; to[1][1] = from.b2;
   to[2][1] = from.b3; to[3][1] = from.b4;
   to[0][2] = from.c1; to[1][2] = from.c2;
   to[2][2] = from.c3; to[3][2] = from.c4;
   to[0][3] = from.d1; to[1][3] = from.d2;
   to[2][3] = from.d3; to[3][3] = from.d4;

   return to;
}

void for_each_mesh(aiScene const & scene, aiNode const & node, glm::mat4 const & parent_transform, std::function<void(aiMesh const &, glm::mat4 const &)> callback) {
   auto transform = parent_transform * to_mat4(node.mTransformation);
   for (auto idx = 0U; idx < node.mNumMeshes; idx++) {
      callback(*scene.mMeshes[node.mMeshes[idx]], transform);
   }

   for (auto idx = 0U; idx < node.mNumChildren; idx++) {
      for_each_mesh(scene, *node.mChildren[idx], transform, callback);
   }
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

   Assimp::Importer importer;
   auto * scene = importer.ReadFile("dude-anim.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
   if (!scene) {
      utils::log(utils::LOG_ERROR, "could not import scene: %s\n", importer.GetErrorString());
      return 0;
   }

   utils::log(utils::LOG_INFO, "scene imported: %d meshes, %d materials, %d textures, %d animations\n", 
      scene->mNumMeshes,
      scene->mNumMaterials,
      scene->mNumTextures,
      scene->mNumAnimations
   );

   auto * node = scene->mRootNode;
   if (node) print_node_info(*node);

#if 1
   auto mat_name = [scene](unsigned idx) {
      aiString name;
      auto & m = *scene->mMaterials[idx];
      if (AI_SUCCESS != m.Get(AI_MATKEY_NAME, name)) return std::string("NONAME");
      return std::string(name.C_Str());
   };

   for (auto idx = 0U; idx < scene->mNumMaterials; idx++) {
      auto & m = *scene->mMaterials[idx];

      std::string props;
      for (auto pidx = 0U; pidx < m.mNumProperties; pidx++) {
         auto & p = *m.mProperties[pidx];
         if (props.empty()) props += " "; else props += ", "; props += p.mKey.C_Str();
      }

      utils::log(utils::LOG_INFO, " - mat%d (%s): %d props (%s)\n", idx, mat_name(idx).c_str(), m.mNumProperties, props.c_str());
   }

   for (auto idx = 0U; idx < scene->mNumMeshes; idx++) {
      auto & m = *scene->mMeshes[idx];
      utils::log(utils::LOG_INFO, " - mesh%d: %d verts (%d%s faces), %d bones, mat %s, %d anim meshes\n",
         idx, m.mNumVertices, m.mNumFaces, m.mPrimitiveTypes == aiPrimitiveType_TRIANGLE ? " tri" : "",
         m.mNumBones, mat_name(m.mMaterialIndex).c_str(), m.mNumAnimMeshes);

      for (auto bidx = 0U; bidx < m.mNumBones; bidx++) {
         auto & b = *m.mBones[bidx];
         utils::log(utils::LOG_INFO, "   - bone%d (%s): %d weights\n", bidx, b.mName.C_Str(), b.mNumWeights);
      }
   }

   for (auto idx = 0U; idx < scene->mNumAnimations; idx++) {
      auto & a = *scene->mAnimations[idx];
      utils::log(utils::LOG_INFO, " - anim%d: '%s' %d channels, %d mesh channels\n", idx, a.mName.C_Str(), a.mNumChannels, a.mNumMeshChannels);
      for (auto cidx = 0U; cidx < a.mNumChannels; cidx++) {
         auto & c = *a.mChannels[cidx];
         utils::log(utils::LOG_INFO, "   - chan%d (%s): %d pos, %d rot, %d scale keys\n", cidx, c.mNodeName.C_Str(), c.mNumPositionKeys, c.mNumRotationKeys, c.mNumScalingKeys);
      }
      for (auto cidx = 0U; cidx < a.mNumMeshChannels; cidx++) {
         auto & c = *a.mMeshChannels[cidx];
         utils::log(utils::LOG_INFO, "   - mesh chan%d (%s): %d keys\n", cidx, c.mName.C_Str(), c.mNumKeys);
      }
   }
#endif

   struct bounds_t { glm::vec3 lower; glm::vec3 upper; };
   auto find_mesh_bounds = [](aiMesh const & m) -> bounds_t {
      if (m.mNumVertices == 0) return{};

      aiVector3D l = m.mVertices[0];
      aiVector3D u = m.mVertices[0];
      for (auto i = 0U; i < m.mNumVertices; i++) {
         auto & v = m.mVertices[i];
         if (v.x < l.x) l.x = v.x;
         if (v.y < l.y) l.y = v.y;
         if (v.z < l.z) l.z = v.z;
         if (v.x > u.x) u.x = v.x;
         if (v.y > u.y) u.y = v.y;
         if (v.z > u.z) u.z = v.z;
      }
      return{ {l.x, l.y, l.z}, {u.x, u.y, u.z} };
   };

   try {
      glpp::init();
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      exit(EXIT_FAILURE);
   }


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

      gl::enable(gl::enable_cap_t::depth_test);
      gl::enable(gl::enable_cap_t::cull_face);
      //gl::enable(gl::enable_cap_t::multisample);
      gl::enable(gl::enable_cap_t::blend);

#ifndef WIN32
      gl::point_size(3.5);
      gl::enable(gl::enable_cap_t::point_smooth);
#endif

      gl::blend_func(gl::blending_factor_src_t::src_alpha, gl::blending_factor_dest_t::one_minus_src_alpha);

      gl::cull_face(gl::cull_face_mode_t::back);
      
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

      // 
      // load shaders
      //

      auto prg_2d = create_program("2d");
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

      auto bg_pass = prg_2d.pass()
         .with(screen_vertices_spec)
         .set_uniform("texture", glpp::texture_t{ "../../res/bg_green.png" });

      auto shadow_tex = std::unique_ptr<glpp::cube_map_texture_t>();
      auto post_tex = std::unique_ptr<glpp::texture_t>();
      auto shadow_fbo = std::unique_ptr<glpp::frame_buffer_t>();
      auto tex_fbo = std::unique_ptr<glpp::frame_buffer_t>();
      auto msaa_fbo = std::unique_ptr<glpp::frame_buffer_t>();

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





      auto make_mesh_vert_buffer = [](aiMesh const & m, glm::mat4 const & xform) -> glpp::buffer_t {
         auto get_mesh_indices = [](aiMesh const & m) {
            std::vector<uint32_t> indices;
            indices.reserve(m.mNumFaces * 3);
            for (auto fidx = 0U; fidx < m.mNumFaces; fidx++) {
               auto & f = m.mFaces[fidx];
               for (auto i = 0U; i < f.mNumIndices; i++) {
                  indices.push_back(f.mIndices[i]);
               }
            }
            return indices;
         };

         auto xform_vert = [&xform](float * in, float * out) {
            auto vert = xform * glm::vec4{ in[0], in[1], in[2], 1. };
            out[0] = vert.x;
            out[1] = vert.y;
            out[2] = vert.z;
         };

         auto * raw_vert_data = reinterpret_cast<float*>(m.mVertices);
         auto elem_count = m.mNumVertices * 3;

         auto indices = get_mesh_indices(m);

         //
         // apply local transform to model
         //

         std::vector<float> vert_data;
         vert_data.reserve(elem_count);
         auto * xformed_vert_data = vert_data.data();

         for (auto i = 0U; i < m.mNumVertices; i++) {
            auto data_in = raw_vert_data + (i * 3);
            auto data_out = xformed_vert_data + (i * 3);
            xform_vert(data_in, data_out);
         }

#if 0
         utils::log(utils::LOG_INFO, "\n == dumping %d verts == \n", m.mNumVertices);
         for (auto idx = 0U; idx < m.mNumVertices; idx++) {
            utils::log(utils::LOG_INFO, " %d - %.1f, %.1f, %.1f\n", idx, m.mVertices[idx].x, m.mVertices[idx].y, m.mVertices[idx].z);
         }

         auto idx = 0;
         for (auto i : indices) utils::log(utils::LOG_INFO, " %d%s", i, idx++ % 3 == 2 ? "," : "");
         utils::log(utils::LOG_INFO, "\n");
#endif

         return{
            { xformed_vert_data, elem_count },
            { indices.data(), indices.size() }
         };
      };

      auto make_mesh_normal_buffer = [](aiMesh const & m, glm::mat4 const & xform) -> glpp::buffer_t {
         assert(m.HasNormals());
         auto * raw_data = reinterpret_cast<float*>(m.mNormals);
         auto elem_count = m.mNumVertices * 3;

         //
         // apply local transform to model
         //

         std::vector<float> vert_data;
         vert_data.reserve(elem_count);
         auto * xformed_data = vert_data.data();

         auto xform_vert = [&xform](float * in, float * out) {
            auto vert = xform * glm::vec4{ in[0], in[1], in[2], 0. };
            out[0] = vert.x;
            out[1] = vert.y;
            out[2] = vert.z;
         };

         for (auto i = 0U; i < m.mNumVertices; i++) {
            auto data_in = raw_data + (i * 3);
            auto data_out = xformed_data + (i * 3);
            xform_vert(data_in, data_out);
         }

         return{
            { xformed_data, elem_count }
         };
      };


      //
      // state callbacks
      //

      auto get_view_cb = [] {
         auto xpos = 0;
         auto ypos = 100.f;
         auto zpos = 100.f;
         return glm::lookAt(glm::vec3{ xpos, ypos, zpos }, glm::vec3{ 0., 0., 0. }, glm::vec3{ 0., 1., 0. });
      };

      auto get_model_cb = [] {
         return glm::scale(glm::mat4{}, glm::vec3{ 32. });
      };

      auto get_proj_cb = [] {
         return glm::ortho<float>(0., 800., 0., 600., 0., 1000.);
         // return glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1000.f);
      };

      auto set_view_cb = [&get_view_cb](glpp::uniform & u) {
         u.set(get_view_cb());
      };

      auto make_shadow_pass = [&](glpp::program & program, aiScene const & scene, aiMesh const & mesh, glm::mat4 const & pos) {
         auto verts = glpp::describe_buffer(make_mesh_vert_buffer(mesh, pos))
            .attrib("p", 3);

         return program.pass()
            .with(verts);
      };

      auto make_mesh_pass = [&](glpp::program & program, aiScene const & scene, aiMesh const & mesh, glm::mat4 const & pos) {
         auto verts = glpp::describe_buffer(make_mesh_vert_buffer(mesh, pos))
            .attrib("p", 3);

         auto normals = glpp::describe_buffer(make_mesh_normal_buffer(mesh, pos))
            .attrib("normal", 3);

         aiColor4D color(1.f, 0.f, 1.f, 1.f);
         aiGetMaterialColor(scene.mMaterials[mesh.mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color);

         auto mesh_colour = glm::vec4(color.r, color.g, color.b, color.a);

         return program.pass()
            .with(verts)
            .with(normals)
            .set_uniform("colour", mesh_colour)
            .set_uniform_action("t", set_time_cb);
      };

      std::vector<glpp::pass_t> d3_shadow_passes;
      std::vector<glpp::pass_t> d3_body_passes;

      aiNode * node = scene->mRootNode;
      if (node) for_each_mesh(*scene, *node, glm::mat4{}, [&](aiMesh const & mesh, glm::mat4 const & pos) {
         d3_shadow_passes.push_back(make_shadow_pass(prg_3d_shadow, *scene, mesh, pos));
         d3_body_passes.push_back(make_mesh_pass(prg_3d, *scene, mesh, pos));
      });


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

      auto ground_buffer_desc = glpp::describe_buffer({ground_verts, ground_indices})
         .attrib("p", 3)
         .attrib("normal", 3);


      auto ground_pass = prg_3d.pass()
         .with(ground_buffer_desc)
         .set_uniform("colour", glm::vec4(.8f, .8f, .16f, 1.f))
         .set_uniform("model", glm::mat4{})
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_proj_cb() * get_view_cb()); })
         .set_uniform("normal_matrix", glm::mat4{})
         .set_uniform_action("t", set_time_cb);


      constant_particle_emitter_buffer_t emitter;
      auto set_screensize_cb = [&](glpp::uniform & u) {
         auto dims = context.win().frame_buffer_dims();
         u.set(glm::vec2{dims.x, dims.y});
      };

      auto emitter_buffer_desc = glpp::describe_buffer(emitter.buffer())
         .attrib("position", 3);
      auto particle_pass = prg_3d_particle.pass()
         .with(emitter_buffer_desc)
         .set_uniform("texture", glpp::texture_t{ "../../res/drop.png" })
         .set_uniform_action("screenSize", set_screensize_cb)
         .set_uniform("proj", glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1000.f))
         //.set_uniform("proj", glm::ortho<float>(0., 800., 0., 600., 0., 1000.))
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
            p.uniform("normal_matrix").set(glm::transpose(glm::inverse(model_transform)));

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
      // game loop
      //

      double last_tick = glpp::get_time();

      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               ::reload_program(prg_2d, "2d");
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
         auto square_dims = glpp::dim_t{ dims.x, dims.x };

#ifdef WIN32 
         const glpp::texture_format_t tex_fmt = glpp::texture_format_t::BGRA;
#else
         const glpp::texture_format_t tex_fmt = glpp::texture_format_t::RGBA;
#endif
         if (!shadow_fbo || shadow_fbo->dims() != square_dims) {
            shadow_tex.reset(new glpp::cube_map_texture_t(square_dims.x, tex_fmt));
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

         const double MAX_TICK = 1000. / 15.;
         if (time_since_last_tick > MAX_TICK) time_since_last_tick = MAX_TICK;

         //
         // update world
         //

         world.update(time_since_last_tick);
         world_view.update(time_since_last_tick);
         emitter.update(time_since_last_tick);


         //
         // render
         //

         gl::clear_color(.7f, .87f, .63f, 1.);
         gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit);
         gl::viewport(0, 0, dims.x, dims.y);


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
            { glpp::frame_buffer_t::POSITIVE_Y, { 0., 1., 0. }, { 0., 0., -1. }, "y_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Y, { 0., -1., 0. }, { 0., 0., 1. }, "y_neg" },
            { glpp::frame_buffer_t::POSITIVE_Z, { 0., 0., 1. }, { 0., -1., 0. }, "z_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Z, { 0., 0., -1. }, { 0., -1., 0. }, "z_neg" },
         };

         const auto light_pos = glm::vec3{ 400., 30., -300. };
         const auto light_proj = glm::perspective((float)glm::radians(90.), 1.f, .1f, 1000.f);
         const auto light_view = glm::lookAt(light_pos, light_pos + faces[0].view_direction, faces[0].up_direction);

         gl::cull_face(gl::cull_face_mode_t::front);
         //gl::clear_color(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
         gl::clear_color(1., 1., 1., 1.);

         for (auto face_idx = 0; face_idx < 6; face_idx++) {
            auto & face = faces[face_idx];
            const auto view = glm::lookAt(light_pos, light_pos + face.view_direction, face.up_direction);
            shadow_fbo->bind(face.face);
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);

            for (auto & pass : d3_shadow_passes) {
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
         do_special_thing = false;

         prg_3d.use();
         glpp::texture_unit_t tu{ 3 };
         tu.activate();
         shadow_tex->bind();
         prg_3d.uniform("shadow_texture").set(tu);
         prg_3d.uniform("light_vp").set(light_proj * light_view);

         //
         // draw to anti-aliasing frame buffer
         //

         msaa_fbo->bind();
         {
            gl::clear_color(0., 0., 0., 1.);
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);
            // bg_pass.draw(glpp::DrawMode::Triangles);

            gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);

            gl::cull_face(gl::cull_face_mode_t::back);
            ground_pass.draw(glpp::DrawMode::Triangles);

            //sprite_pass.draw_batch(
            //   sprite_render_callback_t{
            //      world_view.creatures_begin(),
            //      world_view.creatures_end() },
            //   glpp::DrawMode::Triangles);

            gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
            for (auto & pass : d3_body_passes) {
               pass.draw_batch(
                  mesh_render_callback_t{
                     world_view.creatures_begin(),
                     world_view.creatures_end(),
                     get_view_cb(), get_proj_cb()},
                  glpp::DrawMode::Triangles);
            }

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
