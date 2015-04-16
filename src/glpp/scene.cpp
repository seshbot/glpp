#include <glpp/scene.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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

      frame_info_t calculate_animation_frame(aiNodeAnim const & ai_anim, double time_ticks, double time_ticks_total) {
         frame_info_t result;

         // interpolate key frame from pos, rot and scale keys
         auto rot_frame = make_frame<aiQuaternion>(ai_anim.mNumRotationKeys, ai_anim.mRotationKeys, time_ticks, time_ticks_total);
         auto pos_frame = make_frame<aiVector3D>(ai_anim.mNumPositionKeys, ai_anim.mPositionKeys, time_ticks, time_ticks_total);
         auto scale_frame = make_frame<aiVector3D>(ai_anim.mNumScalingKeys, ai_anim.mScalingKeys, time_ticks, time_ticks_total);

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


   //
   // animation_t implementation
   //

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

   
   //
   // node_animation_snapshot_t implementation
   //

   void node_animation_snapshot_t::advance_transforms_to(double time_ticks_in) {
      bool restart = !(time_ticks_in > time_ticks);
      time_ticks = time_ticks_in;
      if (!node_animation.has_animation()) {
         local_transform = ai::to_mat4(node_animation.ai_node().mTransformation);
      }
      else {
         auto & ai_anim = node_animation.ai_animation();
         auto frame_info = ai::calculate_animation_frame(ai_anim, time_ticks, time_ticks_total);

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
         for (auto idx = bone_count; idx < 4; idx++) {
            bone_transforms.push_back(glm::mat4{});
         }
      }
   }

   //
   // mesh_t implementation
   //


   namespace {
      std::vector<uint32_t> get_mesh_indices(aiMesh const & m) {
         std::vector<uint32_t> indices;
         indices.reserve(m.mNumFaces * 3);
         for (auto fidx = 0U; fidx < m.mNumFaces; fidx++) {
            auto & f = m.mFaces[fidx];
            for (auto i = 0U; i < f.mNumIndices; i++) {
               indices.push_back(f.mIndices[i]);
            }
         }
         return indices;
      };

      // NOTE: material colour key macro is made up of:
      //const char* pKey,
      //unsigned int type,
      //unsigned int index,
      glm::vec4 get_mesh_material_colour(aiScene const & scene, aiMesh const & mesh, const char* ai_key, unsigned ai_type, unsigned ai_idx, glm::vec4 default_colour = glm::vec4{ 1.f, 0.f, 1.f, 1.f }) {
         aiColor4D result(default_colour.x, default_colour.y, default_colour.z, default_colour.w);
         aiGetMaterialColor(scene.mMaterials[mesh.mMaterialIndex], ai_key, ai_type, ai_idx, &result);
         return{ result.r, result.g, result.b, result.a };
      }
   }

   mesh_t::material_t::material_t(aiScene const & scene, aiMesh const & mesh)
      : diffuse_colour_(get_mesh_material_colour(scene, mesh, AI_MATKEY_COLOR_DIFFUSE)) {
   }

   mesh_t::mesh_t(aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform, std::vector<glm::mat4> const & bone_transforms)
      : ai_mesh_{ &mesh }
      , default_transform_(default_transform)
      , bone_transforms_(bone_transforms)
      , material_{ scene, mesh }
      , indices_(get_mesh_indices(mesh))
      , bone_indices_(mesh.mNumVertices)
      , bone_weights_(mesh.mNumVertices)
   {
      std::vector<unsigned> vertex_bone_counts(mesh.mNumVertices);

      for (auto bone_idx = 0U; bone_idx < mesh.mNumBones; bone_idx++) {
         auto & bone = *mesh.mBones[bone_idx];
         for (auto weight_idx = 0U; weight_idx < bone.mNumWeights; weight_idx++) {
            auto & weight = bone.mWeights[weight_idx];
            auto array_idx = vertex_bone_counts[weight.mVertexId]++;
            assert(array_idx < 4 && "vertex has more than 4 bones");
            bone_indices_[weight.mVertexId][array_idx] = (float)bone_idx;
            bone_weights_[weight.mVertexId][array_idx] = weight.mWeight;
         }
      }
   }

   unsigned mesh_t::bone_count() const { return ai_mesh_->mNumBones; }

   mesh_t::buffer_desc_t<float> mesh_t::vertices() const { return{ reinterpret_cast<float*>(ai_mesh_->mVertices), ai_mesh_->mNumVertices * 3 }; }
   mesh_t::buffer_desc_t<uint32_t> mesh_t::indices() const { return{ indices_.data(), indices_.size() }; }
   mesh_t::buffer_desc_t<float> mesh_t::normals() const {
      assert(ai_mesh_->HasNormals());
      return{ reinterpret_cast<float*>(ai_mesh_->mNormals), ai_mesh_->mNumVertices * 3 };
   }
   mesh_t::buffer_desc_t<float> mesh_t::bone_indices() const { return{ bone_indices_.data()->data(), bone_indices_.size() * 4 }; }
   mesh_t::buffer_desc_t<float> mesh_t::bone_weights() const { return{ bone_weights_.data()->data(), bone_weights_.size() * 4 }; }


   //
   // animation_snapshot_t implementation
   //

   namespace {
      double secs_to_ticks(animation_t const & animation, double secs) {
         auto ticks_per_sec = animation.ai_animation->mTicksPerSecond != 0.0 ? animation.ai_animation->mTicksPerSecond : 25.0;
         auto animation_time_ticks = secs * ticks_per_sec;
         animation_time_ticks = (animation.ai_animation->mDuration > 0.) ? std::fmod(animation_time_ticks, animation.ai_animation->mDuration) : 0.;

         return animation_time_ticks;
      }
   }

   animation_snapshot_t::animation_snapshot_t(animation_t const & animation_in, double time_secs)
      : animation(&animation_in)
   {
      auto animation_time_ticks = secs_to_ticks(*animation, time_secs);

      //
      // create node animation snapshots
      //

      node_snapshots.reserve(animation->nodes.size());
      std::map<std::string, node_animation_snapshot_t const *> node_snapshots_by_name;

      std::function<node_animation_snapshot_t const *(node_animation_t const &, node_animation_snapshot_t const *)> create_and_add_snapshot_recursive = [&](node_animation_t const & n, node_animation_snapshot_t const * parent) {
         node_snapshots.emplace_back(n, parent, animation->global_inverse_transform, animation_time_ticks, animation->ai_animation->mDuration);
         auto * new_snapshot = &node_snapshots.back();

         auto ins = node_snapshots_by_name.insert(std::make_pair(new_snapshot->name(), new_snapshot));
         assert(ins.second && "node snapshot already existed with this name");

         for (auto & child : n.children) {
            auto * new_child = create_and_add_snapshot_recursive(*child, new_snapshot);
            new_snapshot->children.push_back(new_child);
         }

         return new_snapshot;
      };

      root_node_snapshot = create_and_add_snapshot_recursive(*animation->root_node, nullptr);

      //
      // create mesh/bone structure (node.mesh_animations) for each animation node
      //

      for (auto & node : node_snapshots) {
         for (auto & m : node.node_animation.mesh_animations()) {
            std::vector<aiBone const *> bones;
            std::vector<node_animation_snapshot_t const *> bone_nodes;
            std::vector<glm::mat4> bone_offsets;
            for (auto & b : m->bones) {
               auto it = node_snapshots_by_name.find(b.node.name());
               assert(it != node_snapshots_by_name.end());
               auto bone_node = it->second;
               bones.push_back(&b.ai_bone);
               bone_nodes.push_back(bone_node);
               bone_offsets.push_back(ai::to_mat4(b.ai_bone.mOffsetMatrix));
            }
            node.mesh_bone_snapshots.push_back({ m->ai_mesh, node, bones, bone_nodes, bone_offsets, {} });
         }
      }

      auto & scene = *animation->ai_scene;
      for_each_mesh([&](glpp::mesh_bone_snapshot_t const & mesh_bone_snapshots){
         // aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform, std::vector<glm::mat4> const & bone_transforms
         auto & mesh = mesh_bone_snapshots.ai_mesh;
         auto & bone_transforms = mesh_bone_snapshots.bone_transforms;
         meshes.push_back({ scene, mesh, glm::mat4{}, bone_transforms });
      });

      advance_to(time_secs);
   }

   void animation_snapshot_t::advance_to(double time_secs) {
      auto animation_time_ticks = secs_to_ticks(*animation, time_secs);

      // advance all nodes first
      for (auto & node : node_snapshots) {
         node.advance_transforms_to(animation_time_ticks);
      }

      // re-evaluate bone transforms for each node snapshot
      for (auto & node : node_snapshots) {
         node.recalc_bones();
      }

      // collect the bone transforms together for easy debug printing of bones
      node_bone_transforms_.clear();
      node_bone_names_.clear();

      for (auto & node : node_snapshots) {
         std::map<std::string, glm::mat4> bone_transforms_by_name;

         // first create a name->xform map of all bones in the node
         for (auto & mesh_bones : node.mesh_bone_snapshots) {
            auto bone_count = mesh_bones.bone_nodes.size();
            for (auto idx = 0U; idx < bone_count; idx++) {
               auto bone_node_name = mesh_bones.bone_nodes[idx]->name();
               auto & bone_transform = mesh_bones.bone_transforms[idx];
               bone_transforms_by_name[bone_node_name] = bone_transform;
            }
         }

         // do nothing if this node has no bones
         if (bone_transforms_by_name.size() == 0) continue;

         // then store the bone name and transform in the member variables
         auto node_name = node.name();
         assert(node_bone_transforms_.find(node_name) == node_bone_transforms_.end());

         auto & transforms = node_bone_transforms_[node_name];
         auto & names = node_bone_names_[node_name];
         for (auto & bone_name_pair : bone_transforms_by_name) {
            transforms.push_back(bone_name_pair.second);
            names.push_back(bone_name_pair.first);
         }
      }
   }


   //
   // scene_t implementation
   //

   scene_t::scene_t(aiScene const * ai_scene)
      : ai_scene_(ai_scene) {
      animations_.reserve(ai_scene->mNumAnimations);

      for (auto idx = 0U; idx < ai_scene->mNumAnimations; idx++) {
         animations_.push_back({ *ai_scene, *ai_scene->mAnimations[idx] });
         // to snapshot: animation_snapshot_t(animation_t const & animation, double time_secs)
      }

      ai::log_scene_info(*ai_scene);

      utils::log(utils::LOG_INFO, "== Animation Hierarchies ==\n");
      for (auto & animation : animations_) {
         utils::log(utils::LOG_INFO, " - animation %s (%d animated nodes)\n", animation.name().c_str(), animation.nodes.size());
         glpp::log_animation_nodes(*animation.root_node, "   - ");
      }
   }

   scene_t::~scene_t() = default;

   scene_t scene_t::load_from_file(std::string const & filename) {
      Assimp::Importer importer;
      auto * scene = importer.ReadFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
      if (!scene) {
         auto error = std::string("could not import scene: ") + importer.GetErrorString();
         throw std::runtime_error(error.c_str());
      }

      // ensure importer doesnt delete the scene!
      importer.GetOrphanedScene();

      utils::log(utils::LOG_INFO, "scene imported: %d meshes, %d materials, %d textures, %d animations\n",
         scene->mNumMeshes,
         scene->mNumMaterials,
         scene->mNumTextures,
         scene->mNumAnimations
         );

      return{ scene };
   }

   std::vector<std::string> scene_t::animation_names() const {
      std::vector<std::string> result;
      for (auto & a : animations_) {
         result.push_back(a.name());
      }
      return result;
   }

   animation_t const & scene_t::animation(std::string name) const {
      for (auto & a : animations_) {
         if (a.name() == name) return a;
      }
      throw std::runtime_error("animation '" + name + "' not found in scene");
   }

} // namespace glpp
