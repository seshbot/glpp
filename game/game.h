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
#include <glpp/glpp.h>




using table_row_id_type = int;

// creates unique ids for data and keeps in efficient order so lookups are constant time
// NOTE: never shrinks!
// NOTE: assumes table works according to table_column_t (all data is compressed)
// row_id
// row_index
class table_index_t {
public:
   std::size_t size() const;
   std::size_t index_of(table_row_id_type row_id) const;

   table_row_id_type alloc_row();
   void delete_row(table_row_id_type row_id);

private:
   std::vector<table_row_id_type> row_id_to_row_idx;
   std::vector<table_row_id_type> row_idx_to_row_id;
};


template <typename T>
class table_column_t {
public:
   using collection_type = std::vector < T > ;

   table_column_t(table_index_t const & index);

   T & select(table_row_id_type id);
   T const & select(table_row_id_type id) const;
   void delete_by_id(table_row_id_type id);
   void set_by_id(table_row_id_type id, T & val);

   void alloc_row();
   void alloc_row(T const & val);

   collection_type & values() { return values_; }
   collection_type const & values() const { return values_; }

private:
   table_index_t const & index_;
   collection_type values_;
};


namespace game {

   glm::vec2 random_world_location();

   using entity_id_t = int;

   // holds geometric (pos, dir) info of a game entity
   class moment_t {
   public:
      moment_t();
      moment_t(glm::vec2 const & pos, glm::vec2 const & vel);

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

      void update(double time_since_last);

   private:
      glm::vec2 pos_;
      glm::vec2 dir_;
      glm::vec2 vel_;
      float speed_ = 0.f;
   };

   struct plan_t {
      enum class type_t { do_nothing, move_to, wait_for, follow_for, walk_on_spot, };

      type_t type;
      double time;
      glm::vec2 pos;
      entity_id_t entity;

      static plan_t do_nothing() { return{ type_t::do_nothing }; }
      static plan_t move_to(glm::vec2 & loc) { return{ type_t::move_to, 0., loc }; }
      static plan_t wait_for(double time) { return{ type_t::wait_for, time }; }
      static plan_t follow_for(entity_id_t entity, double time) { return{ type_t::follow_for, time, {}, entity }; }
   };

   struct creature_t {
      enum types {
         person,
         monster,
      };

      types type;
   };

   struct prop_t {
      enum types {
         tree
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


   struct particle_info_table{
      table_index_t index;

      using id_column_type = table_column_t < table_row_id_type >;
      using moment_column_type = table_column_t < moment_t >;
      using particle_column_type = table_column_t < particle_t >;
      using sprite_column_type = table_column_t < std::unique_ptr<glpp::sprite_cursor_t> >;

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

      glpp::sprite_cursor_t & sprite(table_row_id_type id) { return *sprites_.select(id); }
      sprite_column_type::collection_type & sprites() { return sprites_.values(); }
   };


   struct creature_info_table{
      table_index_t index;

      using entity_id_column_type = table_column_t < entity_id_t >;
      using moment_column_type = table_column_t < moment_t >;
      using creature_column_type = table_column_t < creature_t >;
      using plan_column_type = table_column_t < plan_t >;
      using sprite_column_type = table_column_t < std::unique_ptr<glpp::sprite_cursor_t> >;

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

      glpp::sprite_cursor_t & sprite(table_row_id_type id) { return *sprites_.select(id); }
      sprite_column_type::collection_type & sprites() { return sprites_.values(); }
   };


   struct world_t {
   public:
      struct player_controller_t {
         virtual ~player_controller_t() {}
         virtual glm::vec2 get_relative_velocity() const = 0;
      };

      world_t(creature_info_table & entity_db, particle_info_table & particle_db, player_controller_t const & player_controller);

      entity_id_t create_creature(creature_t::types type, moment_t const & moment);
      entity_id_t create_particle(particle_t::types type, moment_t const & moment, double ttl);

      void update(double time_since_last);

      entity_id_t player_id() const { return player_id_; }

   private:
      void creature_think(double time_since_last);
      void player_update(double time_since_last);
      void move_creatures(double time_since_last);
      void move_particles(double time_since_last);

      creature_info_table & entity_db_;
      particle_info_table & particle_db_;
      player_controller_t const & player_controller_;
      entity_id_t player_id_;
   };


   struct world_view_t {
      struct sprite_repository_t {
         virtual ~sprite_repository_t() { }
         virtual glpp::sprite_t const & find_creature_sprite(creature_t const & creature) const = 0;
         virtual float creature_sprite_updating(std::size_t db_idx, creature_t const & creature, glpp::sprite_cursor_t & cursor, moment_t & moment) const = 0;
         virtual glpp::sprite_t const & find_particle_sprite(particle_t const & particle) const = 0;
         virtual float particle_sprite_updating(std::size_t db_idx, particle_t const & particle, glpp::sprite_cursor_t & cursor, moment_t & moment) const = 0;
      };

      struct render_info_t {
         render_info_t(glpp::sprite_cursor_t const & sprite, moment_t const & moment);
         render_info_t(render_info_t const & other);
         render_info_t & operator=(render_info_t const & other);

         glpp::texture_t::id_type tex_id;
         glpp::sprite_cursor_t const * sprite;
         moment_t const * moment;
      };

      world_view_t(creature_info_table & entity_db, particle_info_table & particle_db, sprite_repository_t & sprite_repository);

      void update(double time_since_last);

      using iterator = std::vector<render_info_t>::iterator;

      iterator creatures_begin() { return creature_render_info_.begin(); }
      iterator creatures_end() { return creature_render_info_.end(); }

      iterator particles_begin() { return particle_render_info_.begin(); }
      iterator particles_end() { return particle_render_info_.end(); }

   private:
      void update_creatures(double time_since_last);
      void update_particles(double time_since_last);

      std::unique_ptr<glpp::sprite_cursor_t> create_sprite(creature_t const & creature);
      std::unique_ptr<glpp::sprite_cursor_t> create_sprite(particle_t const & particle);

      creature_info_table & entity_db_;
      particle_info_table & particle_db_;
      sprite_repository_t & sprite_repository_;
      std::vector<render_info_t> creature_render_info_;
      std::vector<render_info_t> particle_render_info_;
   };

}


//
// template definitions
//


template <typename T>
table_column_t<T>::table_column_t(table_index_t const & index) : index_(index) {}

template <typename T>
T & table_column_t<T>::select(table_row_id_type id) { return values_[index_.index_of(id)]; }

template <typename T>
T const & table_column_t<T>::select(table_row_id_type id) const { return values_[index_.index_of(id)]; }

template <typename T>
void table_column_t<T>::delete_by_id(table_row_id_type id) {
   auto elem_idx = index_.index_of(id);
   auto end_idx = values_.size() - 1;
   std::swap(values_[elem_idx], values_[end_idx]);
   values_.pop_back();
}

template <typename T>
void table_column_t<T>::set_by_id(table_row_id_type id, T & val) {
   values_.at(index_.index_of(id)) = val;
}

template <typename T>
void table_column_t<T>::alloc_row() {
   values_.push_back({});
}

template <typename T>
void table_column_t<T>::alloc_row(T const & val) {
   values_.push_back(val);
}


#endif // #ifndef GAME__H