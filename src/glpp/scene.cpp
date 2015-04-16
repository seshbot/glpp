#include <glpp/scene.h>

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
   }


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

} // namespace glpp
