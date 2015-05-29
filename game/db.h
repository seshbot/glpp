#ifndef DB__H
#define DB__H

#include <vector>

// usage e.g., (until I create a 'table' type)
//
// struct phone_number_table {
//   table_index_t index_;
//   table_column_t<table_row_id_type> ids_; // should be read-only
//   table_column_t<std::string> names_;
//   table_column_t<std::string> numbers_;
//   table_row_id_type create_row() {
//      auto new_id = index_.alloc_row();
//      ids_.alloc_row(new_id);
//      names_.alloc_row();
//      numbers_.alloc_row();
//      return new_id;
//   }
//   void delete_by_id(int id) { // must be manually defined for now
//      ids_.delete_by_id(id);
//      names_.delete_by_id(id);
//      numbers_.delete_by_id(id);
//      index_.delete_row(id);
//   }
//   std::size_t size() const { return index_.size(); }
//
//   id_column_type::collection_type & ids() { return ids_.values(); }
//
//   // quickly look up a name by ID
//   std::string & name(table_row_id_type id) { return names_.select(id); }
//   // iterate over all names (tightly packed, cache friendly - well, if it was POD anyway)
//   table_column_t<std::string>::collection_type & names() { return names_.values(); }
//
//   .. also for other columns
// };
//
// creates unique ids for data and keeps in efficient order so lookups are constant time
// NOTE: never shrinks!
// NOTE: assumes table works according to table_column_t (all data is compressed)
// row_id
// row_index


using table_row_id_type = int;

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
   using collection_type = std::vector < T >;

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

#endif // #ifndef DB__H
