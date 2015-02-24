#include "game.h"

#include <glpp/utils.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <algorithm>
#include <random>

namespace {
   const float VIEW_DIMENSION_WIDTH = 800.f;
   const float VIEW_DIMENSION_HEIGHT = 600.f;
   const float VIEW_DIMENSION_DEPTH = 2.f; // -1 - 1
   const float WORLD_MIN_Y = -VIEW_DIMENSION_HEIGHT / 2.f;
   const float WORLD_MAX_Y = VIEW_DIMENSION_HEIGHT / 2.f * 1.414213562373f;
   const float WORLD_MIN_X = -VIEW_DIMENSION_WIDTH / 2.f;
   const float WORLD_MAX_X = VIEW_DIMENSION_WIDTH / 2.f;

   const float CREATURE_SPEED_PER_SECOND = 150.f;
}

namespace game {

   glm::vec2 random_world_location() {
      const auto WORLD_SPAN_X = (int)(WORLD_MAX_X - WORLD_MIN_X);
      const auto WORLD_SPAN_Y = (int)(WORLD_MAX_Y - WORLD_MIN_Y);
      return{
         (float)(std::rand() % WORLD_SPAN_X + WORLD_MIN_X),
         (float)(std::rand() % WORLD_SPAN_Y + WORLD_MIN_Y)
      };
   }

   //
   // moment_t
   //

   moment_t::moment_t()
      : dir_(0., 1.) {
   }

   moment_t::moment_t(glm::vec2 const & pos, glm::vec2 const & vel)
      : moment_t() {
      set_pos(pos);
      set_vel(vel);
   }


   glm::mat4 moment_t::sprite_transform() const {
      float x = (VIEW_DIMENSION_WIDTH / 2.f) + pos().x;  // 0 - 800
      float y = (VIEW_DIMENSION_HEIGHT / 2.f) + pos().y; // 0 - 600
      float z = (y / VIEW_DIMENSION_HEIGHT) * -1.f; // -1 - 0
      auto moved = glm::translate(glm::mat4{}, glm::vec3{ x, y, z });

      // flip the sprite if we are heading right-to-left
      if (dir().x < 0) return glm::scale(moved, glm::vec3{ -1., 1., 1. });
      return moved;
   }

   glm::mat4 moment_t::mesh_transform() const {
      float x = (VIEW_DIMENSION_WIDTH / 2.f) + pos().x;  // 0 - 800
      float z = -((VIEW_DIMENSION_HEIGHT / 2.f) + pos().y); // 0 - 600
      auto moved = glm::translate(glm::mat4{}, glm::vec3{ x, 0., z });

      return
         glm::rotate(
//            glm::rotate(
               glm::scale(moved, glm::vec3{ 32. }),
//               glm::radians(-45.f), glm::vec3{ 1., 0., 0. }),
            angle(), glm::vec3{ 0., 1., 0. });
   }

   glm::vec2::value_type moment_t::angle() const {
      auto abs_angle = glm::angle(glm::vec2{ 0., -1 }, dir_);
      return (dir_.x >= 0)
         ? abs_angle
         : glm::radians(360.f) - abs_angle;
   }

   void moment_t::update(double time_since_last) {
      pos_ += vel_ * static_cast<float>(time_since_last);
   }


   //
   // class world_t
   //

   world_t::world_t(creature_info_table & entity_db, particle_info_table & particle_db, player_controller_t const & player_controller)
      : entity_db_(entity_db)
      , particle_db_(particle_db)
      , player_controller_(player_controller)
      , player_id_(create_creature(creature_t::types::person, {}))
   {
   }

   entity_id_t world_t::create_creature(creature_t::types type, moment_t const & moment) {
      auto id = entity_db_.create_row();
      entity_db_.moment(id) = moment;
      entity_db_.creature_info(id) = { type };
      return id;
   }

   entity_id_t world_t::create_particle(particle_t::types type, moment_t const & moment, double ttl) {
      auto id = particle_db_.create_row();
      particle_db_.moment(id) = moment;
      particle_db_.particle_info(id) = { type, ttl };
      return id;
   }

   void world_t::update(double time_since_last) {
      player_update(time_since_last);
      creature_think(time_since_last);
      move_creatures(time_since_last);
      move_particles(time_since_last);
   }

   void world_t::creature_think(double time_since_last) {
      auto & player_moment = entity_db_.moment(player_id_);

      auto & entity_ids = entity_db_.entity_ids();
      auto & creatures = entity_db_.creature_infos();
      auto & plans = entity_db_.plans();
      auto & moments = entity_db_.moments();

      for (std::size_t idx = 0; idx < creatures.size(); idx++) {
         auto id = entity_ids[idx];
         auto & c = creatures[idx];
         if (id == player_id_) continue;

         auto & m = moments[idx];
         auto & p = plans[idx];

         switch (p.type) {
         case plan_t::type_t::walk_on_spot: {
         }
         break;
         case plan_t::type_t::do_nothing: {
            if (glm::length(player_moment.pos() - m.pos()) < 60.) {
               p = plan_t::move_to(random_world_location());
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
               m.set_vel((to_target / dist) * CREATURE_SPEED_PER_SECOND);
            }
         }
         break;
         case plan_t::type_t::wait_for: {
            if (m.speed() > 0.01) m.set_vel({});
            p.time -= time_since_last;
            if (p.time < 0.) {
               p = plan_t::move_to(random_world_location());
            }
         }
         break;
         case plan_t::type_t::follow_for: break;
         default: break;
         }
      }
   }

   void world_t::player_update(double time_since_last) {
      auto & m = entity_db_.moment(player_id_);
      auto target_vel = player_controller_.get_relative_velocity() * CREATURE_SPEED_PER_SECOND;
      if (glm::length(target_vel) > 0.1) {
         m.set_dir(player_controller_.get_relative_velocity());
      }
      m.set_vel(utils::lerp(m.vel(), target_vel, 12.f * static_cast<float>(time_since_last)));
   }

   void world_t::move_creatures(double time_since_last) {
      auto & moments = entity_db_.moments();

      for (auto & m : moments) {
         m.update(time_since_last);

         // dont let them go outside the screen
         auto pos = m.pos();
         if (pos.x < WORLD_MIN_X) pos.x = WORLD_MIN_X;
         if (pos.x > WORLD_MAX_X) pos.x = WORLD_MAX_X;
         if (pos.y < WORLD_MIN_Y) pos.y = WORLD_MIN_Y;
         if (pos.y > WORLD_MAX_Y) pos.y = WORLD_MAX_Y;
         m.set_pos(pos);
      }

      // TODO: do not recycle rows within a single cycle!!! otherwise the sprite view wont detect the deletion
   }

   void world_t::move_particles(double time_since_last) {
      auto & ids = particle_db_.ids();
      auto & particles = particle_db_.particle_infos();
      auto & moments = particle_db_.moments();

      auto particle_count = particles.size();
      for (auto idx = 0U; idx < particle_count; idx++) {
         auto & p = particles[idx];
         p.time_remaining -= time_since_last;
         if (p.time_remaining <= 0.) {
            auto id = ids[idx];
            particle_db_.delete_row(id);
            particle_count--;
            idx--; // redo the current row
            continue;
         }

         auto & m = moments[idx];
         m.update(time_since_last);
      }
   }


   //
   // class world_view_t::render_info_t
   //

   world_view_t::render_info_t::render_info_t(glpp::sprite_cursor_t const & sprite, moment_t const & moment)
      : tex_id(sprite.current_animation().texture().id()), sprite(&sprite), moment(&moment)
   { }

   world_view_t::render_info_t::render_info_t(render_info_t const & other)
      : tex_id(other.tex_id), sprite(other.sprite), moment(other.moment)
   { }

   world_view_t::render_info_t & world_view_t::render_info_t::operator=(render_info_t const & other) {
      if (&other == this) return *this;

      tex_id = other.tex_id;
      sprite = other.sprite;
      moment = other.moment;

      return *this;
   }


   //
   // class world_view_t
   //

   world_view_t::world_view_t(creature_info_table & entity_db, particle_info_table & particle_db, sprite_repository_t & sprite_repository)
      : entity_db_(entity_db)
      , particle_db_(particle_db)
      , sprite_repository_(sprite_repository)
   { }

   namespace {
      bool cmp_by_tex_id(world_view_t::render_info_t const & a, world_view_t::render_info_t const & b) {
         if (a.tex_id != b.tex_id) return a.tex_id < b.tex_id;
         return a.moment->pos().y > b.moment->pos().y;
      }
   }

   void world_view_t::update(double time_since_last) {
      update_creatures(time_since_last);
      update_particles(time_since_last);
   }

   void world_view_t::update_creatures(double time_since_last) {
      auto & sprites = entity_db_.sprites();
      auto & creatures = entity_db_.creature_infos();
      auto & moments = entity_db_.moments();

      std::vector<render_info_t> creature_render_info;

      for (std::size_t idx = 0; idx < entity_db_.size(); idx++) {
         auto & sprite_ptr = sprites[idx];
         auto & creature = creatures[idx];

         // create and track sprites if no sprite associated with entity
         if (!sprite_ptr) {
            sprite_ptr = create_sprite(creature);
         }

         auto & moment = moments[idx];
         auto animation_speed = sprite_repository_.creature_sprite_updating(idx, creature, *sprite_ptr, moment);
         sprite_ptr->advance(animation_speed * time_since_last);

         creature_render_info.push_back({ {*sprite_ptr}, moment });
      }

      std::sort(std::begin(creature_render_info), std::end(creature_render_info), cmp_by_tex_id);

      creature_render_info_.swap(creature_render_info);
   }

   void world_view_t::update_particles(double time_since_last) {
      auto & sprites = particle_db_.sprites();
      auto & particles = particle_db_.particle_infos();
      auto & moments = particle_db_.moments();

      std::vector<render_info_t> particle_render_info;

      for (std::size_t idx = 0; idx < particle_db_.size(); idx++) {
         auto & sprite_ptr = sprites[idx];

         // create and track sprites if no sprite associated with entity
         if (!sprite_ptr) {
            sprite_ptr = create_sprite(particles[idx]);
         }

         auto & moment = moments[idx];
         sprite_ptr->advance(time_since_last);

         particle_render_info.push_back({ {*sprite_ptr}, moment });
      }

      std::sort(std::begin(particle_render_info), std::end(particle_render_info), cmp_by_tex_id);

      particle_render_info_.swap(particle_render_info);
   }

   std::unique_ptr<glpp::sprite_cursor_t> world_view_t::create_sprite(creature_t const & creature) {
      return std::unique_ptr<glpp::sprite_cursor_t>(
         new glpp::sprite_cursor_t(
            sprite_repository_.find_creature_sprite(creature)));
   }

   std::unique_ptr<glpp::sprite_cursor_t> world_view_t::create_sprite(particle_t const & particle) {
      return std::unique_ptr<glpp::sprite_cursor_t>(
         new glpp::sprite_cursor_t(
         sprite_repository_.find_particle_sprite(particle)));
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
   auto alloc_row_id = [&](table_row_id_type new_row_idx) -> table_row_id_type{
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