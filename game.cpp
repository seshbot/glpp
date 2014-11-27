#include "game.h"

#include <glm/gtc/matrix_transform.hpp>

namespace game {

   //
   // entity_t
   //

   entity_t::entity_t()
      : state_(std::make_shared<state>()) {
   }

   glm::mat4 entity_t::transform() const {
      float x = 400.f + pos().x;
      float y = 300.f + pos().y;

      auto moved = glm::translate(glm::mat4{}, glm::vec3{ x, y, 0. });

      if (dir().x < 0) return glm::scale(moved, glm::vec3{ -1., 1., 1. });
      return moved;
   }

   void entity_t::update(double time_since_last) {
      auto & _ = *state_;
      _.pos_ += _.vel_;

      if (_.pos_.x < -400.) _.pos_.x = -400.;
      if (_.pos_.x > 400.) _.pos_.x = 400.;
      if (_.pos_.y < -300.) _.pos_.y = -300.;
      if (_.pos_.y > 300.) _.pos_.y = 300.;
   }


   //
   // entities_t
   //

   entity_id_t entities_t::add_entity(entity_t e, entity_update_func update) {
      entity_controllers_.emplace_back(std::move(e), update);
      return entity_controllers_.size() - 1;
   }

   entity_t const & entities_t::entity(entity_id_t id) const { return entity_controllers_.at(id).entity; }

   void entities_t::update(double time_since_last) {
      for (auto & c : entity_controllers_) {
         c.update(time_since_last, c.entity);
      }
   }

   entities_t::entity_controller::entity_controller(entity_t e, entity_update_func u)
      : entity(std::move(e)), update(u) {
   }


   //
   // sprites_t
   //

   void sprites_t::add_sprite(entity_id_t entity_id, gl::sprite_t sprite, sprite_update_func update) {
      sprite_controllers_.emplace_back(entity_id, std::move(sprite), update);
   }

   gl::sprite_t const & sprites_t::entity_sprite(entity_id_t entity_id) const {
      for (auto & c : sprite_controllers_) if (c.entity_id == entity_id) return c.sprite;
      throw std::runtime_error((std::string("sprite not found for entity ") + std::to_string(entity_id)).c_str());
   }

   void sprites_t::update(double time_since_last) {
      for (auto & c : sprite_controllers_) {
         c.update(time_since_last, entity(c.entity_id), c.sprite);
      }
   }

   entity_t const & sprites_t::entity(entity_id_t id) const {
      return entities_.entity(id);
   }

   sprites_t::sprite_controller::sprite_controller(entity_id_t e_id, gl::sprite_t s, sprite_update_func u)
      : entity_id(e_id), sprite(std::move(s)), update(u) {
   }

   sprites_t::sprite_controller::sprite_controller(sprite_controller && o)
      : sprite(std::move(o.sprite))
      , update(o.update) {
   }

   sprites_t::sprite_controller & sprites_t::sprite_controller::operator=(sprites_t::sprite_controller && o) {
      sprite = std::move(o.sprite);
      update = std::move(o.update);
      return *this;
   }
}
