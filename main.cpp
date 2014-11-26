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




   struct sprite_state_t {
      virtual ~sprite_state_t() {}

      virtual int current_state() const = 0;
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

   struct player_t : public sprite_state_t {
      enum class dir { none = 0, up = 1, down = 2, left = 4, right = 8};

      player_t() : dir_flags_(0), pos_(0., 0.) {
      }

      virtual int current_state() const {
         if (!is_moving()) return 0;
         return 1;
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
      }

      void update(double time_since_last) {
         static float pace_per_sec = 200.;
         float right_pace = 0.;
         if (dir_flags_ & static_cast<uint16_t>(dir::left)) right_pace -= pace_per_sec;
         if (dir_flags_ & static_cast<uint16_t>(dir::right)) right_pace += pace_per_sec;
         float up_pace = 0.;
         if (dir_flags_ & static_cast<uint16_t>(dir::up)) up_pace += pace_per_sec;
         if (dir_flags_ & static_cast<uint16_t>(dir::down)) up_pace -= pace_per_sec;

         if (right_pace != 0.) pos_.x += right_pace * static_cast<float>(time_since_last);
         if (up_pace != 0.) pos_.y += up_pace * static_cast<float>(time_since_last);

         if (pos_.x < -400.) pos_.x = -400.;
         if (pos_.x > 400.) pos_.x = 400.;
         if (pos_.y < -300.) pos_.y = -300.;
         if (pos_.y > 300.) pos_.y = 300.;
      }

      bool is_moving() const { return dir_flags_ != 0; }

      std::uint16_t dir_flags_;
      glm::vec2 pos_;
   };

   player_t player;

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

         player.handle_key_action(key, action);
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

      auto fbo = std::make_unique<gl::frame_buffer_t>(context.win().frame_buffer_dims());

      auto post_pass = prg_post.pass()
         .with(screen_vertices_spec)
         .set_uniform("texture", fbo->texture())
         .invoke_uniform_action("t", set_time_cb);




      class sprite_sheet {
      public:
         struct sprite_ref {
            int state;
            glm::ivec2 position;
            glm::ivec2 dimensions;
         };

         sprite_sheet(gl::texture_t texture, std::initializer_list<sprite_ref> frames)
            : texture_(texture) {
            for (auto & s : frames) {
               auto & state_info = states_[s.state];
               if (s.dimensions.x > state_info.sprite_width) state_info.sprite_width = s.dimensions.x;
               if (s.dimensions.y > state_info.sprite_height) state_info.sprite_height = s.dimensions.y;
               state_info.sprite_count++;

               state_info.frames.push_back(s);
            }
         }

         gl::texture_t texture() const { return texture_; }

         std::size_t sprite_count(int state) const { return states_.find(state)->second.sprite_count; }
         glm::ivec2 const & sprite_pos(int state, int idx) const { return states_.find(state)->second.frames.at(idx).position; }
         glm::ivec2 const & sprite_dims(int state, int idx) const { return states_.find(state)->second.frames.at(idx).dimensions; }

      private:
         gl::texture_t texture_;

         struct sprite_state_info {
            sprite_state_info() : sprite_width(0), sprite_height(0), sprite_count(0) {}

            int sprite_width;
            int sprite_height;
            std::size_t sprite_count;

            std::vector<sprite_ref> frames;
         };

         std::map<int, sprite_state_info> states_;
      };

      class sprite_t {
      public:
         sprite_t(sprite_sheet const & sprites, sprite_state_t const & sprite_state)
            : sprites_(sprites)
            , sprite_state_(sprite_state)
            , time_per_frame_(.1)
            , time_acc_(.0)
            , last_state_(sprite_state_.current_state())
            , idx_(0)
         {}

         void update(double time_since_last) {
            auto current_state = sprite_state_.current_state();
            if (current_state != last_state_) {
               utils::log(utils::LOG_INFO, "transitioned from state %d -> %d\n", last_state_, current_state);
               idx_ = 0;
               time_acc_ = time_since_last;

               last_state_ = current_state;
               return;
            }

            time_acc_ += time_since_last;
            while (time_acc_ > time_per_frame_) {
               idx_++;
               idx_ = idx_ % sprites_.sprite_count(current_state);
               time_acc_ -= time_per_frame_;
            }
         }

         int frame_count() const { return sprites_.sprite_count(sprite_state_.current_state()); }

         int idx() const { return idx_; }
         void set_idx(int idx) { idx_ = idx; }

         glm::ivec2 const & pos() const {
            return sprites_.sprite_pos(last_state_, idx());
         }

         glm::ivec2 const & dims() const {
            return sprites_.sprite_dims(last_state_, idx());
         }

      private:
         sprite_sheet const & sprites_;
         sprite_state_t const & sprite_state_;
         double time_per_frame_;
         double time_acc_;
         int last_state_;
         int idx_;
      };


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

      auto set_model = [&player](gl::uniform & u) {
         float t = (float)gl::get_time();
         float x = 400.f + player.pos_.x; // *std::sin(t);
         float y = 300.f + player.pos_.y; // *std::cos(3.f * t);
         u.set(glm::translate(glm::mat4{}, glm::vec3{ x, y, 0. }));
      };

      sprite_sheet sprites({ "../res/kenney_platformer_graphics/Player/p1_walk/p1_walk.png" }, {
         { 0, { 0, 420 }, { 66, 92 } },
         { 1, { 66, 419 }, { 66, 92 } },
         { 1, { 133, 420 }, { 66, 92 } },
         { 1, { 0, 326 }, { 66, 92 } },
         { 1, { 133, 420 }, { 66, 92 } },
         { 1, { 66, 420 }, { 66, 92 } },
      });
      sprite_t sprite(sprites,  player);

      auto set_sprite_xy = [&sprite](gl::uniform & u) { u.set(sprite.pos()); };
      auto set_sprite_wh = [&sprite](gl::uniform & u) { u.set(sprite.dims()); };

      auto sprite_tex = sprites.texture();

      auto prg_sprite = create_program("sprite");
      auto sprite_pass = prg_sprite.pass()
         .with(sprite_vertices_spec)
         .invoke_uniform_action("model", set_model)
         .set_uniform("proj", glm::ortho<float>(0., 800., 0., 600.))
         .invoke_uniform_action("sprite_xy", set_sprite_xy)
         .invoke_uniform_action("sprite_wh", set_sprite_wh)
         .set_uniform("texture_wh", glm::vec2(sprite_tex.width(), sprite_tex.height()))
         .set_uniform("texture", sprite_tex);


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

         if (should_next) {
            auto new_idx = (sprite.idx() + 1) % sprite.frame_count();
            sprite.set_idx(new_idx);
            auto const & pos = sprite.pos();

            utils::log(utils::LOG_INFO, "sprite[%d].pos == [%d,%d]\n", new_idx, (int)pos.x, (int)pos.y);

            should_next = false;
         }

         auto dims = context.win().frame_buffer_dims();

         if (!fbo || fbo->dims() != dims) {
            fbo = std::make_unique<gl::frame_buffer_t>(dims);
         }

         double this_tick = gl::get_time();
         double time_since_last_tick = this_tick - last_tick;
         sprite.update(time_since_last_tick);
         player.update(time_since_last_tick);

         glClearColor(.7f, .87f, .63f, 1.);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glViewport(0, 0, dims.x, dims.y);

         bg_pass.draw_to(*fbo, gl::DrawMode::Triangles);
         sprite_pass.draw_to(*fbo, gl::DrawMode::Triangles);
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
