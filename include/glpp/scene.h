#ifndef GLPP_SCENE__H
#define GLPP_SCENE__H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>
#include <array>
#include <functional>
#include <memory>


// forward declarations
struct aiScene;
struct aiMesh;

namespace glpp
{
   //
   // forward declarations
   //
   namespace ai {
      struct animation_t;
   }
   class scene_t;
   class mesh_t;

   /**
    * calculates all mesh states for a loaded animation baseond on a point in time
    */
   class animation_timeline_t {
   public:
      animation_timeline_t(animation_timeline_t const &) = delete;
      animation_timeline_t & operator=(animation_timeline_t const &) = delete;
	   animation_timeline_t(animation_timeline_t &&);
      animation_timeline_t & operator=(animation_timeline_t &&);
      ~animation_timeline_t();

      std::string name() const;

      void advance_to(double time_secs);
      void advance_by(double time_secs);

      scene_t const & scene() const;
      unsigned scene_idx() const;
      std::vector<mesh_t> const & meshes() const;
      double current_time_secs() const;

      // 
      // for rendering bones to screen (debugging)
      // 
      std::vector<std::string> node_names_with_bones() const;
      std::vector<glm::mat4> const & node_bone_transforms(std::string const & node_name) const;
      std::vector<std::string> const & node_bone_names(std::string const & node_name) const;

   private:
      friend class scene_t;
      animation_timeline_t(ai::animation_t const & animation, scene_t const & scene, unsigned scene_idx, double time_secs = 0);

      struct impl;
      std::unique_ptr<impl> impl_;
   };


   /**
    * a handle to a loaded scene, from which animations may be created
    */
   class scene_t {
   public:
      scene_t(scene_t const &) = delete;
      scene_t & operator=(scene_t const &) = delete;
	   scene_t(scene_t &&);
	   scene_t & operator=(scene_t&&);
      ~scene_t();

      static scene_t create_from_file(std::string const & filename);

      unsigned id() const { return id_; }

      // unanimated meshes
      std::vector<mesh_t> const & meshes() const;

      std::vector<std::string> animation_names() const;
      animation_timeline_t create_timeline(unsigned idx) const;
      animation_timeline_t create_timeline(std::string const & name) const;

   private:
      scene_t(aiScene const * ai_scene);

      struct impl;
      std::unique_ptr<const impl> impl_;
      unsigned id_;
   };


   /**
    * Captures the state of a single mesh at a point in time (created by animation_timeline_t)
    */
   class mesh_t {
   public:
      struct material_t {
         const glm::vec4 diffuse_colour;
         const float shininess;
      };

      template <typename DataT>
      struct buffer_desc_t { DataT const * buffer; unsigned count; };

      std::string name() const;
      bool is_animated() const { return is_animated_; }

      std::vector<glm::mat4> const & bone_transforms() const;
      material_t const & material() const { return material_; }

      unsigned bone_count() const;

      // TODO: expose a buffer_t or static_array_t?
      buffer_desc_t<float> vertices() const;
      buffer_desc_t<uint32_t> indices() const;
      buffer_desc_t<float> normals() const;

      buffer_desc_t<float> bone_indices() const;
      buffer_desc_t<float> bone_weights() const;

   private:
      friend class scene_t;
      friend class animation_timeline_t;
      // for creating unanimated mesh
      mesh_t(aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform);
      // for creating animated mesh
      mesh_t(aiScene const & scene, aiMesh const & mesh, std::vector<glm::mat4> const & bone_transforms);

      aiMesh const * ai_mesh_;
      bool is_animated_;
      std::vector<glm::mat4> default_transforms_;
      std::vector<glm::mat4> const * bone_transforms_;
      material_t material_;
      std::vector<uint32_t> indices_;
      std::vector<std::array<float, 4>> bone_indices_;
      std::vector<std::array<float, 4>> bone_weights_;
   };

} // namespace glpp


#endif
