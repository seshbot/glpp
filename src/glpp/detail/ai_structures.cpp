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
   // animation_t implementation
   //

   animation_t::animation_t(animation_t && other)
      : ai_animation(other.ai_animation)
      , ai_scene(other.ai_scene)
      , root_node(other.root_node)
      , global_inverse_transform(std::move(other.global_inverse_transform))
      , nodes(std::move(other.nodes))
      , mesh_animations(std::move(other.mesh_animations)) {
   }

   animation_t & animation_t::operator=(animation_t && other) {
      ai_animation = other.ai_animation;
      ai_scene = other.ai_scene;
      root_node = other.root_node;
      global_inverse_transform = std::move(other.global_inverse_transform);
      nodes = std::move(other.nodes);
      mesh_animations = std::move(other.mesh_animations);
      return *this;
   }

   animation_t::~animation_t() = default;

   animation_t::animation_t(aiScene const & scene, aiAnimation const & animation)
      : ai_animation(&animation)
      , ai_scene(&scene)
   {
      // ensure node data never moves - allocate enough space for one entry per node
      std::function<unsigned(aiNode* n)> child_count = [&](aiNode* n) {
         auto count = 1U; // count ourselves
         for (auto idx = 0U; idx < n->mNumChildren; idx++) count += child_count(n->mChildren[idx]);
         return count;
      };
      auto node_count = child_count(scene.mRootNode);
      nodes.reserve(node_count);

      // node node_animation lookup table
      std::map<std::string, node_animation_t*> nodes_by_name;
      auto lookup_node_anim = [&](aiNode const * node)->node_animation_t * {
         if (!node) return{};
         auto it = nodes_by_name.find(std::string{ node->mName.C_Str() });
         assert(it != nodes_by_name.end());
         return it->second;
      };
      auto lookup_bone_anim = [&](aiBone const * bone)->node_animation_t * {
         assert(bone);
         auto it = nodes_by_name.find(std::string{ bone->mName.C_Str() });
         assert(it != nodes_by_name.end());
         return it->second;
      };

      //
      // create node animations
      //

      std::function<void(aiNode* n)> create_nodes_recursive = [&](aiNode* n) {
         auto node_name = n->mName.C_Str();
         assert(nodes_by_name.find(node_name) == nodes_by_name.end());
         nodes.push_back({ *n });
         nodes_by_name[node_name] = &nodes.back();
         for (auto idx = 0U; idx < n->mNumChildren; idx++)
            create_nodes_recursive(n->mChildren[idx]);
      };
      create_nodes_recursive(scene.mRootNode);

      //
      // create mesh animations by recursively iterating through nodes
      //

      std::function<unsigned(aiNode* n)> count_meshes_recursive = [&](aiNode* n) -> unsigned {
         auto child_mesh_count = 0U;
         for (auto idx = 0U; idx < n->mNumChildren; idx++)
            child_mesh_count += count_meshes_recursive(n->mChildren[idx]);
         return n->mNumMeshes + child_mesh_count;
      };
      mesh_animations.reserve(count_meshes_recursive(scene.mRootNode));

      std::function<void(aiNode* n)> create_meshes_recursive = [&](aiNode* n) {
         auto * node = lookup_node_anim(n);
         for (auto mesh_idx = 0U; mesh_idx < n->mNumMeshes; mesh_idx++) {
            auto & mesh = *scene.mMeshes[n->mMeshes[mesh_idx]];
            std::vector<mesh_animation_t::bone_t> bones;
            for (auto bone_idx = 0U; bone_idx < mesh.mNumBones; bone_idx++) {
               auto * bone = mesh.mBones[bone_idx];
               auto * bone_node = lookup_bone_anim(bone);
               bones.push_back({ *bone, *bone_node });
            }
            mesh_animations.push_back({ mesh, node, std::move(bones) });
            node->mesh_animations_.push_back(&mesh_animations.back());
         }
         for (auto idx = 0U; idx < n->mNumChildren; idx++)
            create_meshes_recursive(n->mChildren[idx]);
      };
      create_meshes_recursive(scene.mRootNode);

      //
      // add animations to nodes and track channels
      //

      for (auto chan_idx = 0U; chan_idx < animation.mNumChannels; chan_idx++) {
         auto* channel = animation.mChannels[chan_idx];
         auto node_name = std::string{ channel->mNodeName.C_Str() };

         auto * node_animation = nodes_by_name.find(node_name)->second;
         node_animation->attach_animation(*channel, chan_idx);
      }

      //
      // connect node animation hierarchy
      //

      for (auto & anim : nodes) {
         auto & node = anim.node_;
         // set parent
         assert(!anim.parent);
         anim.parent = lookup_node_anim(node.mParent);

         // set children
         for (auto child_idx = 0U; child_idx < node.mNumChildren; child_idx++) {
            auto * elem = lookup_node_anim(node.mChildren[child_idx]);
            anim.children.push_back(elem);
         }
      }

      auto root_node_name = std::string{ scene.mRootNode->mName.C_Str() };
      root_node = nodes_by_name.find(root_node_name)->second;

      global_inverse_transform = glm::inverse(ai::to_mat4(root_node->ai_node().mTransformation));
   }

   std::string animation_t::name() const {
      return ai::get_name(*ai_animation);
   }


   //
   // node_animation_snapshot_t implementation
   //

   node_animation_snapshot_t::node_animation_snapshot_t(node_animation_t const & node_animation_in, node_animation_snapshot_t const * parent_in, glm::mat4 const & global_inverse_transform_in, double time_ticks_in, double time_ticks_total_in)
      : node_animation(node_animation_in)
      , parent(parent_in)
      , global_inverse_transform(global_inverse_transform_in)
      , time_ticks(time_ticks_in)
      , time_ticks_total(time_ticks_total_in)
   {
      advance_transforms_to(time_ticks);
   }

   void node_animation_snapshot_t::advance_transforms_to(double time_ticks_in) {
      bool restart = !(time_ticks_in > time_ticks);
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

   void node_animation_snapshot_t::recalc_bones() {
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
