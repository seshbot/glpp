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

template <unsigned CREATE_PER_SEC>
class constant_create_policy_t {
protected:
   const double TIME_BETWEEN_CREATE = (double)1. / CREATE_PER_SEC;
   double next_create_time_ = 0.;

   template <typename T>
   void create_particle(T & emitter) {
      auto pos = glm::vec3{
         (float)(std::rand() % 1400) - 300.f, // (float)(std::rand() % 460), // - 230.,
         800.,
         800.f - (float)(std::rand() % 1200) };// (float)(std::rand() % 350) - 250. };

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

class constant_update_policy_t {
protected:
   using idx_t = std::size_t;

   const glm::vec3 PARTICLE_VEL = { 0., -1000., 0. };

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

using constant_particle_emitter_buffer_t = particle_emitter_buffer_t <
   constant_create_policy_t<6000>,
   constant_update_policy_t,
   constant_depth_delete_policy_t<0> >;


#endif // #ifndef PARTICLES_3D__H