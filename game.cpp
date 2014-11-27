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
   //
   //


   game_t::id_t game_t::add_entity(entity_t e, entity_update_func update) {
      entity_controllers_.emplace_back(std::move(e), update);
      return entity_controllers_.size() - 1;
   }

   void game_t::add_sprite(id_t entity_id, gl::sprite_t sprite, sprite_update_func update) {
      sprite_controllers_.emplace_back(entity_id, std::move(sprite), update);
   }

   game::entity_t const & game_t::entity(id_t id) const { return entity_controllers_.at(id).entity; }

   void game_t::update(double time_since_last) {
      for (auto & c : entity_controllers_) { c.update(time_since_last, c.entity); }
      for (auto & c : sprite_controllers_) { c.update(time_since_last, entity(c.entity_id), c.sprite); }
   }

   void game_t::for_each_sprite(sprite_iterate_func func) {
      for (auto & c : sprite_controllers_) func(entity(c.entity_id), c.sprite);
   }

   game_t::entity_controller::entity_controller(entity_t e, entity_update_func u)
      : entity(std::move(e)), update(u) {}

   game_t::sprite_controller::sprite_controller(id_t e_id, gl::sprite_t s, sprite_update_func u)
      : entity_id(e_id), sprite(std::move(s)), update(u) {
   }

   game_t::sprite_controller::sprite_controller(sprite_controller && o)
      : sprite(std::move(o.sprite))
      , update(o.update) {
   }

   game_t::sprite_controller & game_t::sprite_controller::operator=(game_t::sprite_controller && o) {
      sprite = std::move(o.sprite);
      update = std::move(o.update);
      return *this;
   }
}
