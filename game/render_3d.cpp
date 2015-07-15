#include "render_3d.h"

#include <glpp/utils.h>

#ifdef _MSC_VER
#  include <glpp/gles2.h>
   namespace gl {
      using namespace gles2;
      using angle::vertex_attrib_divisor;
      using angle::draw_arrays_instanced;
   }
#else
#   include <glpp/gl2.h>
   namespace gl {
      using namespace gl2;
      using osx::vertex_attrib_divisor;
      using osx::draw_arrays_instanced;
   }
#endif

#include <algorithm>

namespace game { namespace impl {
   //
   // ensure_gl_init
   //

   ensure_gl_init::ensure_gl_init() {
      try {
         glpp::init();
      }
      catch (std::exception const & ex) {
         utils::log(utils::LOG_ERROR, "%s\n", ex.what());
         exit(EXIT_FAILURE);
      }
   }
} }

//
// context_3d
//

namespace {
   const int pos_shadow_texture_width = 100;
   const int dir_shadow_texture_width = pos_shadow_texture_width * 10;

   glpp::texture_unit_t DEFAULT_TEXTURE_UNIT{ 0 };
   glpp::texture_unit_t UI_TEXTURE_UNIT{ 8 };
   glpp::texture_unit_t POST_TEXTURE_UNIT{ 2 };
   glpp::texture_unit_t SHADOW_TEXTURE_UNIT{ 3 };
   glpp::texture_unit_t GROUND_TEXTURE_UNIT{ 4 };
   glpp::texture_unit_t RAIN_TEXTURE_UNIT{ 5 };

   glpp::shader vert_shader(glpp::archive_t const & assets, std::string name) { return assets.load_shader(utils::fmt("shaders/%s.vert", name.c_str())); }
   glpp::shader frag_shader(glpp::archive_t const & assets, std::string name) { return assets.load_shader(utils::fmt("shaders/%s.frag", name.c_str())); }

   glpp::program create_program(glpp::archive_t const & assets, std::string name) {
      utils::log(utils::LOG_INFO, "compiling '%s' shader programs... ", name.c_str());

      auto program = glpp::program{ vert_shader(assets, name), frag_shader(assets, name) };

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }

      return program;
   };

   void reload_program(glpp::archive_t const & assets, glpp::program & program, std::string name) {
      utils::log(utils::LOG_INFO, "reloading '%s' shader programs... ", name.c_str());

      program.reload(vert_shader(assets, name), frag_shader(assets, name));

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }
   };

   //
   // unit square data
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

   //
   // screen coords
   //

   glpp::mapped_buffer_t screen_vertices_buffer(glpp::program & prg) {
      auto buffer = glpp::buffer_t{ unit_square_verts, unit_square_indices };
      return glpp::buffer_attrib_mappings_t()
         .push_attrib("p", 2)
         .push_attrib("tex_coords", 2)
         .map_buffer(prg, buffer);
   }

   //
   // debug diamond data
   //

   auto get_diamond_model_matrix = [] {
      auto moment = game::moment_t{ { 400.f, 300.f },{} };
      moment.set_dir({ 0., 1. });
      moment.rotate((float)glpp::get_time());
      return moment.mesh_transform();
   };

   // verts are the same as normals! (we can reuse the same buffer for each)
   const float diamond_mesh_verts[] = {
      // 0: top
      0., 1., 0.,   .5, 1.,
      // 1-4: mid band
      0., 0., 1.,   .5, .5,
      1., 0., 0.,   1., .5,
      0., 0., -1.,  .5, .5,
      -1., 0., 0.,  0., .5,
      // 5: bottom
      0., -1., 0.,  .5, 0.,
   };

   const unsigned short diamond_mesh_indices[] = {
      0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 1, // top
      1, 5, 2,   2, 5, 3,   3, 5, 4,   4, 5, 1, // bottom
   };

   glpp::mapped_buffer_t diamond_vert_buffer(glpp::program & prg) {
      return glpp::buffer_attrib_mappings_t()
         .push_attrib("p", 3)
         .push_attrib("tex_coords", 2)
         .map_buffer(prg, { diamond_mesh_verts, diamond_mesh_indices });
   }

   glpp::mapped_buffer_t diamond_normal_buffer(glpp::program & prg) {
      return glpp::buffer_attrib_mappings_t()
         .push_attrib("normal", 3)
         .map_buffer(prg, { diamond_mesh_verts });
   }

   //
   // ground data
   //
   const float G_VERT_SCALE = 2.5f;
   const float G_TEX_SCALE = 2.5f * 3.f;
   static const float ground_verts[] = {
      G_VERT_SCALE * -800.f, 0., G_VERT_SCALE * -900.f,     0., 1.f, 0.,    0.,                  G_TEX_SCALE,
      G_VERT_SCALE *  800.f, 0., G_VERT_SCALE * -900.f,     0., 1.f, 0.,    G_TEX_SCALE * 1.33f, G_TEX_SCALE,
      G_VERT_SCALE * -800.f, 0., G_VERT_SCALE * 1100.f,     0., 1.f, 0.,    0.,                  0.,
      G_VERT_SCALE *  800.f, 0., G_VERT_SCALE * 1100.f,     0., 1.f, 0.,    G_TEX_SCALE * 1.33f, 0.,
   };
   static const unsigned short ground_indices[] = {
      0, 2, 1,
      1, 2, 3,
   };

   //
   // default bone data
   //

   static const float default_bone_indices[] = {
      0., 0., 0., 0.,
      0., 0., 0., 0.,
      0., 0., 0., 0.,
      0., 0., 0., 0.,
   };
   static const float default_bone_weights[] = {
      1., 0., 0., 0.,
      1., 0., 0., 0.,
      1., 0., 0., 0.,
      1., 0., 0., 0.,
   };

   static const std::vector<glm::mat4> default_bone_transforms = { glm::mat4{} };


   //
   // render callbacks
   //
   namespace {
      glm::vec3 calc_sky_light_direction(float dir) {
         // light goes from 0. == below, .5 == above, 1. == below
         // t: 0.   .25    .5   .75   1.
         // x: 0 -> -1 ->  0 -> 1 -> 0   : -sin(t * 2PI)
         // y: 1 ->  0 -> -1 -> 0 -> 1   :  cos(t * 2PI) 
         return{-std::sin(dir * glpp::TAU), std::cos(dir * glpp::TAU), 0.f};
      }
   }

   struct mesh_render_batch_callback_t : public glpp::pass_t::render_batch_callback {
      using entity_filter = std::function<bool(game::world_view_t::render_info_t const &)>;

      // TODO: this should be in a scene class or namespace
      struct light_t {
         enum class type_t {none, ambient, positional, directional};
         type_t type = type_t::none;
         glm::vec3 colour = {};
         float intensity = 0.f;
         glm::vec3 moment = {}; // either pos or direction, depending on type

         light_t(glm::vec3 const & colour, float intensity) : type(type_t::ambient), colour(colour), intensity(intensity) {}
         light_t(glm::vec3 const & colour, float intensity, float dir) : type(type_t::directional), colour(colour), intensity(intensity), moment(calc_sky_light_direction(dir)) {}
         light_t(glm::vec3 const & colour, float intensity, glm::vec3 pos) : type(type_t::positional), colour(colour), intensity(intensity), moment(pos) {}
         light_t() {}
         light_t(light_t const&) = default;
         light_t & operator=(light_t const&) = default;
      };

      mesh_render_batch_callback_t(
         unsigned pass_mesh_idx,
         game::world_view_t::const_iterator itBegin,
         game::world_view_t::const_iterator itEnd,
         glm::mat4 const & view_matrix,
         glm::mat4 const & proj_matrix,
         entity_filter filter,
         light_t ambient,
         light_t sky)
         : pass_mesh_idx_(pass_mesh_idx)
         , itEnd_(itEnd)
         , it_(itBegin)
         , filter_(filter)
         , proj_view_matrix_(proj_matrix * view_matrix)
         , ambient_light_(ambient)
         , sky_light_(sky){
      }

      mutable bool first_ = true;
      bool prepare_next(glpp::program & p) const override {
         if (first_) {
            first_ = false;
            if (ambient_light_.type != light_t::type_t::none) {
               assert(ambient_light_.type == light_t::type_t::ambient);
               p.uniform("ambient_colour").set(ambient_light_.colour);
               p.uniform("ambient_intensity").set(ambient_light_.intensity);
            }

            if (sky_light_.type != light_t::type_t::none) {
               assert(sky_light_.type == light_t::type_t::directional);
               p.uniform("sky_light_dir").set(sky_light_.moment);
               p.uniform("sky_light_colour").set(sky_light_.colour);
               p.uniform("sky_light_intensity").set(sky_light_.intensity);
            }
         }
         while (true) {
            if (it_ == itEnd_) return false;
            if (filter_(*it_)) break; // found valid entity
            ++it_;
         }

         auto & current_render_info = *it_;
         auto & moment = *current_render_info.moment;
         auto & animation = *current_render_info.sprite;

         auto model_transform = moment.mesh_transform();
         p.uniform("model").set(model_transform);
         p.uniform("mvp").set(proj_view_matrix_ * model_transform);
         //            p.uniform("normal_matrix").set(glm::transpose(glm::inverse(model_transform)));

         auto & mesh = animation.meshes()[pass_mesh_idx_];
         p.uniform("bones[0]").set(mesh.bone_transforms());

         ++it_;
         return true;
      }

   private:
      mesh_render_batch_callback_t(mesh_render_batch_callback_t const &) {}
      mesh_render_batch_callback_t & operator=(mesh_render_batch_callback_t const &) { return *this; }

      unsigned pass_mesh_idx_;
      game::world_view_t::const_iterator itEnd_;
      mutable game::world_view_t::const_iterator it_;
      entity_filter filter_;
      glm::mat4 proj_view_matrix_;
      light_t ambient_light_;
      light_t sky_light_;
   };



   //
   // helper functions
   //

   std::vector<glpp::pass_t> make_mesh_passes(game::render_context const & ctx, glpp::program & program, glpp::animation_t const & animation, bool shadow) {
      std::vector<glpp::pass_t> result;

      auto sprite = animation.create_timeline();
      for (auto & mesh : sprite.meshes()) {

         auto verts = glpp::buffer_attrib_mappings_t()
            .push_attrib("p", 3)
            .map_buffer(program, { { mesh.vertices().buffer, mesh.vertices().count },{ mesh.indices().buffer, mesh.indices().count } });
         auto normals = glpp::buffer_attrib_mappings_t()
            .push_attrib("normal", 3)
            .map_buffer(program, { { mesh.normals().buffer, mesh.normals().count } });
         auto bone_indices = glpp::buffer_attrib_mappings_t()
            .push_attrib("bone_indices", 4)
            .map_buffer(program, { { mesh.bone_indices().buffer, mesh.bone_indices().count } });
         auto bone_weights = glpp::buffer_attrib_mappings_t()
            .push_attrib("bone_weights", 4)
            .map_buffer(program, { { mesh.bone_weights().buffer, mesh.bone_weights().count } });

         if (!shadow) {
            result.push_back(
               program.pass()
               .with(verts)
               .with(normals)
               .with(bone_indices)
               .with(bone_weights)
               .set_uniform("colour", mesh.material().diffuse_colour)
               .set_uniform("use_texture", 0.f)
               );
         }
         else {
            result.push_back(
               program.pass()
               .with(verts)
               .with(bone_indices)
               .with(bone_weights)
               );
         }
      }

      return result;
   };

   game::renderer::mesh_render_info make_render_info(game::render_context const & ctx, glpp::animation_t const & animation, glpp::program & prg_3d, glpp::program & prg_3d_shadow) {
      return{
         animation,
         make_mesh_passes(ctx, prg_3d, animation, false),
         make_mesh_passes(ctx, prg_3d_shadow, animation, true)
      };
   };


   //
   // state callbacks
   //

   using callback_get_float = std::function<float()>;
   using callback_get_vec3 = std::function<glm::vec3()>;
   using callback_get_mat4 = std::function<glm::mat4()>;
   using callback_set_uniform = std::function<void(glpp::uniform &)>;

   static float g_camera_distance = 800.f;
   static float g_camera_height = 1000.f;
   static float g_camera_fov = 45.f;

   glm::vec3 get_camera_pos(game::renderer const & view) {
      auto center_2d = game::center_world_location();
      auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
      auto eye = center + glm::vec3{ 0.f, g_camera_height * view.get_view_height(), g_camera_distance };
      return eye;
   }

   static float g_sky_light_position = 1.;

   glm::mat4 get_view(game::renderer const & view) {
#if 0
      const auto light_dir = calc_sky_light_direction(g_sky_light_position);
      auto position = -light_dir * 1000.f;

      auto v = glm::lookAt(position, glm::vec3{ 0., 0., 0. }, glm::vec3{ 0., 1., 0. });
      return v;
#endif

      auto center_2d = game::center_world_location();
      auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
      auto eye = get_camera_pos(view);
      auto result = glm::lookAt(eye, center, glm::vec3{ 0., 1., 0. });
      return result;
   }

   glm::mat4 get_proj(bool ortho) {
#if 0
      const auto light_proj = glm::ortho(-1000.f, 1000.f, -1000.f, 1000.f, 500.f, 5000.f);
      return light_proj;
#endif

      // 0, 0 is the bottom left of the lookAt target!
      auto max_dist = g_camera_distance > g_camera_height ? g_camera_distance : g_camera_height;
      auto near_plane = .05f * max_dist;
      auto far_dist = 5 * max_dist;
      if (ortho)
         return glm::ortho<float>(-800., 800., -600., 600., near_plane, far_dist);
      return glm::perspective<float>(glpp::PI_F * g_camera_fov / 180.f, 800.f / 600.f, near_plane, far_dist);
   }

   glm::mat4 get_mvp(game::renderer const & view, bool ortho) {
      return get_proj(ortho) * get_view(view);
   }

   namespace callbacks {
      callback_set_uniform set_time() { return [](glpp::uniform & u) {u.set(static_cast<float>(glpp::get_time())); }; }
      callback_set_uniform set_mvp(game::renderer const & view, bool const & ortho) { return [&](glpp::uniform & u) { u.set(get_mvp(view, ortho)); }; }
      callback_set_uniform set_screensize(glpp::context & context) {
         return [&context](glpp::uniform & u) {
            auto dims = context.win().frame_buffer_dims();
            u.set(glm::vec2{ dims.x, dims.y });
         };
      }
      callback_set_uniform set_view(game::renderer const & view) { return [&view](glpp::uniform & u) { u.set(get_view(view)); }; }
   };


   glpp::animation_timeline_t first_animation(glpp::scene_t const & scene) {
      auto animation_names = scene.animation_names();
      auto animation_name = animation_names[0];
      return scene.animation(animation_name).create_timeline();
   }
}


namespace game {
   //
   // scene_repository implementation
   //

   model_repository::model_repository(glpp::archive_t const & archive) {
      scenes.emplace("dude", archive.load_scene("dude-anim.fbx"));
      scenes.emplace("campfire", archive.load_scene("campfire.fbx"));
      scenes.emplace("props", archive.load_scene("trees.fbx"));
      scenes.emplace("weapons", archive.load_scene("weapons.fbx"));
   }

   glpp::scene_t const & model_repository::find_scene_by_name(std::string const & name) const {
      auto it = scenes.find(name);
      if (it == scenes.end()) throw std::runtime_error((std::string("cannot find 3D scene '") + name + "'").c_str());
      return it->second;
   }


   //
   // context_3d implementation
   //

   //static const glpp::context::resolution_t default_res{ 1920, 1080 };
   static const glpp::context::resolution_t default_res{ 1280, 800 };

   render_context::render_context(glpp::archive_t const & assets, glpp::context::key_callback_t key_callback)
      : context(glpp::context::resolution_t::nearest(default_res), key_callback)
      , ui_context{ glpp::imgui::init(context, UI_TEXTURE_UNIT) }
      , assets(assets)
      , prg_3d{ create_program(assets, "3d") }
      , prg_3d_shadow{ create_program(assets, "3d_shadow") }
      , prg_3d_particle{ create_program(assets, "3d_particle") }
      , prg_post{ create_program(assets, "post") }
      , prg_ui{ glpp::make_debug_ui_program() }
      , blank_tex(assets.load_image("blank-1x1.png"))
      , test_tex(assets.load_image("test-100x100.png"))
      , ground_tex(assets.load_image("ground-64x64.png"), false, true)
      , rain_tex(assets.load_image("rain.png"), true, false)
      , pos_shadow_tex{ std::unique_ptr<glpp::cube_map_texture_t>() }
      , dir_shadow_tex{ std::unique_ptr<glpp::texture_t>() }
      , post_tex{ std::unique_ptr<glpp::texture_t>() }
      , pos_shadow_fbo{ std::unique_ptr<glpp::frame_buffer_t>() }
      , dir_shadow_fbo{ std::unique_ptr<glpp::frame_buffer_t>() }
      , tex_fbo{ std::unique_ptr<glpp::frame_buffer_t>() }
      , msaa_fbo{ std::unique_ptr<glpp::frame_buffer_t>() }
   {
      auto extensions = context.extensions();
      utils::log(utils::LOG_INFO, "%s\n", context.info(false).c_str());
      utils::log(utils::LOG_INFO, "GL EXTENSIONS [%s]:\n", std::to_string(extensions.size()).c_str());
      for (auto e : extensions)
         utils::log(utils::LOG_INFO, " - %s\n", e.c_str());

      init_context();
   }

   void render_context::reload_shaders() {
      try {
         ::reload_program(assets, prg_3d, "3d");
         ::reload_program(assets, prg_3d_particle, "3d_particle");
         ::reload_program(assets, prg_3d_shadow, "3d_shadow");
         ::reload_program(assets, prg_post, "post");
      }
      catch (glpp::shader_compile_error const & ex) {
         utils::log(utils::LOG_ERROR, "%s\n", ex.what());
         utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
      }
   }

   bool render_context::reload_framebuffers() {
      auto dims = context.win().frame_buffer_dims();
      if (dims.x == 0 || dims.y == 0) return false;

      auto pos_shadow_texture_dims = glpp::dim_t{ pos_shadow_texture_width, pos_shadow_texture_width };
      auto dir_shadow_texture_dims = glpp::dim_t{ dir_shadow_texture_width, dir_shadow_texture_width};

#ifdef WIN32 
      const glpp::texture_format_t tex_fmt = glpp::texture_format_t::BGRA;
#else
      const glpp::texture_format_t tex_fmt = glpp::texture_format_t::RGBA;
#endif
      if (!pos_shadow_fbo || pos_shadow_fbo->dims() != pos_shadow_texture_dims) {
         pos_shadow_tex.reset(new glpp::cube_map_texture_t(pos_shadow_texture_dims.x, tex_fmt));
         pos_shadow_fbo.reset(new glpp::frame_buffer_t(*pos_shadow_tex));
      }
      if (!dir_shadow_fbo || dir_shadow_fbo->dims() != dir_shadow_texture_dims) {
         dir_shadow_tex.reset(new glpp::texture_t(dir_shadow_texture_dims, tex_fmt));
         dir_shadow_fbo.reset(new glpp::frame_buffer_t(*dir_shadow_tex));
      }
      if (!tex_fbo || tex_fbo->dims() != dims) {
         post_tex.reset(new glpp::texture_t(dims, tex_fmt));
         tex_fbo.reset(new glpp::frame_buffer_t(*post_tex));
      }
      if (!msaa_fbo || msaa_fbo->dims() != dims) {
         msaa_fbo.reset(new glpp::frame_buffer_t(dims, 8));
      }

      return true;
   }

   void render_context::toggle_fullscreen() {
      utils::log(utils::LOG_INFO, "toggling fullscreen - resetting fbos\n");

      context.win().set_fullscreen(!context.win().is_fullscreen());

      init_context();

      // force deallocation of all FBOs after context reinit to get around this ANGLE bug:
      // https://code.google.com/p/angleproject/issues/detail?id=979
      // ideally we would let the resize-handling code re-allocate the FBOs
      pos_shadow_fbo.reset();
      dir_shadow_fbo.reset();
      tex_fbo.reset();
      msaa_fbo.reset();
   }

   glpp::context::resolution_t render_context::resolution() {
      return context.resolution();
   }

   void render_context::set_resolution(glpp::context::resolution_t const & res) {
      utils::log(utils::LOG_INFO, "updating resolution - resetting fbos\n");

      context.set_resolution(res);

      init_context();

      // force deallocation of all FBOs after context reinit to get around this ANGLE bug:
      // https://code.google.com/p/angleproject/issues/detail?id=979
      // ideally we would let the resize-handling code re-allocate the FBOs
      pos_shadow_fbo.reset();
      dir_shadow_fbo.reset();
      tex_fbo.reset();
      msaa_fbo.reset();
   }


   void render_context::init_context() {
      gl::enable(gl::enable_cap_t::depth_test);
      gl::enable(gl::enable_cap_t::dither);
      gl::enable(gl::enable_cap_t::cull_face);
      //gl::enable(gl::enable_cap_t::multisample);
      gl::enable(gl::enable_cap_t::blend);

#ifndef _MSC_VER
      gl::point_size(3.);
      gl::enable(gl::enable_cap_t::point_smooth);
#endif

      gl::blend_func(gl::blending_factor_src_t::src_alpha, gl::blending_factor_dest_t::one_minus_src_alpha);

      gl::cull_face(gl::cull_face_mode_t::back);
   }


   //
   // view_3d implementation
   //

   renderer::renderer(model_repository const & models, render_context & ctx)
      : context(ctx)
      , player_light{{ 400., 40., -424. }, { .75, .40, .05 }, .04f * glm::vec3{ .75, .40, .05 }, .035f, .000005f}
   {
      emitter.set_creation_rate(1000);

      //// for some light colours: http://planetpixelemporium.com/tutorialpages/light.html
      //// fire (black body palette) http://pages.cs.wisc.edu/~dekruijf/docs/capstone.pdf:
      //const lowp vec3 COLOUR_FIRE_LOW = vec3(.75, .40, .05);
      //const lowp vec3 COLOUR_FIRE_MID = vec3(.80, .55, .10);
      //const lowp vec3 COLOUR_FIRE_HOT = vec3(.95, .70, .15);

      lights[0] = { { 400., 40., -424. },{ .75, .40, .05 }, .2f * glm::vec3{ .75, .40, .05 }, .0f, .00005f };

      lights[1].diffuse_colour = { .05, .45, .05 };
      lights[1].ambient_colour = { .05, .45, .05 };
      lights[1].attenuation_linear = .07f;
      lights[1].attenuation_square = .000005f;
      lights[2].diffuse_colour = { .45, .05, .05 };
      lights[2].ambient_colour = { .45, .05, .05 };
      lights[2].attenuation_linear = .07f;
      lights[2].attenuation_square = .000005f;
      lights[3].diffuse_colour = { .05, .05, .45 };
      lights[3].ambient_colour = { .05, .05, .45 };
      lights[3].attenuation_linear = .07f;
      lights[3].attenuation_square = .000005f;

      auto accumulate_scene_renderers = [&](glpp::scene_t const & scene) {
         auto & static_animation = scene.default_animation();
         mesh_renderers.push_back(make_render_info(context, static_animation, context.prg_3d, context.prg_3d_shadow));
         for (auto idx = 0U; idx < static_animation.child_count(); idx++) {
            auto & static_animation_child = static_animation.child(idx);
            mesh_renderers.push_back(make_render_info(context, static_animation_child, context.prg_3d, context.prg_3d_shadow));
         }
         for (auto anim_name : scene.animation_names()) {
            mesh_renderers.push_back(make_render_info(context, scene.animation(anim_name), context.prg_3d, context.prg_3d_shadow));
         }
      };

      // render info for all models
      accumulate_scene_renderers(models.find_scene_by_name("dude"));
      accumulate_scene_renderers(models.find_scene_by_name("campfire"));
      accumulate_scene_renderers(models.find_scene_by_name("props"));
      accumulate_scene_renderers(models.find_scene_by_name("weapons"));


      //
      // debug diamond pass
      //

      debug_diamond_pass.push_back(context.prg_3d.pass()
         .with(diamond_vert_buffer(context.prg_3d))
         .with(diamond_normal_buffer(context.prg_3d))
         .set_uniform("colour", glm::vec4(.1f, .8f, .2f, 1.f))
         .set_uniform_action("model", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); })
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_mvp(*this, ortho) * get_diamond_model_matrix()); }));
      //         .set_uniform_action("normal_matrix", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); });



      //
      // ground pass
      //

      auto ground_buffer_desc = glpp::buffer_attrib_mappings_t()
         .push_attrib("p", 3)
         .push_attrib("normal", 3)
         .push_attrib("tex_coords", 2)
         .map_buffer(context.prg_3d, { ground_verts, ground_indices });
      auto ground_bone_indices_buffer_desc = glpp::buffer_attrib_mappings_t()
         .push_attrib("bone_indices", 4)
         .map_buffer(context.prg_3d, glpp::static_array_t{ default_bone_indices });
      auto ground_bone_weights_buffer_desc = glpp::buffer_attrib_mappings_t()
         .push_attrib("bone_weights", 4)
         .map_buffer(context.prg_3d, glpp::static_array_t{ default_bone_weights });

      auto set_ground_tex_cb = [this](glpp::uniform & u) { GROUND_TEXTURE_UNIT.activate(); context.ground_tex.bind();  u.set(GROUND_TEXTURE_UNIT); };
      ground_pass.push_back(context.prg_3d.pass()
         .with(ground_buffer_desc)
         .with(ground_bone_indices_buffer_desc)
         .with(ground_bone_weights_buffer_desc)
         .set_uniform("colour", glm::vec4(.8f, .8f, .16f, 1.f))
         .set_uniform("use_texture", 1.f)
         .set_uniform_action("texture", set_ground_tex_cb)
         .set_uniform("model", glm::mat4{})
         .set_uniform_action("mvp", callbacks::set_mvp(*this, ortho))
         //         .set_uniform("normal_matrix", glm::mat4{})
         .set_uniform_action("t", callbacks::set_time())
         .set_uniform("bones[0]", default_bone_transforms));


      //
      // particle pass
      //

      //auto emitter_buffer_desc = glpp::describe_buffer(emitter.buffer())
      //   .attrib("position", 3);
      //particle_pass.push_back(context.prg_3d_particle.pass()
      //   .with(emitter_buffer_desc)
      //   .set_uniform_action("screen_size", callbacks::set_screensize(context.context))
      //   .set_uniform("proj", glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1500.f))
      //   .set_uniform_action("view", callbacks::set_view(*this)));


      static float particle_static_data[] = {
         -2., 0.,    0., 1.,  // x, y,   s, t
         -2., -90.,  0., 0.,  // x, y,   s, t
         -2., 0.,    1., 1.,  // x, y,   s, t
         2., 0.,    1., 1.,  // x, y,   s, t
         -2., -90.,  0., 0.,  // x, y,   s, t
         2., -90.,  1., 0.,  // x, y,   s, t
      };

      particle_position_buffer = glpp::buffer_attrib_mappings_t()
         .push_attrib("position", 3)
         .map_buffer(context.prg_3d_particle, emitter.buffer());
      particle_mesh_buffer = glpp::buffer_attrib_mappings_t()
         .push_attrib("offset_coords", 2)
         .push_attrib("tex_coords", 2)
         .map_buffer(context.prg_3d_particle, { particle_static_data });

      static float grass_static_data[] = {
         -8, 0, -5, 0, -7, 9,
         -5, 0, -6, 10, -7, 9,

         -4, 0, -2, 0, -4, 35,
         -2, 0, -3, 10, -4, 35,

         -1, 0, 2, 0, 0, 22,
         2, 0, 1, 30, 0, 22,

         3, 0, 5, 0, 4, 5,
      };

      grass_position_buffer = glpp::buffer_attrib_mappings_t()
         .push_attrib("position", 3)
         .map_buffer(context.prg_3d_particle, { grass.buffer() });
      grass_mesh_buffer = glpp::buffer_attrib_mappings_t()
         .push_attrib("offset_coords", 2)
         .map_buffer(context.prg_3d_particle, { grass_static_data });

      //
      // post-processing pass
      //

      // TODO: dont hard-code the texture unit
      auto set_post_tex_cb = [this](glpp::uniform & u) { POST_TEXTURE_UNIT.activate(); context.post_tex->bind();  u.set(POST_TEXTURE_UNIT); };
      post_pass.push_back(context.prg_post.pass()
         .with(screen_vertices_buffer(context.prg_post))
         .set_uniform_action("texture", set_post_tex_cb)
         .set_uniform_action("t", callbacks::set_time()));

   }

   namespace {
      std::vector<glpp::pass_t> & mesh_passes_for_animation(renderer & state, glpp::animation_t const & animation, bool shadow) {
         for (auto & render_info : state.mesh_renderers) {
            if (animation == render_info.animation) {
               return shadow ? render_info.d3_shadow_mesh_passes : render_info.d3_mesh_passes;
            }
         }

         assert(false && "cannot find mesh passes for animation");
         utils::log(utils::LOG_WARN, "cannot find mesh passes for animation %s\n", animation.name().c_str());
         static std::vector<glpp::pass_t> default_passes;
         return default_passes;
      }
   }

   namespace {
      const float PARTICLE_MAX_LIFE = 15.f;
      float particle_lifecycles[3] = { 0., 5., 10. };

      glm::vec2 signal_2d(float t) { return glm::rotate(glm::vec2{1.f, 0.f}, t * glpp::TAU_F); }
   }

   void renderer::update_and_render(double time_since_last_tick, game::world_view_t const & world_view, bool show_ui) {
      struct stage_t {
         std::string name;
         double time_begin;
      };
      static const unsigned MAX_STAGES = 10;
      stage_t stages[MAX_STAGES];
      static double stage_times[MAX_STAGES] = {};
      auto stage_count = 0U;
      auto begin_stage = [&](std::string const & name) {
         assert(stage_count < MAX_STAGES);
         stages[stage_count++] = {name, glpp::get_time()};
      };
      auto time_stage = [&](unsigned stage_idx, double now) {
         auto next_idx = stage_idx + 1;
         auto next_start = next_idx == stage_count ? now : stages[next_idx].time_begin;
         auto stage_time = next_start - stages[stage_idx].time_begin;
         auto avg_stage_time = stage_times[stage_idx];
         avg_stage_time = avg_stage_time == 0. ? stage_time : (avg_stage_time * 9 + stage_time) / 10.;
         stage_times[stage_idx] = avg_stage_time;
         return avg_stage_time;
      };

      begin_stage("update");

      context.init_context();
      if (!context.reload_framebuffers()) {
         return;
      }

      const float LIGHT_POSITION_MIDNIGHT = .1f;
      const float LIGHT_INTENSITY_MIDNIGHT = .001f;
      const glm::vec3 LIGHT_COLOUR_MIDNIGHT{ .572, .553, .847 };

      const float LIGHT_POSITION_MORNING = .33f;
      const float LIGHT_INTENSITY_MORNING = .5f;
      const glm::vec3 LIGHT_COLOUR_MORNING{ .6, .6, .6 };

      const float LIGHT_POSITION_MIDDAY = .5f;
      const float LIGHT_INTENSITY_MIDDAY = .85f;
      const glm::vec3 LIGHT_COLOUR_MIDDAY{ 1., 1., 1. };

      const float LIGHT_POSITION_EVENING = .7f;
      const float LIGHT_INTENSITY_EVENING = .5f;
      const glm::vec3 LIGHT_COLOUR_EVENING{ .847, .553, .572 };

      static float sky_light_position = LIGHT_POSITION_MIDNIGHT;
      static glm::vec3 sky_light_colour = LIGHT_COLOUR_MIDNIGHT;
      static float sky_light_intensity = LIGHT_INTENSITY_MIDNIGHT;

      static glm::vec3 ambient_colour{ .2, .6, .8 };
      static float ambient_intensity = .0f;

      static float target_sky_light_position = sky_light_position;
      static glm::vec3 target_sky_light_colour = sky_light_colour;
      static float target_sky_light_intensity = sky_light_intensity;
      static glm::vec3 target_ambient_colour = ambient_colour;
      static float target_ambient_intensity = ambient_intensity;

      sky_light_position = utils::lerp(sky_light_position, target_sky_light_position, static_cast<float>(time_since_last_tick * 5.));
      sky_light_colour = utils::lerp(sky_light_colour, target_sky_light_colour, static_cast<float>(time_since_last_tick * 5.));
      sky_light_intensity = utils::lerp(sky_light_intensity, target_sky_light_intensity, static_cast<float>(time_since_last_tick * 5.));
      ambient_intensity = utils::lerp(ambient_intensity, target_ambient_intensity, static_cast<float>(time_since_last_tick * 5.));
      ambient_colour = utils::lerp(ambient_colour, target_ambient_colour, static_cast<float>(time_since_last_tick * 5.));
      g_sky_light_position = sky_light_position;

      bool is_daytime = sky_light_position > .3f && sky_light_position < .7f;

      static glm::vec3 grass_colour = glpp::norm_hex_value(glm::vec3{60.f, 160.f, 20.f});
      static float grass_scale{ .6f };
      static float rain_scale{ .9f };

      //
      // update animations
      //
      emitter.update(time_since_last_tick);
      grass.update(time_since_last_tick);
//      dude_walk_animation.advance_by(time_since_last_tick);

      for (auto i = 0U; i < 3; i++) {
         particle_lifecycles[i] += static_cast<float>(time_since_last_tick);
         auto r = (float)(200. + 50. * (i + 2));
         auto t = 2.f * (((i + 4.f) / 5.f) * particle_lifecycles[i]) / PARTICLE_MAX_LIFE;
         glm::vec2 pos = r * .5f * signal_2d(t) + r * .5f * signal_2d(t + t * .3f) + r * .1f * signal_2d(-t * 11.f) + r * .01f * signal_2d(t * 39.f);
         pos += glm::vec2{300.f, -500.f};
         auto height = 60.f + 55.f * std::sin(20.f * t) + 25.f * std::sin(.2f + t * 30.f);
         lights[i + 1].position = { pos.x, height, -pos.y };
      }

      auto default_entity_filter = [](auto const &) { return true; };
      auto render_entity_meshes = [&](
         auto entity_it,
         auto entity_it_end,
         auto entity_filter,
         glm::mat4 const & view_mat,
         glm::mat4 const & proj_mat,
         bool use_shadow_passes) {
         while (entity_it != entity_it_end) {
            auto & entity_info = *entity_it;
            auto & passes = mesh_passes_for_animation(*this, entity_info.sprite->animation(), use_shadow_passes);

            auto same_entity_type = [&](auto & entity_info_in) {
               return entity_info_in.sprite->animation() == entity_info.sprite->animation();
            };
            auto entity_partition_end = std::partition_point(entity_it, entity_it_end, same_entity_type);

            auto ambient_light = mesh_render_batch_callback_t::light_t{};
            auto sky_light = mesh_render_batch_callback_t::light_t{};
            if (!use_shadow_passes) {
               ambient_light = mesh_render_batch_callback_t::light_t{ambient_colour, ambient_intensity};
               sky_light = mesh_render_batch_callback_t::light_t{sky_light_colour, sky_light_intensity, sky_light_position};
            }

            auto pass_mesh_idx = 0U;
            for (auto & pass : passes) {
               pass.draw_batch(
                  mesh_render_batch_callback_t{
                     pass_mesh_idx++,
                     entity_it,
                     entity_partition_end,
                     view_mat, proj_mat,
                     entity_filter,
                     ambient_light, sky_light},
                  glpp::DrawMode::Triangles);
            }

            entity_it = entity_partition_end;
         }
      };

      begin_stage("shadows");

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
#if 1

      gl::disable(gl::enable_cap_t::blend);
      gl::cull_face(gl::cull_face_mode_t::front);
      gl::clear_color(1., 1., 1., 1.);

      auto shadow_light = light_info{};
      auto dir_shadow_mvp_biased = glm::mat4{};

      //is_daytime = true;
      if (is_daytime) {
         gl::viewport(0, 0, dir_shadow_texture_width, dir_shadow_texture_width);

         const auto light_dir = calc_sky_light_direction(sky_light_position);

         // TODO: get rid of these magic numbers
         shadow_light.position = -light_dir * 500.f;
         shadow_light.diffuse_colour = sky_light_colour;
         shadow_light.ambient_colour = ambient_colour;

         const auto light_proj = glm::ortho(-1500.f, 1500.f, -1500.f, 1500.f, 200.f, 1000.f);
         const auto view = glm::lookAt(shadow_light.position, glm::vec3{ 0., 0., 0. }, glm::vec3{ 0., 0., -1. });

         static glm::mat4 bias_xform(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0);

         dir_shadow_mvp_biased = bias_xform * light_proj * view * glm::mat4{};

         context.dir_shadow_fbo->bind(glpp::frame_buffer_t::BindTarget::Draw);

         GL_VERIFY(gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit));

         context.prg_3d_shadow.use();
         context.prg_3d_shadow.uniform("shadow_lights[0].is_directional").set(is_daytime ? 1.f : 0.f);
         context.prg_3d_shadow.uniform("shadow_lights[0].world_position").set(shadow_light.position);

         // TODO: exclude entities outside view radius
         render_entity_meshes(world_view.creatures_begin(), world_view.creatures_end(), default_entity_filter, view, light_proj, true);
         render_entity_meshes(world_view.props_begin(), world_view.props_end(), default_entity_filter, view, light_proj, true);

         context.dir_shadow_fbo->unbind();
      } // end daytime shadow calculations
      else {
         // HOLY CRAP this took me ages to figure out people
         gl::viewport(0, 0, pos_shadow_texture_width, pos_shadow_texture_width);

         // const PositionalLight c_light1 = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), vec3(.9, .8, .1), .1);
         struct face_info {
            glpp::frame_buffer_t::CubeFace face;
            glm::vec3 view_direction;
            glm::vec3 up_direction;
            std::string name;
         };

         const face_info faces[6] = {
            { glpp::frame_buffer_t::POSITIVE_X,{ 1., 0., 0. },{ 0., -1., 0. }, "x_pos" },
            { glpp::frame_buffer_t::NEGATIVE_X,{ -1., 0., 0. },{ 0., -1., 0. }, "x_neg" },
            { glpp::frame_buffer_t::POSITIVE_Y,{ 0., 1., 0. },{ 0., 0., 1. }, "y_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Y,{ 0., -1., 0. },{ 0., 0., -1. }, "y_neg" },
            { glpp::frame_buffer_t::POSITIVE_Z,{ 0., 0., 1. },{ 0., -1., 0. }, "z_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Z,{ 0., 0., -1. },{ 0., -1., 0. }, "z_neg" },
         };

         shadow_light = player_light;
         const auto light_proj = glm::perspective((float)glm::radians(90.), 1.f, 10.f, 400.f);

         for (auto face_idx = 0; face_idx < 6; face_idx++) {
            auto & face = faces[face_idx];
            const auto view = glm::lookAt(shadow_light.position, shadow_light.position + face.view_direction, face.up_direction);

            context.pos_shadow_fbo->bind(face.face, glpp::frame_buffer_t::BindTarget::Draw);

            GL_VERIFY(gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit));

            auto filter = [&](game::world_view_t::render_info_t const & render_info) {
               // not casting shadows upwards
               if (face.face == glpp::frame_buffer_t::POSITIVE_Y) return false;

               auto light_to_entity = render_info.moment->pos() - glm::vec2(shadow_light.position.x, -shadow_light.position.z);
               auto light_height_squared = shadow_light.position.y * shadow_light.position.y;
               auto light_to_entity_dist_squared = light_to_entity.x * light_to_entity.x + light_to_entity.y * light_to_entity.y;

               // anything thats close to the light gets included
               if (light_to_entity_dist_squared < (2 * light_height_squared)) {
                  return true;
               }

               // TODO: exclude entities that are far away

               // not close to the light, wont cast a shadow underneath the light
               if (face.face == glpp::frame_buffer_t::NEGATIVE_Y) return false;

               auto light_to_entity_norm = glm::normalize(light_to_entity);
               auto dot = light_to_entity.x * face.view_direction.x + light_to_entity.y * -face.view_direction.z;

               return dot > 0;
            };

            context.prg_3d_shadow.use();
            context.prg_3d_shadow.uniform("shadow_lights[0].is_directional").set(is_daytime ? 1.f : 0.f);
            context.prg_3d_shadow.uniform("shadow_lights[0].world_position").set(shadow_light.position);

            render_entity_meshes(world_view.creatures_begin(), world_view.creatures_end(), filter, view, light_proj, true);
            render_entity_meshes(world_view.props_begin(), world_view.props_end(), filter, view, light_proj, true);
         }

         context.pos_shadow_fbo->unbind();
      } // end nighttime shadow calculations

      //gl::enable(gl::enable_cap_t::cull_face);
      gl::cull_face(gl::cull_face_mode_t::back);
      gl::enable(gl::enable_cap_t::blend);

      context.prg_3d.use();
      if (is_daytime) {
         SHADOW_TEXTURE_UNIT.activate();
         context.dir_shadow_tex->bind();
         context.prg_3d.uniform("pos_shadow_texture").set(DEFAULT_TEXTURE_UNIT);
         context.prg_3d.uniform("dir_shadow_texture").set(SHADOW_TEXTURE_UNIT);
         context.prg_3d.uniform("dir_shadow_mvp_biased").set(dir_shadow_mvp_biased);
      }
      else {
         SHADOW_TEXTURE_UNIT.activate();
         context.pos_shadow_tex->bind();
         context.prg_3d.uniform("dir_shadow_texture").set(DEFAULT_TEXTURE_UNIT);
         context.prg_3d.uniform("pos_shadow_texture").set(SHADOW_TEXTURE_UNIT);
      }
      context.prg_3d.uniform("shadow_lights[0].is_directional").set(is_daytime ? 1.f : 0.f);
      context.prg_3d.uniform("shadow_lights[0].world_position").set(shadow_light.position);
      context.prg_3d.uniform("shadow_lights[0].ambient").set(shadow_light.ambient_colour);
      context.prg_3d.uniform("shadow_lights[0].diffuse").set(shadow_light.diffuse_colour);
      context.prg_3d.uniform("shadow_lights[0].attenuation_linear").set(shadow_light.attenuation_linear);
      context.prg_3d.uniform("shadow_lights[0].attenuation_square").set(shadow_light.attenuation_square);

      for (auto i = 0U; i < 4; i++) {
         auto array_name = std::string{ "lights[" } + std::to_string(i) + "]";
         context.prg_3d.uniform(array_name + ".world_position").set(lights[i].position);
         context.prg_3d.uniform(array_name + ".ambient").set(lights[i].ambient_colour);
         context.prg_3d.uniform(array_name + ".diffuse").set(lights[i].diffuse_colour);
         context.prg_3d.uniform(array_name + ".attenuation_linear").set(lights[i].attenuation_linear);
         context.prg_3d.uniform(array_name + ".attenuation_square").set(lights[i].attenuation_square);
      }

#endif

      //
      // draw to anti-aliasing frame buffer
      //
      begin_stage("scene");

      auto dims = context.context.win().frame_buffer_dims();
      gl::viewport(0, 0, dims.x, dims.y);

#ifdef _MSC_VER
#  define USE_MSAA_FBO
#else
#  define USE_POST_PROCESSING_FBO
#endif

#if defined(USE_MSAA_FBO)
      context.msaa_fbo->bind();
#  ifndef USE_POST_PROCESSING_FBO
#    define USE_POST_PROCESSING_FBO
#  endif
#endif
#if !defined(USE_MSAA_FBO) && defined(USE_POST_PROCESSING_FBO)
      context.tex_fbo->bind();
#endif
      {
         gl::clear_color(.01f, .01f, .1f, 1.f);
         gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit);

#if 1
         ground_pass.back().draw(glpp::DrawMode::Triangles);
#endif

#if 1
         render_entity_meshes(world_view.creatures_begin(), world_view.creatures_end(), default_entity_filter, get_view(*this), get_proj(ortho), false);
         render_entity_meshes(world_view.props_begin(), world_view.props_end(), default_entity_filter, get_view(*this), get_proj(ortho), false);
#endif

         //debug_diamond_pass.back().draw(glpp::DrawMode::Triangles);

     //    gl::disable(gl::enable_cap_t::cull_face);
#if 1

         context.prg_3d_particle.use();
         context.prg_3d_particle.uniform("screen_size").set(glm::vec2{ dims.x, dims.y });
         context.prg_3d_particle.uniform("view").set(get_view(*this));
         context.prg_3d_particle.uniform("eye").set(get_camera_pos(*this));

         context.prg_3d_particle.uniform("shadow_lights[0].world_position").set(player_light.position);
         context.prg_3d_particle.uniform("shadow_lights[0].ambient").set(player_light.ambient_colour);
         context.prg_3d_particle.uniform("shadow_lights[0].diffuse").set(player_light.diffuse_colour);
         context.prg_3d_particle.uniform("shadow_lights[0].attenuation_linear").set(player_light.attenuation_linear);
         context.prg_3d_particle.uniform("shadow_lights[0].attenuation_square").set(player_light.attenuation_square);

         context.prg_3d_particle.uniform("ambient_colour").set(ambient_colour);
         context.prg_3d_particle.uniform("ambient_intensity").set(ambient_intensity);

         //context.prg_3d_particle.uniform("sky_light_dir").set(sky_light_.moment);
         context.prg_3d_particle.uniform("sky_light_colour").set(sky_light_colour);
         context.prg_3d_particle.uniform("sky_light_intensity").set(sky_light_intensity);

         for (auto i = 0U; i < 4; i++) {
            auto array_name = std::string{ "lights[" } +std::to_string(i) + "]";
            context.prg_3d_particle.uniform(array_name + ".world_position").set(lights[i].position);
            context.prg_3d_particle.uniform(array_name + ".ambient").set(lights[i].ambient_colour);
            context.prg_3d_particle.uniform(array_name + ".diffuse").set(lights[i].diffuse_colour);
            context.prg_3d_particle.uniform(array_name + ".attenuation_linear").set(lights[i].attenuation_linear);
            context.prg_3d_particle.uniform(array_name + ".attenuation_square").set(lights[i].attenuation_square);
         }
#if 1
         // grass
         context.prg_3d_particle.uniform("scale").set(grass_scale);
         context.prg_3d_particle.uniform("proj").set(get_proj(ortho));
         context.prg_3d_particle.uniform("colour").set(glm::vec4(grass_colour, 1.));
         context.prg_3d_particle.uniform("override_alpha").set(1.f);
         context.prg_3d_particle.uniform("texture").set(DEFAULT_TEXTURE_UNIT);

         glpp::bind(grass_position_buffer);
         gl::vertex_attrib_divisor(context.prg_3d_particle.attrib("position").location(), 1);

         glpp::bind(grass_mesh_buffer);

         GL_VERIFY(gl::draw_arrays_instanced(gl::primitive_type_t::triangles, 0, 21, grass.count()));

         gl::vertex_attrib_divisor(context.prg_3d_particle.attrib("position").location(), 0);
         glpp::unbind(grass_mesh_buffer);
         glpp::unbind(grass_position_buffer);
#endif

#if 1
         // rain
         if (ortho) {
            gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
         }

         RAIN_TEXTURE_UNIT.activate();
         context.rain_tex.bind();
         context.prg_3d_particle.uniform("texture").set(RAIN_TEXTURE_UNIT);
         context.prg_3d_particle.uniform("scale").set(rain_scale);
         context.prg_3d_particle.uniform("proj").set(get_proj(false));
         context.prg_3d_particle.uniform("colour").set(glm::vec4(0.));
         context.prg_3d_particle.uniform("override_alpha").set(-1.f);

         glpp::bind(particle_position_buffer);
         gl::vertex_attrib_divisor(context.prg_3d_particle.attrib("position").location(), 1);

         glpp::bind(particle_mesh_buffer);

         GL_VERIFY(gl::draw_arrays_instanced(gl::primitive_type_t::triangles, 0, 6, emitter.count()));

         gl::vertex_attrib_divisor(context.prg_3d_particle.attrib("position").location(), 0);
         glpp::unbind(particle_mesh_buffer);
         glpp::unbind(particle_position_buffer);
#endif
#endif

         //gl::enable(gl::enable_cap_t::cull_face);

      }
#if defined(USE_MSAA_FBO)
      context.msaa_fbo->bind();
      // TODO: tex_fbo should be a non-MSAA renderbuffer (not texture)
      context.tex_fbo->bind(glpp::frame_buffer_t::Draw);
      context.msaa_fbo->bind(glpp::frame_buffer_t::Read);
      {
         gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit);

         context.msaa_fbo->blit_to_draw_buffer();
      }
      context.tex_fbo->unbind();
      context.msaa_fbo->unbind();
#endif
#if !defined(USE_MSAA_FBO) && defined(USE_POST_PROCESSING_FBO)
      context.tex_fbo->unbind();
#endif

#if defined(USE_POST_PROCESSING_FBO)
      static float brightness = 1.f;
      static float saturation = .5f;
      static float contrast = 1.f;
      static float gamma = 2.2f;
      auto & pass = post_pass.back();
      pass.set_uniform("brightness", brightness);
      pass.set_uniform("saturation", saturation);
      pass.set_uniform("contrast", contrast);
      pass.set_uniform("gamma", gamma);
      pass.draw(glpp::DrawMode::Triangles);
#endif

      gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
      glpp::imgui::new_frame(context.ui_context);

      static float alpha = .5f;

      const auto FIXED_POPUP_SETTINGS = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
      const auto FIXED_OVERLAY_SETTINGS = FIXED_POPUP_SETTINGS | ImGuiWindowFlags_NoTitleBar;

      static bool show_debug_window = true;
      if (show_debug_window) {
         ImGui::SetNextWindowPos(ImVec2(10, 10));
         if (ImGui::Begin("Render Info", &show_debug_window, {}, alpha, FIXED_OVERLAY_SETTINGS)) {
            if (stage_count > 0) {
               auto now = glpp::get_time();
               auto total_time = now - stages[0].time_begin;

               ImGui::SetNextTreeNodeOpened(false, ImGuiSetCond_FirstUseEver);
               if (ImGui::TreeNode("render info root", "render: %.1fms (%.1f FPS)", total_time * 1000., 1.f / total_time))
               {
                  for (auto stage_idx = 0U; stage_idx < stage_count; stage_idx++) {
                     auto stage_time = time_stage(stage_idx, now);
                     ImGui::Text("%s: %.1fms (%.1f%%)", stages[stage_idx].name.c_str(), stage_time * 1000., 100. * stage_time / total_time);
                  }
                  ImGui::TreePop();
               }
               ImGui::SetNextTreeNodeOpened(false, ImGuiSetCond_FirstUseEver);
               if (ImGui::TreeNode("entity info root", "scene: %d entities", world_view.entities_count()))
               {
                  if (ImGui::TreeNode("camera root", "camera")) {
                     ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
                     ImGui::SliderFloat("height", &g_camera_height, 0.f, 5000.f, "%.0f");
                     ImGui::SameLine();
                     ImGui::SliderFloat("dist", &g_camera_distance, 100.f, 5000.f, "%.0f");
                     ImGui::PopItemWidth();
                     ImGui::SliderFloat("fov", &g_camera_fov, 10.f, 110.f, "%.1f");
                     ImGui::TreePop();
                  }
                  if (ImGui::Button("morning")) {
                     target_sky_light_position = LIGHT_POSITION_MORNING;
                     target_sky_light_colour = LIGHT_COLOUR_MORNING;
                     target_sky_light_intensity = LIGHT_INTENSITY_MORNING;
                  }
                  ImGui::SameLine();
                  if (ImGui::Button("midday")) {
                     target_sky_light_position = LIGHT_POSITION_MIDDAY;
                     target_sky_light_colour = LIGHT_COLOUR_MIDDAY;
                     target_sky_light_intensity = LIGHT_INTENSITY_MIDDAY;
                  }
                  ImGui::SameLine();
                  if (ImGui::Button("evening")) {
                     target_sky_light_position = LIGHT_POSITION_EVENING;
                     target_sky_light_colour = LIGHT_COLOUR_EVENING;
                     target_sky_light_intensity = LIGHT_INTENSITY_EVENING;
                  }
                  ImGui::SameLine();
                  if (ImGui::Button("night")) {
                     target_sky_light_position = LIGHT_POSITION_MIDNIGHT;
                     target_sky_light_colour = LIGHT_COLOUR_MIDNIGHT;
                     target_sky_light_intensity = LIGHT_INTENSITY_MIDNIGHT;
                  }
                  if (ImGui::TreeNode("ambient root", "ambient light")) {
                     ImGui::ColorEdit3("colour", glm::value_ptr(target_ambient_colour));
                     ImGui::SliderFloat("strength", &target_ambient_intensity, 0.f, 1.f, "%.2f");
                     ImGui::TreePop();
                  }
                  if (ImGui::TreeNode("skylight root", "sky light")) {
                     ImGui::SliderFloat("position", &target_sky_light_position, 0.f, 1.f, "%.2f");
                     ImGui::ColorEdit3("colour", glm::value_ptr(target_sky_light_colour));
                     ImGui::SliderFloat("strength", &target_sky_light_intensity, 0.f, 1.f, "%.2f");
                     ImGui::TreePop();
                  }
                  if (ImGui::TreeNode("rain root", "rain")) {
                     static auto rain_particles_per_second = emitter.creation_rate();
                     if (ImGui::SliderInt("density", &rain_particles_per_second, 0, 10000)) {
                        emitter.set_creation_rate(rain_particles_per_second);
                     }                     
                     ImGui::SliderFloat("scale", &rain_scale, .1f, 25.f, "%.1f");
                     ImGui::TreePop();
                  }
                  if (ImGui::TreeNode("grass root", "grass")) {
                     static int grass_radius = grass.creation_radius();
                     if (ImGui::SliderInt("radius", &grass_radius, 0, 5000)) {
                        grass.set_creation_radius(grass_radius);
                     }
                     static int grass_count = grass.creation_count();
                     if (ImGui::SliderInt("density", &grass_count, 0, 100000)) {
                        grass.set_creation_count(grass_count);
                        grass.set_deletion_count(grass_count);
                     }
                     ImGui::ColorEdit3("colour", glm::value_ptr(grass_colour));
                     ImGui::SliderFloat("scale", &grass_scale, .1f, 25.f, "%.1f");
                     ImGui::TreePop();
                  }
                  ImGui::Text("creatures: %d", world_view.creatures_count());
                  ImGui::Text("props: %d", world_view.props_count());
                  ImGui::TreePop();
               }
            }
            ImGui::End();
         }
      }
   
      if (show_ui) {
         static bool show_test_window = false;
         if (show_test_window)
            ImGui::ShowTestWindow(&show_test_window);

         ImGui::SetNextWindowPosCenter();
         if (ImGui::Begin("Options", nullptr, {}, alpha, FIXED_OVERLAY_SETTINGS)) {
            if (ImGui::BeginPopupModal("Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
               auto resolutions = glpp::context::resolution_t::supported_name_c_str();
               auto resolution_selected = glpp::context::resolution_t::idx_of(context.resolution());

               if (ImGui::Combo("Resolution", &resolution_selected, &resolutions.front(), resolutions.size())) {
                  context.set_resolution(glpp::context::resolution_t::supported()[resolution_selected]);
               }

               bool fullscreen = context.context.win().is_fullscreen();
               if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
                  context.toggle_fullscreen();
               }

               ImGui::Checkbox("Orthogonal View", &ortho);

               ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
               ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);
               ImGui::SliderFloat("Saturation", &saturation, 0.0f, 2.0f);
               ImGui::SliderFloat("Gamma", &gamma, 0.5f, 4.0f);

               ImGui::SliderFloat("UI alpha", &alpha, 0.0f, 1.0f);
               ImGui::Checkbox("Show Debug UI", &show_debug_window);
               ImGui::Checkbox("Show Test UI", &show_test_window);
               ImGui::SameLine((int)ImGui::GetWindowWidth() - 60);
               if (ImGui::Button("OK", {50, 0})) { ImGui::CloseCurrentPopup(); }
               ImGui::EndPopup();
            }

            if (ImGui::Button("video...")) {
               ImGui::OpenPopup("Settings");
            }

            ImGui::Separator();
            if (ImGui::Button("quit")) {
               context.context.win().set_should_close();
            }
            ImGui::End();
         }
      }

      glpp::imgui::render(context.ui_context);
   }

} // namespace game
