#include "game.h"

#include <glm/gtc/matrix_transform.hpp>

namespace game {

   //
   // entity_moment_t
   //

   entity_moment_t::entity_moment_t()
      : state_(std::make_shared<state>()) {
   }

   entity_moment_t::entity_moment_t(glm::vec2 const & pos, glm::vec2 const & vel)
      : entity_moment_t() {
      set_pos(pos);
      set_vel(vel);
   }


   glm::mat4 entity_moment_t::transform() const {
      float x = 400.f + pos().x;
      float y = 300.f + pos().y;

      auto moved = glm::translate(glm::mat4{}, glm::vec3{ x, y, 0. });

      if (dir().x < 0) return glm::scale(moved, glm::vec3{ -1., 1., 1. });
      return moved;
   }

   void entity_moment_t::update(double time_since_last) {
      auto & _ = *state_;
      _.pos_ += _.vel_;
   }


   //
   // entities_t
   //


   std::unique_ptr<entities_t::controller> entities_t::simple_controller() {
      struct simple : public controller {
         virtual void update(double time_since_last, entities_t & entities, game::entity_id_t eid, entity_moment_t & entity) {
            entity.update(time_since_last);
         }
      };

      return std::unique_ptr<simple>(new simple);
   }

   entities_t::~entities_t() {
      for (auto & p : controllers_)
         delete p.second;
   }

   void entities_t::register_controller(std::string name, std::unique_ptr<controller> controller) {
      controllers_[name] = controller.release();
   }

   entity_id_t entities_t::create_entity(entity_moment_t e, std::string controller_name) {
      entity_info new_entity = { true, std::move(e), controllers_[controller_name] };

      if (first_replace_loc_ != -1) {
         auto new_id = first_replace_loc_;
         entities_[first_replace_loc_] = std::move(new_entity);

         // update first_replace_loc_ to next dead location, or -1 if none exist
         while ((std::size_t)first_replace_loc_ < entities_.size()) {
            if (!entities_[first_replace_loc_].alive) break;
            first_replace_loc_++;
         }
         if (first_replace_loc_ == entities_.size()) first_replace_loc_ = -1;

         return new_id;
      }

      entities_.push_back(std::move(new_entity));
      return entities_.size() - 1;
   }

   void entities_t::destroy_entity(entity_id_t eid) {
      if (!entities_.at(eid).alive) {
         assert(false && "trying to destroy a dead entity");
         return;
      }
      entities_.at(eid).alive = false;

      if (first_replace_loc_ == -1 || eid < (std::size_t)first_replace_loc_) {
         first_replace_loc_ = eid;
      }
   }

   bool entities_t::entity_exists(entity_id_t eid) const {
      return entities_.at(eid).alive;
   }

   entity_moment_t const & entities_t::entity(entity_id_t eid) const {
      auto & e = entities_.at(eid);
      if (!e.alive) {
         assert(false && "accessing dead entity");
         throw std::runtime_error(std::string("accessing dead entity") + std::to_string(eid));
      }
      return e.moment;
   }

   void entities_t::update(double time_since_last) {
      for (auto eid = 0U; eid < entities_.size(); eid++) {
         auto & p = entities_[eid];
         if (!p.alive) continue;
         p.controller->update(time_since_last, *this, eid, p.moment);
      }
   }


   //
   // entity_sprites_t
   //

   std::unique_ptr<entity_sprites_t::controller> entity_sprites_t::simple_controller() {
      struct simple : public controller {
         virtual void update(double time_since_last, entity_moment_t const & entity, gl::sprite_cursor_t & sprite) {
            sprite.advance(time_since_last);
         }
      };

      return std::unique_ptr<simple>(new simple);
   }

   entity_sprites_t::entity_sprites_t(entities_t const & entities)
      : entities_(entities) {
   }

   entity_sprites_t::~entity_sprites_t()
   {
      for (auto & p : controllers_)
         delete p.second;
   }

   void entity_sprites_t::register_controller(std::string name, std::unique_ptr<controller> controller) {
      controllers_[name] = controller.release();
   }

   void entity_sprites_t::register_entity_sprite(entity_id_t entity_id, gl::sprite_t sprite, std::string controller_name) {
      entity_sprites_.push_back({ entity_id, { sprite }, controllers_[controller_name] });
   }

   void entity_sprites_t::destroy_entity_sprites(entity_id_t entity_id) {
      auto idx = 0U;
      while (idx < entity_sprites_.size()) {
         if (entity_sprites_[idx].eid == entity_id) {
            destroy_at_idx(idx);
            continue;
         }
         idx++;
      }
   }

   void entity_sprites_t::destroy_at_idx(std::size_t idx) {
      if (entity_sprites_.size() <= 1) {
         assert(idx == 0 && "attempting to destroy an index outside the range of the container");
         entity_sprites_.clear();
         return;
      }

      std::iter_swap(entity_sprites_.begin() + idx, entity_sprites_.end() - 1);
      entity_sprites_.erase(entity_sprites_.end() - 1);
   }


   void entity_sprites_t::update(double time_since_last) {
      std::vector<entity_id_t> entity_sprites_to_destroy;
      for (auto & s : entity_sprites_) {
         if (!entities_.entity_exists(s.eid)) {
            entity_sprites_to_destroy.push_back(s.eid);
            continue;
         }
         auto & controller = *s.cont;
         auto & e = entity(s.eid);
         auto & sprite_cursor = s.curs;
         controller.update(time_since_last, e, sprite_cursor);
      }

      for (auto eid : entity_sprites_to_destroy)
         destroy_entity_sprites(eid);
   }

   gl::sprite_cursor_t const & entity_sprites_t::entity_sprite(entity_id_t entity_id) const {
      for (auto & p : entity_sprites_) {
         if (p.eid == entity_id) return p.curs;
      }

      throw std::runtime_error("sprite not found for entity");
   }

   std::size_t entity_sprites_t::count() const {
      return entity_sprites_.size();
   }

   entity_moment_t const & entity_sprites_t::entity_at(std::size_t idx) const {
      return entity(entity_sprites_.at(idx).eid);
   }

   gl::sprite_cursor_t const & entity_sprites_t::sprite_at(std::size_t idx) const {
      return entity_sprites_.at(idx).curs;
   }

   entity_moment_t const & entity_sprites_t::entity(entity_id_t id) const {
      return entities_.entity(id);
   }

}
