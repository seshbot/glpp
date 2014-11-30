#include "game.h"

#include "utils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <random>


namespace game {

   //
   // moment_t
   //

   moment_t::moment_t()
      : state_(std::make_shared<state>()) {
   }

   moment_t::moment_t(glm::vec2 const & pos, glm::vec2 const & vel)
      : moment_t() {
      set_pos(pos);
      set_vel(vel);
   }


   glm::mat4 moment_t::transform() const {
      float x = 400.f + pos().x;
      float y = 300.f + pos().y;

      auto moved = glm::translate(glm::mat4{}, glm::vec3{ x, y, 0. });

      if (dir().x < 0) return glm::scale(moved, glm::vec3{ -1., 1., 1. });
      return moved;
   }

   void moment_t::update(double time_since_last) {
      auto & _ = *state_;
      _.pos_ += _.vel_ * static_cast<float>(time_since_last);
   }


   //
   // class world_t
   //

   world_t::world_t(entity_info_table & entity_db, player_controller_t const & player_controller)
      : entity_db_(entity_db)
      , player_controller_(player_controller)
      , player_id(create_creature(creature_t::types::person, {}))
   {
   }

   entity_id_t world_t::create_creature(creature_t::types type, moment_t const & moment) {
      auto id = entity_db_.create_row();
      entity_db_.moment(id) = moment;
      entity_db_.creature_info(id) = { type };
      return id;
   }

   void world_t::update(double time_since_last) {
      player_update(time_since_last);
      creature_think(time_since_last);
      move_creatures(time_since_last);
   }

   void world_t::creature_think(double time_since_last) {
      auto & player_moment = entity_db_.moment(player_id);

      auto & entity_ids = entity_db_.entity_ids();
      auto & creatures = entity_db_.creature_infos();
      auto & plans = entity_db_.plans();
      auto & moments = entity_db_.moments();

      for (std::size_t idx = 0; idx < creatures.size(); idx++) {
         auto id = entity_ids[idx];
         auto & c = creatures[idx];
         if (id == player_id) continue;

         auto & m = moments[idx];
         auto & p = plans[idx];

         switch (p.type) {
         case plan_t::type_t::do_nothing: {
            if (glm::length(player_moment.pos() - m.pos()) < 60.) {
               auto target = glm::vec2((float)(std::rand() % 800 - 400), (float)(std::rand() % 600 - 300));
               p = plan_t::move_to(target);
            }
         }
         break;
         case plan_t::type_t::move_to: {
            auto to_target = p.pos - m.pos();
            auto dist = glm::length(to_target);
            if (dist < 10.) {
               p = plan_t::wait_for((double)(std::rand() % 3 + 1));
            }
            else {
               m.set_dir(to_target);
               m.set_vel((to_target / dist) * 200.f);
            }
         }
         break;
         case plan_t::type_t::wait_for: {
            if (m.speed() > 0.01) m.set_vel({});
            p.time -= time_since_last;
            if (p.time < 0.) {
               auto target = glm::vec2((float)(std::rand() % 750 - 375), (float)(std::rand() % 500 - 250));
               p = plan_t::move_to(target);
            }
         }
         break;
         case plan_t::type_t::follow_for: break;
         default: break;
         }
      }
   }

   void world_t::player_update(double time_since_last) {
      auto & m = entity_db_.moment(player_id);
      auto target_vel = player_controller_.get_relative_velocity() * 500.f;
      if (glm::length(target_vel) > 0.1) {
         m.set_dir(player_controller_.get_relative_velocity());
      }
      m.set_vel(utils::lerp(m.vel(), target_vel, 7.f * static_cast<float>(time_since_last)));
   }

   void world_t::move_creatures(double time_since_last) {
      auto & moments = entity_db_.moments();

      for (auto & m : entity_db_.moments()) {
         m.update(time_since_last);

         // dont let them go outside the screen
         auto pos = m.pos();
         if (pos.x < -400.) pos.x = -400.;
         if (pos.x > 400.) pos.x = 400.;
         if (pos.y < -300.) pos.y = -300.;
         if (pos.y > 300.) pos.y = 300.;
         m.set_pos(pos);
      }
   }


   //
   // class world_view_t::render_info_t
   //

   world_view_t::render_info_t::render_info_t(gl::sprite_cursor_t const & sprite, moment_t const & moment)
      : tex_id(sprite.current_animation().texture().id()), sprite(&sprite), moment(&moment)
   { }

   world_view_t::render_info_t::render_info_t(render_info_t & other)
      : tex_id(other.tex_id), sprite(other.sprite), moment(other.moment)
   { }

   world_view_t::render_info_t & world_view_t::render_info_t::operator=(render_info_t & other) {
      if (&other == this) return *this;

      tex_id = other.tex_id;
      sprite = other.sprite;
      moment = other.moment;

      return *this;
   }


   //
   // class world_view_t
   //

   world_view_t::world_view_t(entity_info_table & entity_db, sprite_repository_t & sprite_repository)
      : entity_db_(entity_db)
      , sprite_repository_(sprite_repository)
   { }

   void world_view_t::update_sprite_cursor(double time_since_last, gl::sprite_cursor_t & cursor, moment_t & moment) {
      if (!moment.is_moving()) cursor.set_animation_idx(0);
      else cursor.set_animation_idx(1);

      cursor.advance(time_since_last);
   }

   namespace {
      bool cmp_by_tex_id(world_view_t::render_info_t const & a, world_view_t::render_info_t const & b) {
         return a.tex_id < b.tex_id;
      }
   }

   void world_view_t::update(double time_since_last) {
      auto & sprites = entity_db_.sprites();
      auto & creatures = entity_db_.creature_infos();
      auto & moments = entity_db_.moments();

      std::vector<render_info_t> render_info;

      for (std::size_t idx = 0; idx < entity_db_.size(); idx++) {
         auto & sprite_ptr = sprites[idx];

         // create and track sprites if no sprite associated with entity
         if (!sprite_ptr) {
            sprite_ptr = create_sprite(creatures[idx]);
         }

         auto & moment = moments[idx];
         update_sprite_cursor(time_since_last, *sprite_ptr, moment);

         render_info.push_back({ *sprite_ptr, moment });
      }

      std::sort(std::begin(render_info_), std::end(render_info_), cmp_by_tex_id);

      render_info_.swap(render_info);
   }

   std::unique_ptr<gl::sprite_cursor_t> world_view_t::create_sprite(creature_t const & creature) {
      return std::unique_ptr<gl::sprite_cursor_t>(
         new gl::sprite_cursor_t(
            sprite_repository_.find_creature_sprite(creature)));
   }

}


//
// class table_index_t
//

std::size_t table_index_t::size() const {
   return row_idx_to_row_id.size();
}

std::size_t table_index_t::index_of(table_row_id_type row_id) const {
   return row_id_to_row_idx[row_id];
}

table_row_id_type table_index_t::alloc_row() {
   auto alloc_row_id = [&](table_row_id_type new_row_idx) {
      for (auto id = 0U; id < row_id_to_row_idx.size(); id++) {
         if (row_id_to_row_idx[id] == -1) {
            row_id_to_row_idx[id] = new_row_idx;
            return id;
         }
      }

      row_id_to_row_idx.push_back(new_row_idx);
      return row_id_to_row_idx.size() - 1;
   };

   // new rows are always pushed to the back of the pack
   auto new_row_idx = row_idx_to_row_id.size();
   auto new_row_id = alloc_row_id(new_row_idx);
   row_idx_to_row_id.push_back(new_row_id);
   return new_row_id;
}

void table_index_t::delete_row(table_row_id_type row_id) {
   // deleted row is swapped with last row then erased from the back of the pack
   auto last_row_id = row_idx_to_row_id.back();

   auto erased_row_idx = row_id_to_row_idx[row_id];
   row_idx_to_row_id[erased_row_idx] = last_row_id;
   row_id_to_row_idx[last_row_id] = erased_row_idx;

   row_id_to_row_idx[row_id] = -1;
   row_idx_to_row_id.pop_back();
}


//
//
//
