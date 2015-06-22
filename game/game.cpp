#include "game.h"

#include <glpp/utils.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <random>

namespace {
   // world dimensions go from 0 - 800, 0 - 600

   // 3d world space goes from:
   //  x: 0 - 800
   //  y: 0 (altitude)
   //  z: 0 - -600 (north is -z)

   const float VIEW_VISIBLE_HEIGHT_SCALING = 1.414213562373f;
   const float VIEW_DIMENSION_WIDTH = 2.f * 800.f;
   const float VIEW_DIMENSION_HEIGHT = 2.f * 600.f;
   const float WORLD_MIN_Y = -.5f * VIEW_DIMENSION_HEIGHT * VIEW_VISIBLE_HEIGHT_SCALING;
   const float WORLD_MAX_Y =  .5f * VIEW_DIMENSION_HEIGHT * VIEW_VISIBLE_HEIGHT_SCALING;
   const float WORLD_MIN_X = -.5f * VIEW_DIMENSION_WIDTH;
   const float WORLD_MAX_X =  .5f * VIEW_DIMENSION_WIDTH;

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

   // center is 400, 300
   glm::vec2 center_world_location() {
      const auto WORLD_SPAN_X = (int)(WORLD_MAX_X - WORLD_MIN_X);
      const auto WORLD_SPAN_Y = (int)(WORLD_MAX_Y - WORLD_MIN_Y);
      return{
         (float)(WORLD_SPAN_X / 2. + WORLD_MIN_X),
         (float)(WORLD_SPAN_Y / 2. + WORLD_MIN_Y)
      };
   }

   glm::vec2 random_direction() {
      static const float PI = 3.141592653589793238462643383f;
      static const float TAU = 2 * PI;
      static const unsigned PRECISION = 10000;
      static const unsigned TAU_10000 = static_cast<unsigned>(TAU * PRECISION);
      float rotation = (float)(std::rand() % TAU_10000) / PRECISION;
      return glm::rotate(glm::vec2{ 1., 0. }, rotation);
   }

   //
   // moment_t
   //

   moment_t::moment_t()
      : dir_(0., 1.)
      , scale_(32.) {
   }

   moment_t::moment_t(glm::vec2 const & pos, glm::vec2 const & vel)
      : moment_t() {
      set_pos(pos);
      set_vel(vel);
      set_dir(vel);
   }

   moment_t::moment_t(glm::vec2 const & pos, glm::vec2 const & vel, glm::vec2 const & dir)
      : moment_t(pos, vel) {
      set_dir(dir);
   }

   moment_t::moment_t(glm::vec2 const & pos, glm::vec2 const & vel, glm::vec2 const & dir, float scale)
      : moment_t(pos, vel, dir) {
      auto factor = static_cast<float>(32. * scale);
      scale_ = glm::vec3{ factor };
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
      float x = pos().x;  // 0 - 800
      float z = -pos().y; // 0 - -600

      auto scale = glm::scale(scale_);
      auto rot = glm::rotate(angle(), glm::vec3{ 0., 1., 0. });
      auto trans = glm::translate(glm::vec3{ x, 0., z });

      auto result = trans * rot * scale;
      return result;

      //auto result = glm::mat4{ 1. };
      //result = glm::translate(result, glm::vec3{ x, 0., z }); // move
      //result = glm::rotate(result, angle(), glm::vec3{ 0., 1., 0. }); // rotate
      //result = glm::scale(result, glm::vec3{ 32. });          // scale
      //return result;
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

   world_t::world_t(
      creature_info_table & entity_db,
      prop_info_table & prop_db,
      particle_info_table & particle_db,
      player_controller_t const & player_controller)
      : entity_db_(entity_db)
      , prop_db_(prop_db)
      , particle_db_(particle_db)
      , player_controller_(player_controller)
      , player_id_(create_creature(creature_t::types::person, { center_world_location(), {} }))
   {
   }

   entity_id_t world_t::create_creature(creature_t::types type, moment_t const & moment) {
      auto id = entity_db_.create_row();
      entity_db_.moment(id) = moment;
      entity_db_.creature_info(id) = { type };
      return id;
   }

   entity_id_t world_t::create_prop(prop_t::types type, moment_t const & moment) {
      auto id = prop_db_.create_row();
      prop_db_.moment(id) = moment;
      prop_db_.prop_info(id) = { type };
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
   // class world_view_t
   //

   world_view_t::world_view_t(creature_info_table & entity_db, prop_info_table & prop_db, particle_info_table & particle_db, sprite_repository_t & sprite_repository)
      : entity_db_(entity_db)
      , prop_db_(prop_db)
      , particle_db_(particle_db)
      , sprite_repository_(sprite_repository)
   { }

   namespace {
      //bool cmp_by_tex_id(world_view_t::render_info_t const & a, world_view_t::render_info_t const & b) {
      //   if (a.tex_id != b.tex_id) return a.tex_id < b.tex_id;
      //   return a.moment->pos().y > b.moment->pos().y;
      //}

      bool cmp_by_animation(world_view_t::render_info_t const & a, world_view_t::render_info_t const & b) {
         return a.sprite->animation() < b.sprite->animation();
      }
   }

   void world_view_t::update(double time_since_last) {
      update_creatures(time_since_last);
      update_props(time_since_last);
      update_particles(time_since_last);
   }

   void world_view_t::update_creatures(double time_since_last) {
      auto & sprites = entity_db_.sprites();
      auto & entities = entity_db_.creature_infos();
      auto & moments = entity_db_.moments();
      auto & plans = entity_db_.plans();

      std::vector<render_info_t> updated_render_info;

      for (std::size_t idx = 0; idx < entity_db_.size(); idx++) {
         auto & sprite_ptr = sprites[idx];
         auto & entity = entities[idx];
         auto & moment = moments[idx];
         auto & plan = plans[idx];

         // create and track sprites if no sprite associated with entity
         if (!sprite_ptr) {
            sprite_ptr = create_sprite(entity, moment, plan);
         }

         auto & sprite = *sprite_ptr;
         sprite_repository_.animate(idx, entity, sprite, moment, plan, time_since_last);

         updated_render_info.push_back({ {*sprite_ptr}, moment });
      }

      std::sort(std::begin(updated_render_info), std::end(updated_render_info), cmp_by_animation);

      creature_render_info_.swap(updated_render_info);
   }

   void world_view_t::update_props(double time_since_last) {
      auto & sprites = prop_db_.sprites();
      auto & entities = prop_db_.prop_infos();
      auto & moments = prop_db_.moments();

      std::vector<render_info_t> updated_render_info;

      for (std::size_t idx = 0; idx < prop_db_.size(); idx++) {
         auto & sprite_ptr = sprites[idx];
         auto & entity = entities[idx];
         auto & moment = moments[idx];

         // create and track sprites if no sprite associated with entity
         if (!sprite_ptr) {
            sprite_ptr = create_sprite(entity, moment);
         }

         auto & sprite = *sprite_ptr;
         sprite_repository_.animate(idx, entity, sprite, moment, time_since_last);

         updated_render_info.push_back({ { *sprite_ptr }, moment });
      }

      std::sort(std::begin(updated_render_info), std::end(updated_render_info), cmp_by_animation);

      prop_render_info_.swap(updated_render_info);
   }

   void world_view_t::update_particles(double time_since_last) {
      auto & sprites = particle_db_.sprites();
      auto & entities = particle_db_.particle_infos();
      auto & moments = particle_db_.moments();

      std::vector<render_info_t> updated_render_info;

      for (std::size_t idx = 0; idx < particle_db_.size(); idx++) {
         auto & sprite_ptr = sprites[idx];
         auto & entity = entities[idx];
         auto & moment = moments[idx];

         // create and track sprites if no sprite associated with entity
         if (!sprite_ptr) {
            sprite_ptr = create_sprite(entity, moment);
         }

         auto & sprite = *sprite_ptr;
         sprite_repository_.animate(idx, entity, sprite, moment, time_since_last);

         updated_render_info.push_back({ {*sprite_ptr}, moment });
      }

      //std::sort(std::begin(particle_render_info), std::end(particle_render_info), cmp_by_tex_id);

      particle_render_info_.swap(updated_render_info);
   }

   std::unique_ptr<glpp::animation_timeline_t> world_view_t::create_sprite(creature_t const & entity, moment_t & moment, plan_t const & plan) {
      return std::unique_ptr<glpp::animation_timeline_t>(
         new glpp::animation_timeline_t(
            sprite_repository_.find_sprite(entity, moment, plan)));
   }

   std::unique_ptr<glpp::animation_timeline_t> world_view_t::create_sprite(prop_t const & entity, moment_t & moment) {
      return std::unique_ptr<glpp::animation_timeline_t>(
         new glpp::animation_timeline_t(
            sprite_repository_.find_sprite(entity, moment)));
   }

   std::unique_ptr<glpp::animation_timeline_t> world_view_t::create_sprite(particle_t const & entity, moment_t & moment) {
      return std::unique_ptr<glpp::animation_timeline_t>(
         new glpp::animation_timeline_t(
         sprite_repository_.find_sprite(entity, moment)));
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
