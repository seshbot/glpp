#ifndef GAME__H
#define GAME__H

#include <functional>
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
   class entity_t {
   public:
      entity_t();

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
      using entity_update_func = std::function < void(double, game::entity_t &) >;

      entity_id_t add_entity(game::entity_t e, entity_update_func update);
      game::entity_t const & entity(entity_id_t id) const;

      void update(double time_since_last);

   private:
      struct entity_controller {
         entity_controller(game::entity_t e, entity_update_func u);
         game::entity_t entity;
         entity_update_func update;
      };

      std::vector<entity_controller> entity_controllers_;
   };

   class sprites_t{
   public:
      sprites_t(entities_t const & entities) : entities_(entities) {}

      using sprite_update_func = std::function < void(double, game::entity_t const &, gl::sprite_t &) >;
      using sprite_iterate_func = std::function < void(game::entity_t const &, gl::sprite_t const &) >;

      void add_sprite(entity_id_t entity_id, gl::sprite_t sprite, sprite_update_func update);
      gl::sprite_t const & entity_sprite(entity_id_t entity_id) const {
         for (auto & c : sprite_controllers_) if (c.entity_id == entity_id) return c.sprite;
         throw std::runtime_error((std::string("sprite not found for entity ") + std::to_string(entity_id)).c_str());
      }
      void for_each_sprite(sprite_iterate_func func);

      void update(double time_since_last);

   private:
      game::entity_t const & entity(entity_id_t id) const;

      struct sprite_controller {
         sprite_controller(entity_id_t e_id, gl::sprite_t s, sprite_update_func u);
         sprite_controller(sprite_controller && o);
         sprite_controller & operator=(sprite_controller && o);

         entity_id_t entity_id;
         gl::sprite_t sprite;
         sprite_update_func update;

      private:
         sprite_controller(sprite_controller &) = delete;
         sprite_controller & operator=(sprite_controller &) = delete;
      };

      entities_t const & entities_;
      std::vector<sprite_controller> sprite_controllers_;
   };

}

#endif // #ifndef GAME__H
