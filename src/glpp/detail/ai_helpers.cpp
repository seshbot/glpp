#include "ai_helpers.h"

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

#include <glpp/utils.h>


namespace glpp {
namespace ai {

   void log_node_info(aiScene const & scene, aiNode const & node, int depth) {
      auto indent = std::string(depth * 2, ' ');
      utils::log(utils::LOG_INFO, " %s- %s %d children, %d meshes\n", indent.c_str(), node.mName.C_Str(), node.mNumChildren, node.mNumMeshes);

      for (auto mesh_idx = 0U; mesh_idx < node.mNumMeshes; mesh_idx++) {
         auto & mesh = *scene.mMeshes[node.mMeshes[mesh_idx]];
         std::string bones;
         for (auto bone_idx = 0U; bone_idx < mesh.mNumBones; bone_idx++) {
            if (bones.length() > 0) bones += ", ";
            bones = bones + mesh.mBones[bone_idx]->mName.C_Str();
         }
         if (bones.length() > 0) bones = " (" + bones + ")";
         utils::log(utils::LOG_INFO, "   %s- [mesh '%s' %d bones%s]\n", indent.c_str(), mesh.mName.C_Str(), mesh.mNumBones, bones.c_str());
      }

      for (auto idx = 0U; idx < node.mNumChildren; idx++) {
         log_node_info(scene, *node.mChildren[idx], depth + 1);
      }
   }


   void log_scene_info(aiScene const & scene) {
      auto * node = scene.mRootNode;
      if (node) {
         utils::log(utils::LOG_INFO, "== Scene Node Hierarchy ==\n");
         glpp::ai::log_node_info(scene, *node);
      }

      auto mat_name = [&scene](unsigned idx) {
         aiString name;
         auto & m = *scene.mMaterials[idx];
         if (AI_SUCCESS != m.Get(AI_MATKEY_NAME, name)) return std::string("NONAME");
         return std::string(name.C_Str());
      };

      utils::log(utils::LOG_INFO, "== Materials ==\n");
      for (auto idx = 0U; idx < scene.mNumMaterials; idx++) {
         auto & m = *scene.mMaterials[idx];

         std::string props;
         for (auto pidx = 0U; pidx < m.mNumProperties; pidx++) {
            auto & p = *m.mProperties[pidx];
            if (props.empty()) props += " "; else props += ", "; props += p.mKey.C_Str();
         }

         utils::log(utils::LOG_INFO, " - mat%d (%s): %d props (%s)\n", idx, mat_name(idx).c_str(), m.mNumProperties, props.c_str());
      }

      utils::log(utils::LOG_INFO, "== Meshes ==\n");
      for (auto idx = 0U; idx < scene.mNumMeshes; idx++) {
         auto & m = *scene.mMeshes[idx];
         utils::log(utils::LOG_INFO, " - mesh%d: %d verts (%d%s faces), %d bones, mat %s, %d anim meshes\n",
            idx, m.mNumVertices, m.mNumFaces, m.mPrimitiveTypes == aiPrimitiveType_TRIANGLE ? " tri" : "",
            m.mNumBones, mat_name(m.mMaterialIndex).c_str(), m.mNumAnimMeshes);

         for (auto bidx = 0U; bidx < m.mNumBones; bidx++) {
            auto & b = *m.mBones[bidx];
            utils::log(utils::LOG_INFO, "   - bone%d (%s): %d weights\n", bidx, b.mName.C_Str(), b.mNumWeights);
         }
      }

      utils::log(utils::LOG_INFO, "== Animations ==\n");
      for (auto idx = 0U; idx < scene.mNumAnimations; idx++) {
         auto & a = *scene.mAnimations[idx];
         utils::log(utils::LOG_INFO, " - anim%d: '%s' %d channels, %d mesh channels\n", idx, a.mName.C_Str(), a.mNumChannels, a.mNumMeshChannels);
         for (auto cidx = 0U; cidx < a.mNumChannels; cidx++) {
            auto & c = *a.mChannels[cidx];
            utils::log(utils::LOG_INFO, "   - chan%d (%s): %d pos, %d rot, %d scale keys\n", cidx, c.mNodeName.C_Str(), c.mNumPositionKeys, c.mNumRotationKeys, c.mNumScalingKeys);
         }
         for (auto cidx = 0U; cidx < a.mNumMeshChannels; cidx++) {
            auto & c = *a.mMeshChannels[cidx];
            utils::log(utils::LOG_INFO, "   - mesh chan%d (%s): %d keys\n", cidx, c.mName.C_Str(), c.mNumKeys);
         }
      }
   }

   std::string get_name(aiNode const & node) {
      return node.mName.C_Str();
   }

   std::string get_name(aiAnimation const & animation) {
      return animation.mName.C_Str();
   }

   std::string get_name(aiNodeAnim const & node) {
      return node.mNodeName.C_Str();
   }

   std::string get_name(aiMesh const & mesh) {
      return mesh.mName.C_Str();
   }

   glm::mat4 to_mat4(aiMatrix4x4 const & from) {
      glm::mat4 to;
      to[0][0] = from.a1; to[1][0] = from.a2;
      to[2][0] = from.a3; to[3][0] = from.a4;
      to[0][1] = from.b1; to[1][1] = from.b2;
      to[2][1] = from.b3; to[3][1] = from.b4;
      to[0][2] = from.c1; to[1][2] = from.c2;
      to[2][2] = from.c3; to[3][2] = from.c4;
      to[0][3] = from.d1; to[1][3] = from.d2;
      to[2][3] = from.d3; to[3][3] = from.d4;

      return to;
   }

   glm::mat4 to_mat4(aiQuaternion const & from) {
      auto from_mat = aiMatrix4x4(from.GetMatrix());
      glm::mat4 to;
      to[0][0] = from_mat.a1; to[1][0] = from_mat.a2;
      to[2][0] = from_mat.a3; to[3][0] = from_mat.a4;
      to[0][1] = from_mat.b1; to[1][1] = from_mat.b2;
      to[2][1] = from_mat.b3; to[3][1] = from_mat.b4;
      to[0][2] = from_mat.c1; to[1][2] = from_mat.c2;
      to[2][2] = from_mat.c3; to[3][2] = from_mat.c4;
      to[0][3] = from_mat.d1; to[1][3] = from_mat.d2;
      to[2][3] = from_mat.d3; to[3][3] = from_mat.d4;

      return to;
   }

   namespace {

      template <typename TransformT>
      struct frame {
         unsigned key_index;
         TransformT transform;
      };

      static aiQuaternion interpolate(aiQuaternion const & q1, aiQuaternion const & q2, float fraction) {
         aiQuaternion result{ 1, 0, 0, 0 };
         aiQuaternion::Interpolate(result, q1, q2, fraction);
         return result;
      }

      static aiVector3D interpolate(aiVector3D const & v1, aiVector3D const & v2, float fraction) {
         return v1 + (v2 - v1) * fraction;
      }

      template <typename ResultTransformT, typename KeyT>
      static frame<ResultTransformT> make_frame(unsigned num_keys, KeyT* keys, double time_ticks, double duration_ticks, unsigned start_key_index = 0) {
         if (num_keys == 0) return{};

         // find index of the last key whose time is less than now
         unsigned key_index = start_key_index;
         for (auto idx = 0U; idx < (num_keys - 1); idx++) {
            auto & next_key = keys[idx + 1];
            // if next key is after now
            if (next_key.mTime > time_ticks) {
               key_index = idx;
               break;
            }
         }

         auto next_key_index = key_index + 1;
         if (next_key_index >= num_keys) next_key_index = 0;

         auto & key1 = keys[key_index];
         auto & key2 = keys[next_key_index];

         auto time_diff = key2.mTime - key1.mTime;
         time_diff = (time_diff < 0.) ? time_diff + duration_ticks : time_diff;

         decltype(KeyT::mValue) frame_xform;
         if (time_diff == 0.) {
            frame_xform = key1.mValue;
         }
         else {
            auto fraction = (float)((time_ticks - key1.mTime) / time_diff);
            assert(fraction >= 0. && fraction <= 1.);
            frame_xform = interpolate(key1.mValue, key2.mValue, fraction);
         }

         return{ key_index, frame_xform };
      }
   }

   frame_info_t calculate_animation_frame(aiNodeAnim const & ai_anim, double time_ticks, double time_ticks_total, frame_info_t const & prev_frame) {
      frame_info_t result;

      // interpolate key frame from pos, rot and scale keys
      auto rot_frame = make_frame<aiQuaternion>(ai_anim.mNumRotationKeys, ai_anim.mRotationKeys, time_ticks, time_ticks_total, prev_frame.rotation_key_idx);
      auto pos_frame = make_frame<aiVector3D>(ai_anim.mNumPositionKeys, ai_anim.mPositionKeys, time_ticks, time_ticks_total, prev_frame.position_key_idx);
      auto scale_frame = make_frame<aiVector3D>(ai_anim.mNumScalingKeys, ai_anim.mScalingKeys, time_ticks, time_ticks_total, prev_frame.scale_key_idx);

      result.rotation_key_idx = rot_frame.key_index;
      result.position_key_idx = pos_frame.key_index;
      result.scale_key_idx = scale_frame.key_index;

      auto ai_xform = aiMatrix4x4(rot_frame.transform.GetMatrix());
      //    aiMatrix4x4& mat = mTransforms[a];
      //    mat = aiMatrix4x4( presentRotation.GetMatrix());
      ai_xform.a1 *= scale_frame.transform.x; ai_xform.b1 *= scale_frame.transform.x; ai_xform.c1 *= scale_frame.transform.x;
      ai_xform.a2 *= scale_frame.transform.y; ai_xform.b2 *= scale_frame.transform.y; ai_xform.c2 *= scale_frame.transform.y;
      ai_xform.a3 *= scale_frame.transform.z; ai_xform.b3 *= scale_frame.transform.z; ai_xform.c3 *= scale_frame.transform.z;
      ai_xform.a4 = pos_frame.transform.x; ai_xform.b4 = pos_frame.transform.y; ai_xform.c4 = pos_frame.transform.z;

      // create local transform from above
      result.transform = ai::to_mat4(ai_xform);

      return result;
   }


   void for_each_mesh(aiScene const & scene, aiNode const & node, glm::mat4 const & parent_transform, std::function<void(aiMesh const &, glm::mat4 const &)> callback) {
      auto transform = parent_transform * to_mat4(node.mTransformation);
      for (auto idx = 0U; idx < node.mNumMeshes; idx++) {
         callback(*scene.mMeshes[node.mMeshes[idx]], transform);
      }

      for (auto idx = 0U; idx < node.mNumChildren; idx++) {
         for_each_mesh(scene, *node.mChildren[idx], transform, callback);
      }
   }

   bounds_t find_mesh_bounds(aiMesh const & m) {
      if (m.mNumVertices == 0) return{};

      aiVector3D l = m.mVertices[0];
      aiVector3D u = m.mVertices[0];
      for (auto i = 0U; i < m.mNumVertices; i++) {
         auto & v = m.mVertices[i];
         if (v.x < l.x) l.x = v.x;
         if (v.y < l.y) l.y = v.y;
         if (v.z < l.z) l.z = v.z;
         if (v.x > u.x) u.x = v.x;
         if (v.y > u.y) u.y = v.y;
         if (v.z > u.z) u.z = v.z;
      }
      return{ { l.x, l.y, l.z }, { u.x, u.y, u.z } };
   };

   double animation_secs_to_ticks(aiAnimation const & animation, double secs) {
      auto ticks_per_sec = animation.mTicksPerSecond != 0.0 ? animation.mTicksPerSecond : 25.0;
      auto animation_time_ticks = secs * ticks_per_sec;
      animation_time_ticks = (animation.mDuration > 0.) ? std::fmod(animation_time_ticks, animation.mDuration) : 0.;

      return animation_time_ticks;
   }
} // namespace ai
} // namespace glpp
