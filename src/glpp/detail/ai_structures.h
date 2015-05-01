#ifndef GLPP_AI_STRUCTURES__H
#define GLPP_AI_STRUCTURES__H

#include <string>
#include <vector>
#include <memory>
#include <cassert>

#include <glm/mat4x4.hpp>


struct aiScene;
struct aiNode;
struct aiNodeAnim;
struct aiAnimation;
struct aiMesh;
struct aiBone;

namespace glpp
{
namespace ai
{
   struct animation_t;
   struct node_animation_t;
   struct node_animation_timeline_t;
   struct mesh_animation_t;
   struct mesh_bone_snapshot_t;

   void log_animation_nodes(node_animation_t const & n, std::string indent);
   std::string to_string(mesh_bone_snapshot_t const & mesh_bone_snapshots);

   struct mesh_animation_t {
      aiMesh const & ai_mesh;
      node_animation_t const * node;

      struct bone_t {
         aiBone const & ai_bone;
         node_animation_t const & node;
      };
      std::vector<bone_t> bones;
   };

   // encapsulates a node in relation to its animation information and parent/children in the hierarchy
   struct node_animation_t {
      node_animation_t(aiNode const & node_in);

      aiNode const & ai_node() const { return node_; }
      std::string name() const;

      bool has_animation() const { return nullptr != animation_; }
      aiNodeAnim const & ai_animation() const;
      unsigned animation_idx() const { return anim_idx_; }

      node_animation_t const * parent = nullptr;
      std::vector<node_animation_t const *> children;

      std::vector<mesh_animation_t const *> const & mesh_animations() const { return mesh_animations_; }

      void attach_animation(aiNodeAnim const & animation_in, unsigned anim_idx);

      aiNode const & node_;
      aiNodeAnim const * animation_; // declares local transformations
      unsigned anim_idx_ = 0;
      std::vector<mesh_animation_t const *> mesh_animations_;
   };

   // owns all high-level state for a single animation, including node hierarchy and mesh bone structure
   struct animation_t {
   public:
      animation_t(animation_t && other) = default;
      animation_t & operator=(animation_t && other) = default;

      animation_t(aiScene const & scene, aiAnimation const & animation);
      ~animation_t();

      std::string name() const;

      aiAnimation const * ai_animation;
      aiScene const * ai_scene;

      node_animation_t const * root_node;
      glm::mat4 global_inverse_transform;
      std::vector<node_animation_t> nodes; // storage for node anim hierarchy
      std::vector<mesh_animation_t> mesh_animations;
   };

   // encapsulates all information required to recalculate bone transforms for a mesh
   struct mesh_bone_snapshot_t {
      aiMesh const & ai_mesh;
      node_animation_timeline_t const & mesh_node;
      // the following structures correlate with each other but are kept separate to make it easier to pass data to shaders
      std::vector<aiBone const *> bones;
      std::vector<node_animation_timeline_t const *> bone_nodes;
      std::vector<glm::mat4> bone_offsets;
      std::vector<glm::mat4> bone_transforms; // recalculated every frame update
   };

   // encapsulates both node animation info (for calculating node transforms over time) and bone hierarchy (for calculating final mesh node positions)
   // TODO: separate above structures out and create 'calculated result' structure, maybe to get rid of this struct
   // NOTE: this owns snapshot information only - all static node information is owned by animation_t
   struct node_animation_timeline_t {
      node_animation_timeline_t(node_animation_t const & node_animation_in, node_animation_timeline_t const * parent_in, glm::mat4 const & global_inverse_transform_in, double time_ticks, double time_ticks_total);

      // uses node hierarchy to calculate 'global_transform'
      void advance_transforms_to(double time_ticks);
      // turn newly-transformed bone nodes into an array of bone transforms
      void recalc_bones();

      std::string name() const { return node_animation.name(); }
      aiNode const & node() const { return node_animation.ai_node(); }

      node_animation_t const & node_animation;
      node_animation_timeline_t const * parent;
      std::vector<node_animation_timeline_t const *> children;
      glm::mat4 const & global_inverse_transform;
      double time_ticks;
      double time_ticks_total;

      int rot_key_idx = 0;
      int pos_key_idx = 0;
      int scale_key_idx = 0;
      glm::mat4 local_transform;
      glm::mat4 global_transform;

      std::vector<mesh_bone_snapshot_t> mesh_bone_snapshots;
   };


} // namespace ai

} // namespace glpp

#endif // #ifndef GLPP_AI_STRUCTURES__H
