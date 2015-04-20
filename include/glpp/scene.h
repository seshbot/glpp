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
   class mesh_t;

   /**
    * calculates all mesh states for a loaded animation baseond on a point in time
    */
   class animation_snapshot_t {
   public:
      ~animation_snapshot_t();

      void advance_to(double time_secs);

      std::vector<mesh_t> const & meshes() const;

      // 
      // for rendering bones to screen (debugging)
      // 
      std::vector<std::string> node_names_with_bones() const;
      std::vector<glm::mat4> const & node_bone_transforms(std::string const & node_name) const;
      std::vector<std::string> const & node_bone_names(std::string const & node_name) const;

   private:
      friend class scene_t;
      animation_snapshot_t(ai::animation_t const & animation, double time_secs = 0);

      struct impl;
      std::unique_ptr<impl> impl_;
   };


   /**
    * a handle to a loaded scene, from which animations may be created
    */
   class scene_t {
   public:
      ~scene_t();

      static scene_t load_from_file(std::string const & filename);

      std::vector<std::string> animation_names() const;
      animation_snapshot_t start_animation(std::string const & name) const;

   private:
      scene_t(aiScene const * ai_scene);

      struct impl;
      std::unique_ptr<const impl> impl_;
   };


   /**
    * Captures the state of a single mesh at a point in time (created by animation_snapshot_t)
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

      glm::mat4 const & default_transform() const { return default_transform_; }
      std::vector<glm::mat4> const & bone_transforms() const { return bone_transforms_; }
      material_t const & material() const { return material_; }

      unsigned bone_count() const;

      // TODO: expose a buffer_t or static_array_t?
      buffer_desc_t<float> vertices() const;
      buffer_desc_t<uint32_t> indices() const;
      buffer_desc_t<float> normals() const;

      buffer_desc_t<float> bone_indices() const;
      buffer_desc_t<float> bone_weights() const;

   private:
      friend class animation_snapshot_t;
      mesh_t(aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform, std::vector<glm::mat4> const & bone_transforms);

      aiMesh const * ai_mesh_;
      glm::mat4 default_transform_;
      std::vector<glm::mat4> const & bone_transforms_;
      material_t material_;
      std::vector<uint32_t> indices_;
      std::vector<std::array<float, 4>> bone_indices_;
      std::vector<std::array<float, 4>> bone_weights_;
   };

} // namespace glpp


#endif
