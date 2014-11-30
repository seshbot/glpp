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
#include "gl.h"




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
   using entity_id_t = int;

   // holds geometric (pos, dir) info of a game entity
   class moment_t {
   public:
      moment_t();
      moment_t(glm::vec2 const & pos, glm::vec2 const & vel);

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


   struct entity_info_table{
      table_index_t index;

      using entity_id_column_type = table_column_t < entity_id_t >;
      using moment_column_type = table_column_t < moment_t >;
      using creature_column_type = table_column_t < creature_t >;
      using sprite_column_type = table_column_t < std::unique_ptr<gl::sprite_cursor_t> >;
      entity_id_column_type entity_ids_;
      moment_column_type moments_;
      creature_column_type creature_info_;
      sprite_column_type sprites_;

      entity_info_table()
         : entity_ids_(index)
         , moments_(index)
         , creature_info_(index)
         , sprites_(index)
      {}

      entity_info_table(entity_info_table const &) = delete;
      entity_info_table& operator=(entity_info_table const &) = delete;

      table_row_id_type create_row() {
         auto id = index.alloc_row();
         entity_ids_.alloc_row(id);
         moments_.alloc_row();
         creature_info_.alloc_row();
         sprites_.alloc_row();
         return id;
      }

      void delete_row(table_row_id_type id) {
         entity_ids_.delete_by_id(id);
         moments_.delete_by_id(id);
         creature_info_.delete_by_id(id);
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

      gl::sprite_cursor_t & sprite(table_row_id_type id) { return *sprites_.select(id); }
      sprite_column_type::collection_type & sprites() { return sprites_.values(); }
   };


   struct world_t {
   public:
      struct player_controller_t {
         virtual ~player_controller_t() {}
         virtual glm::vec2 get_relative_velocity() const = 0;
      };

      world_t(entity_info_table & entity_db, player_controller_t const & player_controller);

      entity_id_t create_creature(creature_t::types type, moment_t const & moment);

      void update(double time_since_last);

   private:
      void creature_think(double time_since_last);
      void player_update(double time_since_last);
      void move_creatures(double time_since_last);

      entity_info_table & entity_db_;
      player_controller_t const & player_controller_;
      entity_id_t player_id;
   };


   struct world_view_t {
      struct render_info_t {
         render_info_t(gl::sprite_cursor_t const & sprite, moment_t const & moment);
         render_info_t(render_info_t & other);

         render_info_t & operator=(render_info_t & other);

         gl::texture_t::id_type tex_id;
         gl::sprite_cursor_t const * sprite;
         moment_t const * moment;
      };

      struct render_callback_t : public gl::pass_t::render_callback {
         std::vector<render_info_t> render_info_;

         render_callback_t();
         render_callback_t(std::vector<render_info_t> render_info);

         render_callback_t(render_callback_t && other);
         render_callback_t & operator=(render_callback_t && other);

         virtual bool prepare_next(gl::program & p) const;

      private:
         mutable std::size_t current_idx_ = 0;
         mutable gl::texture_t::id_type current_tex_id_ = 0;

         render_callback_t(render_callback_t const &) {}
         render_callback_t & operator=(render_callback_t const &) {}
      };

      entity_info_table & entity_db_;
      render_callback_t render_callback;

      world_view_t(entity_info_table & entity_db);

      void update(double time_since_last);

   private:
      void update_sprite_cursor(double time_since_last, gl::sprite_cursor_t & cursor, moment_t & moment);
      std::unique_ptr<gl::sprite_cursor_t> create_sprite(game::creature_t const & creature);
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