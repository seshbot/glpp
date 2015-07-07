#ifndef GAME__H
#define GAME__H

#include <functional>
#include <map>
#include <memory>
#include <stdint.h>

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

// TODO: this should be sprite.h
//#include <glpp/glpp.h>
#include <glpp/scene.h>

#include "game_db.h"


namespace game {

   glm::vec2 random_world_location();
   glm::vec2 center_world_location();
   glm::vec2 random_direction();


   struct world_t {
   public:
      struct player_controller_t {
         virtual ~player_controller_t() {}
         virtual glm::vec2 get_relative_velocity() const = 0;
      };

      world_t(creature_info_table & entity_db, prop_info_table & prop_db, particle_info_table & particle_db, player_controller_t const & player_controller);

      entity_id_t create_creature(creature_t::types type, moment_t const & moment);
      entity_id_t create_prop(prop_t::types type, moment_t const & moment);
      entity_id_t create_particle(particle_t::types type, moment_t const & moment, double ttl);

      void update(double time_since_last);

      entity_id_t player_id() const { return player_id_; }

   private:
      void creature_think(double time_since_last);
      void player_update(double time_since_last);
      void move_creatures(double time_since_last);
      void move_particles(double time_since_last);

      creature_info_table & entity_db_;
      prop_info_table & prop_db_;
      particle_info_table & particle_db_;
      player_controller_t const & player_controller_;
      entity_id_t player_id_;
   };


   struct world_view_t {
      struct sprite_repository_t {
         virtual ~sprite_repository_t() { }
         virtual glpp::animation_timeline_t find_sprite(creature_t const & entity, moment_t & moment, plan_t const & plan) const = 0;
         virtual void animate(std::size_t db_idx, creature_t const & entity, glpp::animation_timeline_t & cursor, moment_t & moment, plan_t & plan, double time_since_last) const = 0;
         virtual glpp::animation_timeline_t find_sprite(prop_t const & entity, moment_t & moment) const = 0;
         virtual void animate(std::size_t db_idx, prop_t const & entity, glpp::animation_timeline_t & cursor, moment_t & moment, double time_since_last) const = 0;
         virtual glpp::animation_timeline_t find_sprite(particle_t const & entity, moment_t & moment) const = 0;
         virtual void animate(std::size_t db_idx, particle_t const & entity, glpp::animation_timeline_t & cursor, moment_t & moment, double time_since_last) const = 0;
      };

      struct render_info_t {
         render_info_t(glpp::animation_timeline_t const & sprite, moment_t const & moment)
            : sprite(&sprite), moment(&moment) {}
         render_info_t(render_info_t const & other) = default;
         render_info_t & operator=(render_info_t const & other) = default;

         glpp::animation_timeline_t const * sprite;
         moment_t const * moment;
      };

      world_view_t(creature_info_table & entity_db, prop_info_table & prop_db, particle_info_table & particle_db, sprite_repository_t & sprite_repository);

      void update(double time_since_last);

      using iterator = std::vector<render_info_t>::iterator;
      using const_iterator = std::vector<render_info_t>::const_iterator;

      iterator creatures_begin() { return creature_render_info_.begin(); }
      iterator creatures_end() { return creature_render_info_.end(); }

      const_iterator creatures_begin() const { return creature_render_info_.begin(); }
      const_iterator creatures_end() const { return creature_render_info_.end(); }
      unsigned creatures_count() const { return creature_render_info_.size(); }

      iterator props_begin() { return prop_render_info_.begin(); }
      iterator props_end() { return prop_render_info_.end(); }

      const_iterator props_begin() const { return prop_render_info_.begin(); }
      const_iterator props_end() const { return prop_render_info_.end(); }
      unsigned props_count() const { return prop_render_info_.size(); }

      iterator particles_begin() { return particle_render_info_.begin(); }
      iterator particles_end() { return particle_render_info_.end(); }

      const_iterator particles_begin() const { return particle_render_info_.begin(); }
      const_iterator particles_end() const { return particle_render_info_.end(); }
      unsigned particles_count() const { return particle_render_info_.size(); }

      unsigned entities_count() const { return creatures_count() + props_count() + particles_count(); }

   private:
      void update_creatures(double time_since_last);
      void update_props(double time_since_last);
      void update_particles(double time_since_last);

      std::unique_ptr<glpp::animation_timeline_t> create_sprite(creature_t const & entity, moment_t & moment, plan_t const & plan);
      std::unique_ptr<glpp::animation_timeline_t> create_sprite(prop_t const & entity, moment_t & moment);
      std::unique_ptr<glpp::animation_timeline_t> create_sprite(particle_t const & entity, moment_t & moment);

      creature_info_table & entity_db_;
      prop_info_table & prop_db_;
      particle_info_table & particle_db_;
      sprite_repository_t & sprite_repository_;
      std::vector<render_info_t> creature_render_info_;
      std::vector<render_info_t> prop_render_info_;
      std::vector<render_info_t> particle_render_info_;
   };
}

#endif // #ifndef GAME__H
