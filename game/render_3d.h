#ifndef GAME_VIEW_3D__H
#define GAME_VIEW_3D__H

#include <glpp/glpp.h>
#include <glpp/assets.h>

#include "game.h"
#include "particles_3d.h"

#include <memory>
#include <map>


namespace game {
   struct model_repository {
      model_repository(model_repository const &) = delete;
      model_repository & operator=(model_repository const &) = delete;
      model_repository(model_repository &&) = delete;
      model_repository & operator=(model_repository &&) = delete;

      explicit model_repository(glpp::archive_t const & assets);

      glpp::scene_t const & find_scene_by_name(std::string const & name) const;

      std::map<std::string, glpp::scene_t> scenes;
   };


   namespace impl {
      struct ensure_gl_init {
      protected:
         ensure_gl_init();
      };
   }

   struct render_context : impl::ensure_gl_init {
      render_context(glpp::archive_t const & assets, glpp::context::key_callback_t key_callback);

      bool closing() const { return context.win().closing(); }
      void swap() { context.win().swap(); }

      void reload_shaders();
      void reload_framebuffers();
      void toggle_fullscreen();

      void init_context();

      glpp::context context;

      glpp::archive_t const & assets;

      glpp::program prg_3d;
      glpp::program prg_3d_shadow;
      glpp::program prg_3d_particle;
      glpp::program prg_post;

      std::unique_ptr<glpp::cube_map_texture_t> shadow_tex;
      std::unique_ptr<glpp::texture_t> post_tex;
      std::unique_ptr<glpp::frame_buffer_t> shadow_fbo;
      std::unique_ptr<glpp::frame_buffer_t> tex_fbo;
      std::unique_ptr<glpp::frame_buffer_t> msaa_fbo;
   };

   struct renderer {
      renderer(model_repository const & models, render_context & ctx);

      void update_and_render(double time_since_last_tick, game::world_view_t const & world_view);

      float get_view_height() const { return view_height; }
      void set_view_height(float view_height_in) { view_height = std::max(0.f, std::min(1.f, view_height_in)); }

      void debug_do_special_thing() { debug_do_special_thing_ = true; }
      void debug_show_mesh(int id) { debug_selected_item_ = id; }
      void debug_enable_special_mode(bool enabled) { debug_special_mode_enabled_ = enabled; }
      void debug_enable_shadows(bool enabled) { debug_shadows_disabled_ = !enabled; }

      render_context & context;

      struct mesh_render_info {
         glpp::animation_t const & animation;
         std::vector<glpp::pass_t> d3_mesh_passes;
         std::vector<glpp::pass_t> d3_shadow_mesh_passes;
      };

      std::vector<mesh_render_info> mesh_renderers;

      std::vector<glpp::pass_t> debug_diamond_pass;
      std::vector<glpp::pass_t> ground_pass;
      std::vector<glpp::pass_t> particle_pass;
      std::vector<glpp::pass_t> post_pass;

      // debugging
      std::vector<std::string> mesh_names;

      constant_particle_emitter_buffer_t emitter;

      float view_height = 1.;
      bool debug_do_special_thing_ = false;
      bool debug_special_mode_enabled_ = false;
      bool debug_shadows_disabled_ = false;
      int debug_selected_item_ = 0;
   };
} // namespace game

#endif // #ifndef GAME_VIEW_3D__H
