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
   const int shadow_texture_width = 100;

   glpp::texture_unit_t BLANK_TEXTURE_UNIT{ 9 };
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

   glpp::buffer_spec_builder_t screen_vertices_spec() {
      return glpp::describe_buffer({ unit_square_verts, unit_square_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);
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

   glpp::buffer_spec_builder_t diamond_vert_buffer() {
      return glpp::describe_buffer({ diamond_mesh_verts, diamond_mesh_indices })
         .attrib("p", 3)
         .attrib("tex_coords", 2);
   }

   glpp::buffer_spec_builder_t diamond_normal_buffer() {
      return glpp::describe_buffer({ diamond_mesh_verts })
         .attrib("normal", 3);
   }

   //
   // ground data
   //

   static const float ground_verts[] = {
      -400.,   0., -1300.,   0., 1., 0.,    0., 5.,
      1200., 0., -1300.,     0., 1., 0.,    5.f * 1.33f, 5.,
      -400.,   0.,  500.,    0., 1., 0.,    0., 0.,
      1200., 0.,  500.,      0., 1., 0.,    5.f * 1.33f, 0.,
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

   struct mesh_render_batch_callback_t : public glpp::pass_t::render_batch_callback {
      using entity_filter = std::function<bool(game::world_view_t::render_info_t const &)>;

      mesh_render_batch_callback_t(
         unsigned pass_mesh_idx,
         game::world_view_t::const_iterator itBegin,
         game::world_view_t::const_iterator itEnd,
         glm::mat4 const & view_matrix,
         glm::mat4 const & proj_matrix,
         entity_filter filter)
         : pass_mesh_idx_(pass_mesh_idx)
         , itEnd_(itEnd)
         , it_(itBegin)
         , filter_(filter)
         , proj_view_matrix_(proj_matrix * view_matrix) {
      }

      bool prepare_next(glpp::program & p) const override {
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
   };



   //
   // helper functions
   //

   std::vector<glpp::pass_t> make_mesh_passes(game::render_context const & ctx, glpp::program & program, glpp::animation_t const & animation, bool shadow) {
      std::vector<glpp::pass_t> result;

      //auto animation = first_animation(scene);
      auto sprite = animation.create_timeline();
      for (auto & mesh : sprite.meshes()) {
         // utils::log(utils::LOG_INFO, " - mesh %s: %d bones, %d transforms\n", mesh.name().c_str(), mesh.bone_count(), mesh.bone_transforms().size());
         // mesh_names.push_back(mesh.name());

         auto verts = glpp::describe_buffer({ { mesh.vertices().buffer, mesh.vertices().count },{ mesh.indices().buffer, mesh.indices().count } })
            .attrib("p", 3);
         auto normals = glpp::describe_buffer({ { mesh.normals().buffer, mesh.normals().count } })
            .attrib("normal", 3);
         auto bone_indices = glpp::describe_buffer({ { mesh.bone_indices().buffer, mesh.bone_indices().count } })
            .attrib("bone_indices", 4);
         auto bone_weights = glpp::describe_buffer({ { mesh.bone_weights().buffer, mesh.bone_weights().count } })
            .attrib("bone_weights", 4);

         auto set_bones_action = [&](glpp::uniform & u) { u.set(mesh.bone_transforms()); };

         if (!shadow) {
            auto set_blank_tex_cb = [&](glpp::uniform & u) { BLANK_TEXTURE_UNIT.activate(); ctx.blank_tex.bind(); u.set(BLANK_TEXTURE_UNIT); };

            result.push_back(
               program.pass()
               .with(verts)
               .with(normals)
               .with(bone_indices)
               .with(bone_weights)
               .set_uniform("colour", mesh.material().diffuse_colour)
               .set_uniform_action("texture", set_blank_tex_cb)
               //.set_uniform_action("bones[0]", set_bones_action)
               );
         }
         else {
            result.push_back(
               program.pass()
               .with(verts)
               .with(bone_indices)
               .with(bone_weights)
               //.set_uniform_action("bones[0]", set_bones_action)
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


   glm::vec3 get_camera_pos(game::renderer const & view) {
      auto center_2d = game::center_world_location();
      auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
      auto eye = center + glm::vec3{ 0.f, 800.f * view.get_view_height(), 800.f };
      return eye;
   }

   glm::mat4 get_view(game::renderer const & view) {
      auto center_2d = game::center_world_location();
      auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
      auto eye = get_camera_pos(view);
      auto result = glm::lookAt(eye, center, glm::vec3{ 0., 1., 0. });
      return result;
   }

   glm::mat4 get_proj() {
      // 0, 0 is the bottom left of the lookAt target!
      return glm::ortho<float>(-800., 800., -600., 600., 100., 2000.);
      //return glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1000.f);
   }

   glm::mat4 get_mvp(game::renderer const & view) {
      return get_proj() * get_view(view);
   }

   namespace callbacks {
      callback_set_uniform set_time() { return [](glpp::uniform & u) {u.set(static_cast<float>(glpp::get_time())); }; }
      callback_set_uniform set_mvp(game::renderer const & view) { return [&view](glpp::uniform & u) { u.set(get_mvp(view)); }; }
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
      scenes.emplace("tree", archive.load_scene("trees.fbx"));
   }

   glpp::scene_t const & model_repository::find_scene_by_name(std::string const & name) const {
      auto it = scenes.find(name);
      if (it == scenes.end()) throw std::runtime_error((std::string("cannot find 3D scene '") + name + "'").c_str());
      return it->second;
   }


   //
   // context_3d implementation
   //

   render_context::render_context(glpp::archive_t const & assets, glpp::context::key_callback_t key_callback)
      : context(key_callback)
      , assets(assets)
      , prg_3d{ create_program(assets, "3d") }
      , prg_3d_shadow{ create_program(assets, "3d_shadow") }
      , prg_3d_particle{ create_program(assets, "3d_particle") }
      , prg_post{ create_program(assets, "post") }
      , blank_tex(assets.load_image("blank-1x1.png"))
      , test_tex(assets.load_image("test-100x100.png"))
      , ground_tex(assets.load_image("ground-64x64.png"))
      , rain_tex(assets.load_image("rain.png"))
      , shadow_tex{ std::unique_ptr<glpp::cube_map_texture_t>() }
      , post_tex{ std::unique_ptr<glpp::texture_t>() }
      , shadow_fbo{ std::unique_ptr<glpp::frame_buffer_t>() }
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

   void render_context::reload_framebuffers() {
      auto dims = context.win().frame_buffer_dims();

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
   }

   void render_context::toggle_fullscreen() {
      context.win().set_fullscreen(!context.win().is_fullscreen());

      init_context();

      // force deallocation of all FBOs after context reinit to get around this ANGLE bug:
      // https://code.google.com/p/angleproject/issues/detail?id=979
      // ideally we would let the resize-handling code re-allocate the FBOs
      shadow_fbo.reset();
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
   {
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
      accumulate_scene_renderers(models.find_scene_by_name("tree"));


      //
      // debug diamond pass
      //

      debug_diamond_pass.push_back(context.prg_3d.pass()
         .with(diamond_vert_buffer())
         .with(diamond_normal_buffer())
         .set_uniform("colour", glm::vec4(.1f, .8f, .2f, 1.f))
         .set_uniform_action("model", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); })
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_mvp(*this) * get_diamond_model_matrix()); }));
      //         .set_uniform_action("normal_matrix", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); });



      //
      // ground pass
      //

      auto ground_buffer_desc = glpp::describe_buffer({ ground_verts, ground_indices })
         .attrib("p", 3)
         .attrib("normal", 3)
         .attrib("tex_coords", 2);
      auto ground_bone_indices_buffer_desc = glpp::describe_buffer(glpp::static_array_t{ default_bone_indices })
         .attrib("bone_indices", 4);
      auto ground_bone_weights_buffer_desc = glpp::describe_buffer(glpp::static_array_t{ default_bone_weights })
         .attrib("bone_weights", 4);

      auto set_ground_tex_cb = [this](glpp::uniform & u) { GROUND_TEXTURE_UNIT.activate(); context.ground_tex.bind();  u.set(GROUND_TEXTURE_UNIT); };
      ground_pass.push_back(context.prg_3d.pass()
         .with(ground_buffer_desc)
         .with(ground_bone_indices_buffer_desc)
         .with(ground_bone_weights_buffer_desc)
         .set_uniform("colour", glm::vec4(.8f, .8f, .16f, 1.f))
         .set_uniform_action("texture", set_ground_tex_cb)
         .set_uniform("model", glm::mat4{})
         .set_uniform_action("mvp", callbacks::set_mvp(*this))
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

      particle_position_buffer = glpp::describe_buffer(emitter.buffer())
         .attrib("position", 3)
         .build(context.prg_3d_particle);
      particle_mesh_buffer = glpp::describe_buffer({ particle_static_data })
         .attrib("offset_coords", 2)
         .attrib("tex_coords", 2)
         .build(context.prg_3d_particle);

      //
      // post-processing pass
      //

      // TODO: dont hard-code the texture unit
      auto set_post_tex_cb = [this](glpp::uniform & u) { POST_TEXTURE_UNIT.activate(); context.post_tex->bind();  u.set(POST_TEXTURE_UNIT); };
      post_pass.push_back(context.prg_post.pass()
         .with(screen_vertices_spec())
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

   void renderer::update_and_render(double time_since_last_tick, game::world_view_t const & world_view) {
      context.reload_framebuffers();

      //
      // update animations
      //
      emitter.update(time_since_last_tick);
//      dude_walk_animation.advance_by(time_since_last_tick);



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

            auto pass_mesh_idx = 0U;
            for (auto & pass : passes) {
               pass.draw_batch(
                  mesh_render_batch_callback_t{
                     pass_mesh_idx++,
                     entity_it,
                     entity_partition_end,
                     view_mat, proj_mat,
                     entity_filter },
                  glpp::DrawMode::Triangles);
            }

            entity_it = entity_partition_end;
         }
      };



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
         { glpp::frame_buffer_t::POSITIVE_X,{ 1., 0., 0. },{ 0., -1., 0. }, "x_pos" },
         { glpp::frame_buffer_t::NEGATIVE_X,{ -1., 0., 0. },{ 0., -1., 0. }, "x_neg" },
         { glpp::frame_buffer_t::POSITIVE_Y,{ 0., 1., 0. },{ 0., 0., 1. }, "y_pos" },
         { glpp::frame_buffer_t::NEGATIVE_Y,{ 0., -1., 0. },{ 0., 0., -1. }, "y_neg" },
         { glpp::frame_buffer_t::POSITIVE_Z,{ 0., 0., 1. },{ 0., -1., 0. }, "z_pos" },
         { glpp::frame_buffer_t::NEGATIVE_Z,{ 0., 0., -1. },{ 0., -1., 0. }, "z_neg" },
      };

      const auto light_pos = glm::vec3{ 400., 30., -424. };
      const auto light_proj = glm::perspective((float)glm::radians(90.), 1.f, 10.f, 400.f);

#if 1
      gl::disable(gl::enable_cap_t::blend);
      gl::cull_face(gl::cull_face_mode_t::front);
      gl::clear_color(1., 1., 1., 1.);

      // HOLY CRAP this took me ages to figure out people
      gl::viewport(0, 0, shadow_texture_width, shadow_texture_width);

      for (auto face_idx = 0; face_idx < 6; face_idx++) {
         auto & face = faces[face_idx];
         const auto view = glm::lookAt(light_pos, light_pos + face.view_direction, face.up_direction);

         context.shadow_fbo->bind(face.face);

         GL_VERIFY(gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit));

         auto filter = [&](game::world_view_t::render_info_t const & render_info) {
            // not casting shadows upwards
            if (face.face == glpp::frame_buffer_t::POSITIVE_Y) return false;

            auto light_to_entity = render_info.moment->pos() - glm::vec2(light_pos.x, -light_pos.z);
            auto light_height_squared = light_pos.y * light_pos.y;
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
         render_entity_meshes(world_view.creatures_begin(), world_view.creatures_end(), filter, view, light_proj, true);
         render_entity_meshes(world_view.props_begin(), world_view.props_end(), filter, view, light_proj, true);
      }
      context.shadow_fbo->unbind();
      gl::cull_face(gl::cull_face_mode_t::back);
      gl::enable(gl::enable_cap_t::blend);

      context.prg_3d.use();
      SHADOW_TEXTURE_UNIT.activate();
      context.shadow_tex->bind();
      context.prg_3d.uniform("shadow_texture").set(SHADOW_TEXTURE_UNIT);
#endif

      //
      // draw to anti-aliasing frame buffer
      //

      auto dims = context.context.win().frame_buffer_dims();
      gl::viewport(0, 0, dims.x, dims.y);
      context.msaa_fbo->bind();
      {
         gl::clear_color(1., 1., 1., 1.);
         gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit);

#if 1
         ground_pass.back().draw(glpp::DrawMode::Triangles);
#endif

#if 1
         render_entity_meshes(world_view.creatures_begin(), world_view.creatures_end(), default_entity_filter, get_view(*this), get_proj(), false);
         render_entity_meshes(world_view.props_begin(), world_view.props_end(), default_entity_filter, get_view(*this), get_proj(), false);
#endif

         //debug_diamond_pass.back().draw(glpp::DrawMode::Triangles);

#if 1
         gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);

         context.prg_3d_particle.use();
         context.prg_3d_particle.uniform("screen_size").set(glm::vec2{ dims.x, dims.y });
         context.prg_3d_particle.uniform("proj").set(glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1500.f));
         context.prg_3d_particle.uniform("view").set(get_view(*this));
         context.prg_3d_particle.uniform("eye").set(get_camera_pos(*this));
         
         RAIN_TEXTURE_UNIT.activate();
         context.rain_tex.bind();
         context.prg_3d_particle.uniform("texture").set(RAIN_TEXTURE_UNIT);

         glpp::bind(particle_position_buffer);
         gl::vertex_attrib_divisor(context.prg_3d_particle.attrib("position").location(), 1);

         glpp::bind(particle_mesh_buffer);

         GL_VERIFY(gl::draw_arrays_instanced(gl::primitive_type_t::triangles, 0, 6, emitter.count()));

         gl::vertex_attrib_divisor(context.prg_3d_particle.attrib("position").location(), 0);
         glpp::unbind(particle_mesh_buffer);
         glpp::unbind(particle_position_buffer);
#endif
      }
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

      post_pass.back().draw(glpp::DrawMode::Triangles);
   }

} // namespace game
