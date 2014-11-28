#ifndef GAME__H
#define GAME__H

#include <functional>
#include <map>
#include <memory>
#include <stdint.h>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>

// TODO: this should be sprite.h
#include "gl.h"


namespace game {
   using entity_id_t = std::size_t;

   // holds geometric (pos, dir) info of a game entity
   class entity_moment_t {
   public:
      entity_moment_t();
      entity_moment_t(glm::vec2 const & pos, glm::vec2 const & vel);

      glm::mat4 transform() const;

      glm::vec2 const & dir() const { return state_->dir_; }
      glm::vec2 const & pos() const { return state_->pos_; }
      glm::vec2 const & vel() const { return state_->vel_; }

      bool is_moving() const { return glm::length(vel()) > 0.1; }

      void set_dir(glm::vec2 const & dir) {
         if (glm::length(dir) < 0.1)
            return;
         auto norm = glm::normalize(dir);
         state_->dir_ = norm;
      }
      void set_pos(glm::vec2 const & pos) { state_->pos_ = pos; }
      void set_vel(glm::vec2 const & vel) { state_->vel_ = vel; }

      void update(double time_since_last);

   private:
      struct state {
         state() : dir_(0., 1.) {}

         glm::vec2 pos_;
         glm::vec2 dir_;
         glm::vec2 vel_;

      };

      std::shared_ptr<state> state_;
   };

   // tracks entities and their sprites
   class entities_t {
   public:
      struct controller {
         virtual ~controller() {}
         virtual void update(double time_since_last, entities_t & entities, entity_id_t eid, entity_moment_t & entity) = 0;
      };

      static std::unique_ptr<controller> simple_controller();

      ~entities_t();

      void register_controller(std::string name, std::unique_ptr<controller> controller);
      entity_id_t create_entity(entity_moment_t e, std::string controller_name);
      void destroy_entity(entity_id_t eid);

      bool entity_exists(entity_id_t eid) const;
      entity_moment_t const & entity(entity_id_t eid) const;

      void update(double time_since_last);

   private:
      std::map<std::string, controller*> controllers_;
      struct entity_info {
         bool alive;
         entity_moment_t moment;
         controller * controller;
      };
      std::vector<entity_info> entities_;
      long first_replace_loc_ = -1;
   };

   // this ensures all sprite cursors 1represent the current time and state of the attached entity
   class entity_sprites_t {
   public:
      struct controller {
         virtual ~controller() {}
         virtual void update(double time_since_last, entity_moment_t const & entity, gl::sprite_cursor_t & sprite) = 0;
      };

      static std::unique_ptr<controller> simple_controller();

      entity_sprites_t(entities_t const & entities);
      ~entity_sprites_t();

      void register_controller(std::string name, std::unique_ptr<controller> controller);
      void register_entity_sprite(entity_id_t entity_id, gl::sprite_t sprite, std::string controller_name);
      void destroy_entity_sprites(entity_id_t entity_id);

      void update(double time_since_last);

      gl::sprite_cursor_t const & entity_sprite(entity_id_t entity_id) const;
      std::size_t count() const;
      entity_moment_t const & entity_at(std::size_t idx) const;
      gl::sprite_cursor_t const & sprite_at(std::size_t idx) const;

   private:
      void destroy_at_idx(std::size_t idx);

      entity_moment_t const & entity(entity_id_t id) const;

      entities_t const & entities_;
      std::map<std::string, controller*> controllers_;
      struct entity_sprite_state { entity_id_t eid; gl::sprite_cursor_t curs; controller * cont; };
      std::vector<entity_sprite_state> entity_sprites_;
   };
}

#endif // #ifndef GAME__H
