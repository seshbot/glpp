#ifndef GLPP_SCENE__H
#define GLPP_SCENE__H

#include <glpp/utils.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/matrix_operation.hpp>

#include <string>
#include <vector>
#include <array>
#include <map>
#include <functional>
#include <memory>


// TODO: all the node hierarchy stuff is just implementation detail and should be hidden from the public interface
// i.e., we should only need:
// - a handle on the scene imported from the file
// - scene::meshes() -> mesh_t[] where mesh_t exposes vertex attributes (a mesh_t that contains vertices, indices, normals, bone indices and bone weights)
// - animation snapshot (or animation instances) that:
//   - intended to be instantiated for each animated entity
//   - provide interface to updated state to reflect arbitrary time points
//   - provide an interface (identical to the scene) for iteration through meshes and their vertex attributes above


// forward declarations
struct aiScene;
struct aiMesh;
struct aiBone;
struct aiAnimation;
struct aiNode;
struct aiNodeAnim;

namespace glpp
{
   namespace ai
   {
      void log_node_info(aiScene const & scene, aiNode const & node, int depth = 0);
      void log_scene_info(aiScene const & scene);

      std::string get_name(aiNode const & node);
      std::string get_name(aiAnimation const & animation);
      std::string get_name(aiNodeAnim const & node);
      std::string get_name(aiMesh const & mesh);

      struct frame_info_t { glm::mat4 transform; int rotation_key_idx; int scale_key_idx; int position_key_idx; };
      frame_info_t calculate_animation_frame(aiNodeAnim const & node_anim, double time_ticks, double time_ticks_total);

      void for_each_mesh(
         aiScene const & scene,
         aiNode const & node,
         glm::mat4 const & parent_transform,
         std::function<void(aiMesh const &, glm::mat4 const &)> callback);

      struct bounds_t { glm::vec3 lower; glm::vec3 upper; };
      bounds_t find_mesh_bounds(aiMesh const & m);
   }

   struct node_animation_t;
   struct mesh_bone_snapshot_t;

   std::string to_string(mesh_bone_snapshot_t const & mesh_bone_snapshots);
   void log_animation_nodes(node_animation_t const & n, std::string indent);


   struct mesh_animation_t {
      aiMesh const & ai_mesh;
      node_animation_t const * node;

      struct bone_t {
         aiBone const & ai_bone;
         node_animation_t const & node;
      };
      std::vector<bone_t> bones;
   };

   struct node_animation_t {
      node_animation_t(aiNode const & node_in)
         : node_(node_in), animation_(nullptr)
      { }

      aiNode const & ai_node() const { return node_; }
      std::string name() const { return ai::get_name(node_); }

      bool has_animation() const { return nullptr != animation_; }
      aiNodeAnim const & ai_animation() const {
         if (!has_animation()) throw std::runtime_error("no animation data for node");
         return *animation_;
      }
      unsigned animation_idx() const { return anim_idx_; }

      node_animation_t const * parent = nullptr;
      std::vector<node_animation_t const *> children;

      std::vector<mesh_animation_t const *> const & mesh_animations() const { return mesh_animations_; }

   private:
      friend struct animation_t;

      void attach_animation(aiNodeAnim const & animation_in, unsigned anim_idx) {
         assert(!animation_ && anim_idx_ == 0 && "unexpectedly set node animation multiple times");
         animation_ = &animation_in;
         anim_idx_ = anim_idx;
         assert((ai::get_name(*animation_) == ai::get_name(node_)) && "associating wrong animation with animation node");
      }

      aiNode const & node_;
      aiNodeAnim const * animation_; // declares local transformations
      unsigned anim_idx_ = 0;
      std::vector<mesh_animation_t const *> mesh_animations_;
   };

   struct animation_t {
      // disallow copy to ensure memory locations remain consistent
      animation_t(animation_t const &) = delete;
      animation_t & operator=(animation_t const &) = delete;

      // allow move because it keeps memory locations for our hierarchy
      animation_t(animation_t && other)
         : ai_animation(other.ai_animation)
         , ai_scene(other.ai_scene)
         , root_node(other.root_node)
         , nodes(std::move(other.nodes))
         , mesh_animations(std::move(other.mesh_animations))
      {}

      animation_t & operator=(animation_t && other) {
         ai_animation = other.ai_animation;
         ai_scene = other.ai_scene;
         root_node = other.root_node;
         global_inverse_transform = std::move(other.global_inverse_transform);
         nodes = std::move(other.nodes);
         mesh_animations = std::move(other.mesh_animations);
         return *this;
      }

      ~animation_t() = default;

      animation_t(aiScene const & scene, aiAnimation const & animation);

      std::string name() const { return ai::get_name(*ai_animation); }

      aiAnimation const * ai_animation;
      aiScene const * ai_scene;
      node_animation_t const * root_node;
      glm::mat4 global_inverse_transform;
      std::vector<node_animation_t> nodes; // storage for node anim hierarchy
      std::vector<mesh_animation_t> mesh_animations;
   };


   struct node_animation_snapshot_t;
   struct mesh_bone_snapshot_t {
      aiMesh const & ai_mesh;
      node_animation_snapshot_t const & mesh_node;
      // the following structures correlate with each other but are kept separate to make it easier to pass data to shaders
      std::vector<aiBone const *> bones;
      std::vector<node_animation_snapshot_t const *> bone_nodes;
      std::vector<glm::mat4> bone_offsets;
      std::vector<glm::mat4> bone_transforms; // recalculated every frame update
   };

   struct node_animation_snapshot_t {
      node_animation_snapshot_t(node_animation_t const & node_animation_in, node_animation_snapshot_t const * parent_in, glm::mat4 const & global_inverse_transform_in, double time_ticks_in, double time_ticks_total_in)
         : node_animation(node_animation_in)
         , parent(parent_in)
         , global_inverse_transform(global_inverse_transform_in)
         , time_ticks(time_ticks_in)
         , time_ticks_total(time_ticks_total_in)
      {
         advance_transforms_to(time_ticks);
      }

      void advance_transforms_to(double time_ticks_in);

      void recalc_bones();

      std::string name() const { return node_animation.name(); }

      aiNode const & node() const { return node_animation.ai_node(); }

      node_animation_t const & node_animation;
      node_animation_snapshot_t const * parent;
      std::vector<node_animation_snapshot_t const *> children;
      glm::mat4 const & global_inverse_transform;
      double time_ticks;
      double time_ticks_total;

      int rot_key_idx;
      int pos_key_idx;
      int scale_key_idx;
      glm::mat4 local_transform;
      glm::mat4 global_transform;

      std::vector<mesh_bone_snapshot_t> mesh_bone_snapshots;
   };


   class mesh_t {
   public:
      class material_t {
      public:
         glm::vec4 const & diffuse_colour() const { return diffuse_colour_; }

      private:
         friend class mesh_t;
         material_t(aiScene const & scene, aiMesh const & mesh);
         glm::vec4 diffuse_colour_;
      };
      template <typename DataT>
      struct buffer_desc_t { DataT const * buffer; unsigned count; };

      std::string name() const { return ai::get_name(*ai_mesh_); }

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
      friend struct animation_snapshot_t;
      mesh_t(aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform, std::vector<glm::mat4> const & bone_transforms);

      aiMesh const * ai_mesh_;
      glm::mat4 default_transform_;
      std::vector<glm::mat4> const & bone_transforms_;
      material_t material_;
      std::vector<uint32_t> indices_;
      std::vector<std::array<float, 4>> bone_indices_;
      std::vector<std::array<float, 4>> bone_weights_;
   };


   struct animation_snapshot_t {
      // disallow copy to ensure memory locations remain consistent
      animation_snapshot_t(animation_snapshot_t const &) = delete;
      animation_snapshot_t & operator=(animation_snapshot_t const &) = delete;

      animation_snapshot_t(animation_snapshot_t && other)
         : animation(other.animation)
         , root_node_snapshot(other.root_node_snapshot)
         , node_snapshots(std::move(other.node_snapshots))
      {}
      animation_snapshot_t & operator=(animation_snapshot_t && other) {
         animation = other.animation;
         root_node_snapshot = other.root_node_snapshot;
         node_snapshots = std::move(other.node_snapshots);
         return *this;
      }

      ~animation_snapshot_t() = default;

      animation_snapshot_t(animation_t const & animation_in, double time_secs = 0);
      void advance_to(double time_secs);

      template <typename CallbackT>
      void for_each_mesh_impl(node_animation_snapshot_t const & node_snapshot, CallbackT callback) {
         for (auto & mesh_bones : node_snapshot.mesh_bone_snapshots) {
            callback(mesh_bones);
         }

         for (auto * child : node_snapshot.children) {
            for_each_mesh_impl(*child, callback);
         }
      }

      // callback void(mesh_bone_snapshot_t const & mesh_bone_snapshots)
      // elements of bone_transforms match bone_node elements (transforms given separately so they can be used as-is for shader)
      template <typename CallbackT>
      void for_each_mesh(CallbackT callback) {
         for_each_mesh_impl(*root_node_snapshot, callback);
      }

      animation_t const * animation;
      node_animation_snapshot_t const * root_node_snapshot;
      std::vector<node_animation_snapshot_t> node_snapshots;
      std::vector<mesh_t> meshes;

      // 
      // for rendering bones to screen (debugging)
      // 
      std::vector<std::string> node_names_with_bones() const {
         auto result = std::vector < std::string > {};
         for (auto & p : node_bone_transforms_) {
            result.push_back(p.first);
         }
         return result;
      }
      std::vector<glm::mat4> const & node_bone_transforms(std::string const & node_name) const {
         return node_bone_transforms_.find(node_name)->second;
      }
      std::vector<std::string> const & node_bone_names(std::string const & node_name) const {
         return node_bone_names_.find(node_name)->second;
      }
      std::map<std::string, std::vector<glm::mat4>> node_bone_transforms_;
      std::map<std::string, std::vector<std::string>> node_bone_names_;
   };


   class scene_t {
   public:
      ~scene_t();

      static scene_t load_from_file(std::string const & filename);

      std::vector<std::string> animation_names() const;
      animation_t const & animation(std::string name) const;

      aiScene const & ai_scene() const { return *ai_scene_; }

   private:
      scene_t(aiScene const * ai_scene);

      std::unique_ptr<const aiScene> ai_scene_;
      std::vector<animation_t> animations_;
   };

} // namespace glpp


#endif
