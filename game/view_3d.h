#ifndef GAME_VIEW_3D__H
#define GAME_VIEW_3D__H

#include <glpp/glpp.h>
#include <glpp/assets.h>

#include "game.h"
#include "particles_3d.h"

#include <memory>


struct ensure_gl_init {
protected:
   ensure_gl_init();
};

struct view_3d : ensure_gl_init {
   view_3d(glpp::context::key_callback_t key_callback);

   void reload_shaders();
   void toggle_fullscreen();

   void init_context();

   float view_height = 1.;
   float get_view_height() const { return view_height; }
   void set_view_height(float view_height_in) { view_height = std::max(0.f, std::min(1.f, view_height_in)); }

   bool debug_do_special_thing_ = false;
   bool debug_special_mode_enabled_ = false;
   int debug_selected_item_ = 0;
   void debug_do_special_thing() { debug_do_special_thing_ = true; }
   void debug_show_mesh(int id) { debug_selected_item_ = id; }
   void debug_enable_special_mode(bool enabled) { debug_special_mode_enabled_ = enabled; }

   void update_and_render(double time_since_last_tick, game::world_view_t const & world_view);

   glpp::context context;

   glpp::archive_t assets;

   constant_particle_emitter_buffer_t emitter;

   glpp::scene_t model_dude;
   glpp::scene_t model_campfire;

   // TODO: we should have one of these for each visible entity
   glpp::animation_timeline_t dude_walk_animation;
   
   glpp::program prg_3d;
   glpp::program prg_3d_shadow;
   glpp::program prg_3d_particle;
   glpp::program prg_post;

   std::unique_ptr<glpp::cube_map_texture_t> shadow_tex;
   std::unique_ptr<glpp::texture_t> post_tex;
   std::unique_ptr<glpp::frame_buffer_t> shadow_fbo;
   std::unique_ptr<glpp::frame_buffer_t> tex_fbo;
   std::unique_ptr<glpp::frame_buffer_t> msaa_fbo;

   // actual render passes that are executed
   std::vector<glpp::pass_t> d3_body_passes;
   std::vector<glpp::pass_t> d3_body_shadow_passes;
   std::vector<glpp::pass_t> d3_campfire_passes;
   std::vector<glpp::pass_t> d3_campfire_shadow_passes;

   std::vector<glpp::pass_t> debug_diamond_pass;
   std::vector<glpp::pass_t> ground_pass;
   std::vector<glpp::pass_t> particle_pass;
   std::vector<glpp::pass_t> post_pass;

   // debugging
   std::vector<std::string> mesh_names;
};
#endif // #ifndef GAME_VIEW_3D__H
