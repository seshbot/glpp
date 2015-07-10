#ifndef PARTICLES_3D__H
#define PARTICLES_3D__H

#include <glpp/glpp.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>



// TODO: lifetime (create/delete), movement (update), render (vertices, buffer) policies
template <typename CreatePolicy, typename UpdatePolicy, typename DeletePolicy>
class particle_emitter_buffer_t
   : public CreatePolicy   // provides create_particles(emitter, t)
   , public UpdatePolicy   // provides particle_created(emitter, idx, t), update_particle(emitter, idx, t)
   , public DeletePolicy { // provides should_delete(emitter, idx)
public:
   using cont_t = std::vector < glm::vec3 >;
   using idx_t = std::size_t;

   void update(double time_since_last) {
      current_time_ += time_since_last;

      auto prev_top = count();
      CreatePolicy::create_particles(*this, time_since_last);

      // glm likes floats
      auto t = static_cast<float>(time_since_last);
      auto new_top = count();

      for (auto idx = prev_top; idx < new_top; idx++) {
         UpdatePolicy::particle_created(*this, idx);
      }

      auto idx = idx_t(0);
      while (idx < count()) {
         UpdatePolicy::update_particle(*this, idx, t);

         if (DeletePolicy::should_delete(*this, idx)) {
            del_at(idx);
         }
         else {
            // only increment if we didnt delete an element
            idx++;
         }
      }

      if (particle_positions_.size() > 0)
         buffer_.update({ glm::value_ptr(particle_positions_[0]), particle_positions_.size() * 3 });
   }

   glpp::buffer_t buffer() { return buffer_; }

   double time() const { return current_time_; }
   idx_t count() const { return particle_velocities_.size(); }
   idx_t add(glm::vec3 pos, glm::vec3 vel = {}) { particle_create_times_.push_back(current_time_);  particle_positions_.push_back(pos); particle_velocities_.push_back(vel); return count() - 1; }
   void del_at(idx_t idx) { del_at_(particle_create_times_, idx);  del_at_(particle_positions_, idx); del_at_(particle_velocities_, idx); }
   double create_time_at(idx_t idx) { return particle_create_times_[idx]; }
   glm::vec3 & pos_at(idx_t idx) { return particle_positions_[idx]; }
   glm::vec3 & vel_at(idx_t idx) { return particle_velocities_[idx]; }

private:
   glpp::buffer_t buffer_ = { glpp::buffer_t::Usage::Dynamic };
   double current_time_ = 0.;
   std::vector<double> particle_create_times_;
   std::vector<glm::vec3> particle_positions_;
   std::vector<glm::vec3> particle_velocities_;

   template <typename T>
   static void del_at_(std::vector<T> & vec, idx_t idx) { auto last = vec.size() - 1; if (idx != last) std::swap(vec[idx], vec[last]); vec.erase(vec.begin() + last); }
};

#include <random>

class single_create_policy_t {
protected:
   bool created_ = false;

   glm::vec3 create_location_ = {400., 50., -200.};
   void set_create_location(glm::vec3 const & loc) { create_location_ = loc; }

   template <typename T>
   void create_particles(T & emitter, double time_since_last) {
      if (created_) return;
      created_ = true;
      emitter.add(create_location_);
   }
};

template <unsigned CREATE_PER_SEC, unsigned ALTITUDE = 800>
class static_continuous_create_policy_t {
protected:
   std::mt19937 rnd;

   const double TIME_BETWEEN_CREATE = (double)1. / CREATE_PER_SEC;
   double next_create_time_ = 0.;

   template <typename T>
   void create_particle(T & emitter) {
      auto pos = glm::vec3{
         (float)(rnd() % 1400) - 700.f, // (float)(std::rand() % 460), // - 230.,
         (float)ALTITUDE,
         600.f - (float)(rnd() % 1200) };// (float)(std::rand() % 350) - 250. };

      emitter.add(pos);
   }

   template <typename T>
   void create_particles(T & emitter, double time_since_last) {
      auto now = emitter.time();
      while (next_create_time_ < now) {
         create_particle(emitter);
         next_create_time_ += TIME_BETWEEN_CREATE;
      }
   }
};

template <unsigned CREATE_NUMBER, unsigned ALTITUDE = 0>
class static_create_policy_t {
protected:
   std::mt19937 rnd;

   template <typename T>
   void create_particle(T & emitter) {
      auto pos = glm::vec3{
         (float)(rnd() % 1400) - 700.f, // (float)(std::rand() % 460), // - 230.,
         (float)ALTITUDE,
         600.f - (float)(rnd() % 1200) };// (float)(std::rand() % 350) - 250. };

      emitter.add(pos);
   }

   template <typename T>
   void create_particles(T & emitter, double time_since_last) {
      for (auto i = emitter.count(); i < CREATE_NUMBER; i++) {
         create_particle(emitter);
      }
      return;
   }
};

template <unsigned ALTITUDE = 0>
class manual_immediate_create_policy_t {
public:
   glm::vec3 creation_center() const { return create_center_; }
   void set_creation_center(glm::vec3 const & center) { create_center_ = center; }

   int creation_radius() const { return create_radius_; }
   void set_creation_radius(int radius) { create_radius_ = radius; }

   int creation_count() const { return create_to_count_; }
   void set_creation_count(int create_count) { create_to_count_ = create_count; }

protected:
   std::mt19937 rnd;
   glm::vec3 create_center_ = {};
   int create_radius_ = 700;
   int create_to_count_ = 0;

   template <typename T>
   void create_particle(T & emitter) {
      auto pos = glm::vec3{
         (float)(rnd() % (2 * create_radius_)) - create_radius_,
         (float)ALTITUDE,
         create_radius_ - (float)(rnd() % (2 * create_radius_)) };

      emitter.add(pos);
   }

   template <typename T>
   void create_particles(T & emitter, double time_since_last) {
      for (int i = emitter.count(); i < create_to_count_; i++) {
         create_particle(emitter);
      }
      return;
   }
};

template <unsigned ALTITUDE = 800>
class manual_continuous_create_policy_t {
public:
   int creation_rate() const { return per_second_; }
   void set_creation_rate(int per_second) { per_second_ = per_second; }

protected:
   std::mt19937 rnd;

   int per_second_ = 0;
   double next_create_time_ = 0.;

   template <typename T>
   void create_particle(T & emitter) {
      auto pos = glm::vec3{
         (float)(rnd() % 1400) - 700.f, // (float)(std::rand() % 460), // - 230.,
         (float)ALTITUDE,
         600.f - (float)(rnd() % 1200) };// (float)(std::rand() % 350) - 250. };

      emitter.add(pos);
   }

   template <typename T>
   void create_particles(T & emitter, double time_since_last) {
      if (per_second_ <= 0) return;

      auto now = emitter.time();
      while (next_create_time_ < now) {
         create_particle(emitter);
         next_create_time_ += 1. / per_second_;
      }
   }
};

class constant_update_policy_t {
protected:
   using idx_t = std::size_t;

   const glm::vec3 PARTICLE_VEL = { 0., -1500., 0. };

   template <typename T>
   void particle_created(T & emitter, idx_t idx) {
      emitter.vel_at(idx) = PARTICLE_VEL;
   }

   template <typename T>
   void update_particle(T & emitter, idx_t idx, float time_since_last) {
      auto & pos = emitter.pos_at(idx);
      pos += emitter.vel_at(idx) * time_since_last;
   }
};

class never_update_policy_t {
protected:
   using idx_t = std::size_t;
   template <typename T>
   void particle_created(T & emitter, idx_t idx) {
   }

   template <typename T>
   void update_particle(T & emitter, idx_t idx, float time_since_last) {
   }
};

class oscillating_update_policy_t {
protected:
   using idx_t = std::size_t;

   float amplitude_ = 7.;
   float frequency_ = 3.;

   template <typename T>
   void particle_created(T & emitter, idx_t idx) {
      emitter.vel_at(idx) = {0., 0., 0.};
   }

   template <typename T>
   void update_particle(T & emitter, idx_t idx, float time_since_last) {
      auto & pos = emitter.pos_at(idx);
      auto create_time = emitter.create_time_at(idx);
      auto xoffs = amplitude_ * std::sin(frequency_ * (float)emitter.time() - create_time);
      pos += glm::vec3{xoffs, 0., 0.};
   }
};

class never_delete_policy_t {
public:
   using idx_t = std::size_t;

   template <typename T>
   bool should_delete(T & emitter, idx_t idx) { return false; }
};

template <unsigned TTL_MS>
class constant_time_delete_policy_t {
public:
   const double TTL_SECS = TTL_MS / 1000.;
   using idx_t = std::size_t;

   template <typename T>
   bool should_delete(T & emitter, idx_t idx) {
      return emitter.time() - emitter.create_time_at(idx) > TTL_SECS;
   }
};

class manual_immediate_delete_policy_t {
public:
   using idx_t = std::size_t;

   unsigned deletion_count() const { return delete_to_count_; }
   void set_deletion_count(unsigned delete_count) { delete_to_count_ = delete_count; }

protected:
   unsigned delete_to_count_ = 0;

   template <typename T>
   bool should_delete(T & emitter, idx_t idx) {
      return idx >= delete_to_count_;
   }
};

template <unsigned TTL_MS>
class constant_depth_delete_policy_t {
public:
   const double TTL_SECS = TTL_MS / 1000.;
   using idx_t = std::size_t;

   template <typename T>
   bool should_delete(T & emitter, idx_t idx) {
      return emitter.pos_at(idx).y <= 0.;
   }
};

using static_continuous_particle_emitter_buffer_t = particle_emitter_buffer_t <
   static_continuous_create_policy_t<6000>,
   constant_update_policy_t,
   constant_depth_delete_policy_t<0> >;

using manual_continuous_particle_emitter_buffer_t = particle_emitter_buffer_t <
   manual_continuous_create_policy_t<>,
   constant_update_policy_t,
   constant_depth_delete_policy_t<0> >;

using manual_immediate_particle_emitter_buffer_t = particle_emitter_buffer_t <
   manual_immediate_create_policy_t<>,
   never_update_policy_t,
   manual_immediate_delete_policy_t >;

using test_particle_emitter_buffer_t = particle_emitter_buffer_t <
   single_create_policy_t,
   oscillating_update_policy_t,
   never_delete_policy_t >;


#endif // #ifndef PARTICLES_3D__H
