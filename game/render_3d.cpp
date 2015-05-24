#include "render_3d.h"

#include <glpp/utils.h>

#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

namespace gl {
#ifdef _MSC_VER
   using namespace gles2;
#else
   using namespace gl2;
#endif
}

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
      0., 1., 0.,  // 0: top 
      0., 0., 1.,   1., 0., 0.,    0., 0., -1.,    -1., 0., 0.,   // 1-4: mid band
      0., -1., 0., // 5: bottom
   };

   const unsigned short diamond_mesh_indices[] = {
      0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 1, // top
      1, 5, 2,   2, 5, 3,   3, 5, 4,   4, 5, 1, // bottom
   };

   glpp::buffer_spec_builder_t diamond_vert_buffer() {
      return glpp::describe_buffer({ diamond_mesh_verts, diamond_mesh_indices })
         .attrib("p", 3);
   }

   glpp::buffer_spec_builder_t diamond_normal_buffer() {
      return glpp::describe_buffer({ diamond_mesh_verts })
         .attrib("normal", 3);
   }

   //
   // ground data
   //

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



   struct shadow_render_callback_t : public glpp::pass_t::render_callback {
      shadow_render_callback_t(
         game::world_view_t::const_iterator itBegin,
         game::world_view_t::const_iterator itEnd,
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

      game::world_view_t::const_iterator itEnd_;
      mutable game::world_view_t::const_iterator it_;
      glm::mat4 proj_view_matrix_;
   };

   struct mesh_render_callback_t : public glpp::pass_t::render_callback {
      mesh_render_callback_t(
         unsigned mesh_idx,
         game::world_view_t::const_iterator itBegin,
         game::world_view_t::const_iterator itEnd,
         glm::mat4 const & view_matrix,
         glm::mat4 const & proj_matrix)
         : mesh_idx_(mesh_idx)
         , itEnd_(itEnd)
         , it_(itBegin)
         , proj_view_matrix_(proj_matrix * view_matrix) {
      }

      bool prepare_next(glpp::program & p) const override {
         if (it_ == itEnd_) return false;

         auto & current_render_info = *it_;
         auto & moment = *current_render_info.moment;
         auto & animation = *current_render_info.sprite;

         auto model_transform = moment.mesh_transform();
         p.uniform("model").set(model_transform);
         p.uniform("mvp").set(proj_view_matrix_ * model_transform);
         //            p.uniform("normal_matrix").set(glm::transpose(glm::inverse(model_transform)));

         auto & mesh = animation.meshes()[mesh_idx_];
         p.uniform("bones[0]").set(mesh.bone_transforms());

         it_++;
         return true;
      }

   private:
      mesh_render_callback_t(mesh_render_callback_t const &) {}
      mesh_render_callback_t & operator=(mesh_render_callback_t const &) { return *this; }

      unsigned mesh_idx_;
      game::world_view_t::const_iterator itEnd_;
      mutable game::world_view_t::const_iterator it_;
      glm::mat4 proj_view_matrix_;
   };



   //
   // helper functions
   //

   void accumulate_mesh(glpp::mesh_t const & mesh, glpp::program & prg_3d, std::vector<glpp::pass_t> & passes_3d, glpp::program & prg_3d_shadow, std::vector<glpp::pass_t> & passes_3d_shadow) {
      auto verts = glpp::describe_buffer({ { mesh.vertices().buffer, mesh.vertices().count },{ mesh.indices().buffer, mesh.indices().count } })
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
      return glm::ortho<float>(-400., 400., -300., 300., 100., 2000.);
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
      auto animation_name = scene.animation_names()[0];
      return scene.create_timeline(animation_name);
   }
}


namespace game {

   //
   // context_3d implementation
   //

   render_context::render_context(glpp::context::key_callback_t key_callback)
      : context(key_callback)
      , assets{ glpp::archive_t::load_from_directory({ "../../res/" }) }
      , model_dude{ assets.load_scene("dude-anim.fbx") }
      , model_campfire{ assets.load_scene("campfire.fbx") }
      , prg_3d{ create_program(assets, "3d") }
      , prg_3d_shadow{ create_program(assets, "3d_shadow") }
      , prg_3d_particle{ create_program(assets, "3d_particle") }
      , prg_post{ create_program(assets, "post") }
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

#ifndef WIN32
      gl::point_size(3.);
      gl::enable(gl::enable_cap_t::point_smooth);
#endif

      gl::blend_func(gl::blending_factor_src_t::src_alpha, gl::blending_factor_dest_t::one_minus_src_alpha);

      gl::cull_face(gl::cull_face_mode_t::back);
   }


   //
   // view_3d implementation
   //

   renderer::renderer(render_context & ctx)
      : context(ctx)
      , dude_walk_animation{ first_animation(context.model_dude) }
   {
#if 0
      auto next_config = 1;
      std::map<std::string, int> mesh_weight_configs;
      auto add_config = [&](glpp::mesh_t const & m) -> int {
         auto ws = m.bone_weights();
         auto ws_str = std::string{};
         for (auto idx = 0U; idx < ws.count; idx++) {
            ws_str += std::to_string(ws.buffer[idx]) + ",";
         }
         auto it = mesh_weight_configs.find(ws_str);
         if (it != mesh_weight_configs.end()) return it->second;

         auto new_config = next_config++;
         mesh_weight_configs[ws_str] = new_config;
         return new_config;
      };

      utils::log(utils::LOG_INFO, "==========================\n     scene meshes\n==========================\n");
      for (auto & m : context.model_dude.meshes()) {
         utils::log(utils::LOG_INFO, "'%s' bones:%d weights-config:%d\n", m.name().c_str(), m.bone_count(), add_config(m));
      }
      auto t = context.model_dude.create_timeline(0);
      utils::log(utils::LOG_INFO, "==========================\n    timeline meshes\n==========================\n");
      for (auto & m : t.meshes()) {
         utils::log(utils::LOG_INFO, "'%s' bones:%d weights-config:%d\n", m.name().c_str(), m.bone_count(), add_config(m));
      }
#endif

      //
      // body passes
      //

      auto animation_name = context.model_dude.animation_names()[0];
      utils::log(utils::LOG_INFO, "== Animation '%s' Meshes ==\n", animation_name.c_str());
      for (auto & mesh : dude_walk_animation.meshes()) {
         utils::log(utils::LOG_INFO, " - mesh %s: %d bones, %d transforms\n", mesh.name().c_str(), mesh.bone_count(), mesh.bone_transforms().size());
         mesh_names.push_back(mesh.name());
         accumulate_mesh(mesh, context.prg_3d, d3_body_passes, context.prg_3d_shadow, d3_body_shadow_passes);
      }


      //
      // campfire passes
      //

      for (auto & mesh : context.model_campfire.meshes()) {
         mesh_names.push_back(mesh.name());
         accumulate_mesh(mesh, context.prg_3d, d3_campfire_passes, context.prg_3d_shadow, d3_campfire_shadow_passes);
      }


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
         .attrib("normal", 3);
      auto ground_bone_indices_buffer_desc = glpp::describe_buffer(glpp::static_array_t{ default_bone_indices })
         .attrib("bone_indices", 4);
      auto ground_bone_weights_buffer_desc = glpp::describe_buffer(glpp::static_array_t{ default_bone_weights })
         .attrib("bone_weights", 4);

      ground_pass.push_back(context.prg_3d.pass()
         .with(ground_buffer_desc)
         .with(ground_bone_indices_buffer_desc)
         .with(ground_bone_weights_buffer_desc)
         .set_uniform("colour", glm::vec4(.8f, .8f, .16f, 1.f))
         .set_uniform("model", glm::mat4{})
         .set_uniform_action("mvp", callbacks::set_mvp(*this))
         //         .set_uniform("normal_matrix", glm::mat4{})
         .set_uniform_action("t", callbacks::set_time())
         .set_uniform("bones[0]", default_bone_transforms));


      //
      // particle pass
      //

      auto emitter_buffer_desc = glpp::describe_buffer(emitter.buffer())
         .attrib("position", 3);
      particle_pass.push_back(context.prg_3d_particle.pass()
         .with(emitter_buffer_desc)
         .set_uniform_action("screen_size", callbacks::set_screensize(context.context))
         .set_uniform("proj", glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1500.f))
         .set_uniform_action("view", callbacks::set_view(*this)));


      //
      // post-processing pass
      //

      // TODO: dont hard-code the texture unit
      auto set_post_tex_cb = [this](glpp::uniform & u) { glpp::texture_unit_t tu{ 2 }; tu.activate(); context.post_tex->bind();  u.set(tu); };
      post_pass.push_back(context.prg_post.pass()
         .with(screen_vertices_spec())
         .set_uniform_action("texture", set_post_tex_cb)
         .set_uniform_action("t", callbacks::set_time()));

   }

   void renderer::update_and_render(double time_since_last_tick, game::world_view_t const & world_view) {
      context.reload_framebuffers();

      //
      // update animations
      //
      emitter.update(time_since_last_tick);
      dude_walk_animation.advance_by(time_since_last_tick);


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

      const auto light_pos = glm::vec3{ 400., 30., -300. };
      const auto light_proj = glm::perspective((float)glm::radians(90.), 1.f, 10.f, 400.f);

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

         for (auto & pass : d3_body_shadow_passes) {
            pass.draw_batch(
               shadow_render_callback_t{
                  world_view.creatures_begin(),
                  world_view.creatures_end(),
                  view, light_proj },
               glpp::DrawMode::Triangles);
         }
      }
      context.shadow_fbo->unbind();
      gl::cull_face(gl::cull_face_mode_t::back);
      gl::enable(gl::enable_cap_t::blend);

      context.prg_3d.use();
      glpp::texture_unit_t tu{ 3 };
      tu.activate();
      context.shadow_tex->bind();
      context.prg_3d.uniform("shadow_texture").set(tu);

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

         ground_pass.back().draw(glpp::DrawMode::Triangles);

         //gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
         static int prev_selected_item = -1;
         if (debug_selected_item_ != prev_selected_item) {
            prev_selected_item = debug_selected_item_;
            utils::log(utils::LOG_INFO, " !!! Selecting mesh '%s'\n", (debug_selected_item_ < (int)mesh_names.size()) ? mesh_names[debug_selected_item_].c_str() : "INVALID");
         }
         auto mesh_idx = 0U;
         for (auto & pass : d3_body_passes) {
            if (!debug_special_mode_enabled_ || mesh_idx == debug_selected_item_) {
               pass.draw_batch(
                  mesh_render_callback_t{
                     mesh_idx,
                     world_view.creatures_begin(),
                     world_view.creatures_end(),
                     get_view(*this), get_proj() },
                  glpp::DrawMode::Triangles);
            }
            mesh_idx++;
         }

         debug_diamond_pass.back().draw(glpp::DrawMode::Triangles);

         gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);

         particle_pass.back().draw(glpp::DrawMode::Points);
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
