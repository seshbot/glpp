#include "ai_structures.h"

#include <functional>
#include <glpp/utils.h>

#include <glm/gtx/matrix_operation.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

#include <map>

#include "ai_helpers.h"


namespace glpp {
namespace ai {

   void log_animation_nodes(node_animation_t const & n, std::string indent) {
      if (n.has_animation())
         utils::log(utils::LOG_INFO, "%s node %s (chan%d: %d rot %d pos %d scale keys)\n", indent.c_str(), n.name().c_str(), n.animation_idx(), n.ai_animation().mNumRotationKeys, n.ai_animation().mNumPositionKeys, n.ai_animation().mNumScalingKeys);
      else
         utils::log(utils::LOG_INFO, "%s node %s (no keys)\n", indent.c_str(), n.name().c_str());
      for (auto & m : n.mesh_animations()) {
         auto bones = std::string{};
         for (auto & b : m->bones) {
            if (bones.length() > 0) bones += ", ";
            bones += std::string{ b.ai_bone.mName.C_Str() } +":" + b.node.name();
         }
         if (bones.length() > 0) bones = " (" + bones + ")";
         utils::log(utils::LOG_INFO, "  %s mesh %s %d bones%s\n", indent.c_str(), m->ai_mesh.mName.C_Str(), m->bones.size(), bones.c_str());
      }
      for (auto & c : n.children) {
         log_animation_nodes(*c, "  " + indent);
      }
   };

   std::string to_string(mesh_bone_snapshot_t const & mesh_bone_snapshots) {
      std::string bone_info_str;

      unsigned char bone_idx = 0;
      for (auto bone_node : mesh_bone_snapshots.bones) {
         auto avg_weight = 0.f;
         for (auto weight_idx = 0U; weight_idx < bone_node->mNumWeights; weight_idx++) {
            auto & weight = bone_node->mWeights[weight_idx];
            avg_weight += weight.mWeight;
         }
         bone_idx++;

         avg_weight /= bone_node->mNumWeights;
         if (bone_info_str.length() > 0) bone_info_str += ", ";
         bone_info_str += std::string{ "[bone" } +std::to_string(bone_idx) + " '" + std::string{ bone_node->mName.C_Str() } +"' - weights: " + std::to_string(bone_node->mNumWeights) + ", avg : " + std::to_string(avg_weight) + "]";
      }

      return bone_info_str;
   }


   //
   // node_animation_t implementation
   //

   node_animation_t::node_animation_t(aiNode const & node_in)
      : node_(node_in), animation_(nullptr)
   { }
   
   std::string node_animation_t::name() const {
      return ai::get_name(node_);
   }

   aiNodeAnim const & node_animation_t::ai_animation() const {
      if (!has_animation()) throw std::runtime_error("no animation data for node");
      return *animation_;
   }

   void node_animation_t::attach_animation(aiNodeAnim const & animation_in, unsigned anim_idx) {
      assert(!animation_ && anim_idx_ == 0 && "unexpectedly set node animation multiple times");
      animation_ = &animation_in;
      anim_idx_ = anim_idx;
      assert((ai::get_name(*animation_) == ai::get_name(node_)) && "associating wrong animation with animation node");
   }


   //
   // node_animation_timeline_t implementation
   //

   node_animation_timeline_t::node_animation_timeline_t(node_animation_t const & node_animation_in, node_animation_timeline_t const * parent_in, glm::mat4 const & global_inverse_transform_in, double time_ticks_in, double time_ticks_total_in)
      : node_animation(node_animation_in)
      , parent(parent_in)
      , global_inverse_transform(global_inverse_transform_in)
      , time_ticks(time_ticks_in)
      , time_ticks_total(time_ticks_total_in)
   {
      advance_transforms_to(time_ticks);
   }

   void node_animation_timeline_t::advance_transforms_to(double time_ticks_in) {
      time_ticks = time_ticks_in;
      if (!node_animation.has_animation()) {
         local_transform = ai::to_mat4(node_animation.ai_node().mTransformation);
      }
      else {
         auto & ai_anim = node_animation.ai_animation();
         auto prev_frame_info = ai::frame_info_t{ {},  rot_key_idx, scale_key_idx, pos_key_idx };
         auto frame_info = ai::calculate_animation_frame(ai_anim, time_ticks, time_ticks_total, prev_frame_info);

         rot_key_idx = frame_info.rotation_key_idx;
         pos_key_idx = frame_info.position_key_idx;
         scale_key_idx = frame_info.scale_key_idx;

         // create local transform from above
         local_transform = frame_info.transform;
      }

      // create global_transform from parent + local
      if (!parent) {
         global_transform = local_transform;
      }
      else {
         global_transform = parent->global_transform * local_transform;
      }
   }

   void node_animation_timeline_t::recalc_bones() {
      for (auto & mesh_bones : mesh_bone_snapshots) {
         auto bone_count = mesh_bones.bone_nodes.size();
         auto & bone_transforms = mesh_bones.bone_transforms;
         bone_transforms.clear();
         for (auto idx = 0U; idx < bone_count; idx++) {
            auto & bone_node = mesh_bones.bone_nodes[idx];
            auto & bone_offset = mesh_bones.bone_offsets[idx];
            auto bone_transform = global_inverse_transform * bone_node->global_transform * bone_offset;
            bone_transforms.push_back(bone_transform);
         }
      }
   }


} // namespace ai
} // namespace glpp
