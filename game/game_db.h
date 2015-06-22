#ifndef GAME_DB__H
#define GAME_DB__H

#include "db.h"
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace game {
   using entity_id_t = int;

   struct creature_t {
      enum types {
         person,
         monster,
      };

      types type;
   };

   struct prop_t {
      enum types {
         tree,
         rock,
         campfire,
         torch,
      };

      types type;
   };

   struct particle_t {
      enum types {
         bullet
      };

      types type;
      double time_remaining;
   };

   // holds geometric (pos, dir) info of a game entity
   // TODO: remove instance info (scale)
   class moment_t {
   public:
      moment_t();
      moment_t(glm::vec2 const & pos, glm::vec2 const & vel);
      moment_t(glm::vec2 const & pos, glm::vec2 const & vel, glm::vec2 const & dir);
      moment_t(glm::vec2 const & pos, glm::vec2 const & vel, glm::vec2 const & dir, float scale);

      glm::mat4 sprite_transform() const;
      glm::mat4 mesh_transform() const;

      glm::vec2::value_type angle() const;
      glm::vec2 const & dir() const { return dir_; }
      glm::vec2 const & pos() const { return pos_; }
      glm::vec2 const & vel() const { return vel_; }

      bool is_moving() const { return speed_ > 0.1; }
      float speed() const { return speed_; }

      void set_dir(glm::vec2 const & dir) {
         if (glm::length(dir) < 0.1)
            return;
         auto norm = glm::normalize(dir);
         dir_ = norm;
      }
      void set_pos(glm::vec2 const & pos) { pos_ = pos; }
      void set_vel(glm::vec2 const & vel) { vel_ = vel; speed_ = glm::length(vel); }
      void rotate(float rads) { set_dir(glm::rotate(dir(), rads)); }

      void update(double time_since_last);

   private:
      glm::vec2 pos_;
      glm::vec2 dir_;
      glm::vec2 vel_;
      glm::vec3 scale_;
      float speed_ = 0.f;
   };

   struct plan_t {
      enum class type_t { do_nothing, move_to, wait_for, follow_for, walk_on_spot, };

      type_t type;
      double time;
      glm::vec2 pos;
      entity_id_t entity;

      static plan_t do_nothing() { return{ type_t::do_nothing }; }
      static plan_t move_to(glm::vec2 const & loc) { return{ type_t::move_to, 0., loc }; }
      static plan_t wait_for(double time) { return{ type_t::wait_for, time }; }
      static plan_t follow_for(entity_id_t entity, double time) { return{ type_t::follow_for, time, {}, entity }; }
   };


   //
   // data tables
   //

   struct creature_info_table{
      table_index_t index;

      using entity_id_column_type = table_column_t < entity_id_t >;
      using moment_column_type = table_column_t < moment_t >;
      using creature_column_type = table_column_t < creature_t >;
      using plan_column_type = table_column_t < plan_t >;
      using sprite_column_type = table_column_t < std::unique_ptr<glpp::animation_timeline_t> >;

      entity_id_column_type entity_ids_;
      moment_column_type moments_;
      creature_column_type creature_info_;
      plan_column_type plan_;
      sprite_column_type sprites_;

      creature_info_table()
         : entity_ids_(index)
         , moments_(index)
         , creature_info_(index)
         , plan_(index)
         , sprites_(index)
      {}

      creature_info_table(creature_info_table const &) = delete;
      creature_info_table& operator=(creature_info_table const &) = delete;

      table_row_id_type create_row() {
         auto id = index.alloc_row();
         entity_ids_.alloc_row(id);
         moments_.alloc_row();
         creature_info_.alloc_row();
         plan_.alloc_row();
         sprites_.alloc_row();
         return id;
      }

      void delete_row(table_row_id_type id) {
         entity_ids_.delete_by_id(id);
         moments_.delete_by_id(id);
         creature_info_.delete_by_id(id);
         plan_.delete_by_id(id);
         sprites_.delete_by_id(id);
         index.delete_row(id);
      }

      std::size_t size() const { return index.size(); }

      entity_id_t entity_id(table_row_id_type id) { return id; }
      entity_id_column_type::collection_type & entity_ids() { return entity_ids_.values(); }

      moment_t & moment(table_row_id_type id) { return moments_.select(id); }
      moment_column_type::collection_type & moments() { return moments_.values(); }

      creature_t & creature_info(table_row_id_type id) { return creature_info_.select(id); }
      creature_column_type::collection_type & creature_infos() { return creature_info_.values(); }

      plan_t & plan(table_row_id_type id) { return plan_.select(id); }
      plan_t const & plan(table_row_id_type id) const { return plan_.select(id); }
      plan_column_type::collection_type & plans() { return plan_.values(); }

      glpp::animation_timeline_t & sprite(table_row_id_type id) { return *sprites_.select(id); }
      sprite_column_type::collection_type & sprites() { return sprites_.values(); }
   };


   struct prop_info_table {
      table_index_t index;

      using entity_id_column_type = table_column_t < entity_id_t >;
      using moment_column_type = table_column_t < moment_t >;
      using prop_column_type = table_column_t < prop_t >;
      using sprite_column_type = table_column_t < std::unique_ptr<glpp::animation_timeline_t> >;

      entity_id_column_type entity_ids_;
      moment_column_type moments_;
      prop_column_type prop_info_;
      sprite_column_type sprites_;

      prop_info_table()
         : entity_ids_(index)
         , moments_(index)
         , prop_info_(index)
         , sprites_(index)
      {}

      prop_info_table(prop_info_table const &) = delete;
      prop_info_table& operator=(prop_info_table const &) = delete;

      table_row_id_type create_row() {
         auto id = index.alloc_row();
         entity_ids_.alloc_row(id);
         moments_.alloc_row();
         prop_info_.alloc_row();
         sprites_.alloc_row();
         return id;
      }

      void delete_row(table_row_id_type id) {
         entity_ids_.delete_by_id(id);
         moments_.delete_by_id(id);
         prop_info_.delete_by_id(id);
         sprites_.delete_by_id(id);
         index.delete_row(id);
      }

      std::size_t size() const { return index.size(); }

      entity_id_t entity_id(table_row_id_type id) { return id; }
      entity_id_column_type::collection_type & entity_ids() { return entity_ids_.values(); }

      moment_t & moment(table_row_id_type id) { return moments_.select(id); }
      moment_column_type::collection_type & moments() { return moments_.values(); }

      prop_t & prop_info(table_row_id_type id) { return prop_info_.select(id); }
      prop_column_type::collection_type & prop_infos() { return prop_info_.values(); }

      glpp::animation_timeline_t & sprite(table_row_id_type id) { return *sprites_.select(id); }
      sprite_column_type::collection_type & sprites() { return sprites_.values(); }
   };

   struct particle_info_table {
      table_index_t index;

      using id_column_type = table_column_t < table_row_id_type >;
      using moment_column_type = table_column_t < moment_t >;
      using particle_column_type = table_column_t < particle_t >;
      using sprite_column_type = table_column_t < std::unique_ptr<glpp::animation_timeline_t> >;

      id_column_type ids_;
      moment_column_type moments_;
      particle_column_type particle_info_;
      sprite_column_type sprites_;

      particle_info_table()
         : ids_(index)
         , moments_(index)
         , particle_info_(index)
         , sprites_(index)
      { }

      particle_info_table(particle_info_table const &) = delete;
      particle_info_table& operator=(particle_info_table const &) = delete;

      table_row_id_type create_row() {
         auto id = index.alloc_row();
         ids_.alloc_row(id);
         moments_.alloc_row();
         particle_info_.alloc_row();
         sprites_.alloc_row();
         return id;
      }

      void delete_row(table_row_id_type id) {
         ids_.delete_by_id(id);
         moments_.delete_by_id(id);
         particle_info_.delete_by_id(id);
         sprites_.delete_by_id(id);
         index.delete_row(id);
      }

      std::size_t size() const { return index.size(); }

      table_row_id_type id(table_row_id_type id) { return id; }
      id_column_type::collection_type & ids() { return ids_.values(); }

      moment_t & moment(table_row_id_type id) { return moments_.select(id); }
      moment_column_type::collection_type & moments() { return moments_.values(); }

      particle_t & particle_info(table_row_id_type id) { return particle_info_.select(id); }
      particle_column_type::collection_type & particle_infos() { return particle_info_.values(); }

      glpp::animation_timeline_t & sprite(table_row_id_type id) { return *sprites_.select(id); }
      sprite_column_type::collection_type & sprites() { return sprites_.values(); }
   };
}

#endif // #ifndef GAME_DB__H
