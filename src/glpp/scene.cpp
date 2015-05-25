#include <glpp/scene.h>

#include <glpp/utils.h>

#include <glm/gtx/matrix_operation.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

#include <glpp/utils.h>

#include "detail/ai_helpers.h"
#include "detail/ai_structures.h"

#include <map>


namespace glpp {
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

      float get_mesh_material_shininess(aiScene const & scene, aiMesh const & mesh, float default_value = 0.f) {
         float shininess = 0.f;
         float shininess_strength = 0.f;
         if ( aiGetMaterialFloat(scene.mMaterials[mesh.mMaterialIndex], AI_MATKEY_SHININESS, &shininess) &&
              aiGetMaterialFloat(scene.mMaterials[mesh.mMaterialIndex], AI_MATKEY_SHININESS_STRENGTH, &shininess_strength)) {
            return shininess * shininess_strength;
         }
         return default_value;
      }

      mesh_t::material_t get_mesh_material(aiScene const & scene, aiMesh const & mesh) {
         return{
            get_mesh_material_colour(scene, mesh, AI_MATKEY_COLOR_DIFFUSE),
            get_mesh_material_shininess(scene, mesh)
         };
      }
   }

   mesh_t::mesh_t(aiScene const & scene, aiMesh const & mesh, std::vector<glm::mat4> const & bone_transforms)
      : ai_mesh_{ &mesh }
      , is_animated_{ true }
      , default_transforms_()
      , bone_transforms_(&bone_transforms)
      , material_(get_mesh_material(scene, mesh))
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

   mesh_t::mesh_t(aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform)
      : ai_mesh_{ &mesh }
      , is_animated_{ false }
      , default_transforms_{ default_transform }
      , bone_transforms_(nullptr)
      , material_(get_mesh_material(scene, mesh))
      , indices_(get_mesh_indices(mesh))
      , bone_indices_(mesh.mNumVertices)
      , bone_weights_(mesh.mNumVertices)
   {
      // all vertices point to a single bone transform - the default transform for this mesh
      for (auto idx = 0U; idx < mesh.mNumVertices; idx++) {
         bone_indices_[idx][0] = 0.;
         bone_weights_[idx][0] = 1.;
      }
   }

   std::vector<glm::mat4> const & mesh_t::bone_transforms() const {
      if (is_animated_) {
         return *bone_transforms_;
      }
      else {
         return default_transforms_;
      }
   }

   std::string mesh_t::name() const { return ai::get_name(*ai_mesh_); }
   unsigned mesh_t::bone_count() const { return ai_mesh_->mNumBones; }

   mesh_t::buffer_desc_t<float> mesh_t::vertices() const { return{ reinterpret_cast<float*>(ai_mesh_->mVertices), ai_mesh_->mNumVertices * 3 }; }
   mesh_t::buffer_desc_t<uint32_t> mesh_t::indices() const { return{ indices_.data(), static_cast<uint32_t>(indices_.size()) }; }
   mesh_t::buffer_desc_t<float> mesh_t::normals() const {
      assert(ai_mesh_->HasNormals());
      return{ reinterpret_cast<float*>(ai_mesh_->mNormals), ai_mesh_->mNumVertices * 3 };
   }
   mesh_t::buffer_desc_t<float> mesh_t::bone_indices() const { return{ bone_indices_.data()->data(), static_cast<uint32_t>(bone_indices_.size() * 4) }; }
   mesh_t::buffer_desc_t<float> mesh_t::bone_weights() const { return{ bone_weights_.data()->data(), static_cast<uint32_t>(bone_weights_.size() * 4) }; }


   //
   // animation_timeline_t implementation
   //


   struct animation_timeline_t::impl {
      impl(ai::animation_t const & animation_in, scene_t const & scene, unsigned scene_idx, double time_secs)
         : animation(animation_in)
         , scene(scene)
         , scene_idx(scene_idx)
         , current_time_secs(time_secs)
      {
         auto animation_time_ticks = ai::animation_secs_to_ticks(*animation.ai_animation, time_secs);

         //
         // create node animation snapshots
         //

         node_snapshots.reserve(animation.nodes.size());
         std::map<std::string, ai::node_animation_timeline_t const *> node_snapshots_by_name;

         std::function<ai::node_animation_timeline_t const *(ai::node_animation_t const &, ai::node_animation_timeline_t const *)> create_and_add_snapshot_recursive = [&](ai::node_animation_t const & n, ai::node_animation_timeline_t const * parent) {
            node_snapshots.emplace_back(n, parent, animation.global_inverse_transform, animation_time_ticks, animation.ai_animation->mDuration);
            auto * new_snapshot = &node_snapshots.back();

            auto ins = node_snapshots_by_name.insert(std::make_pair(new_snapshot->name(), new_snapshot));
            assert(ins.second && "node snapshot already existed with this name");

            for (auto & child : n.children) {
               auto * new_child = create_and_add_snapshot_recursive(*child, new_snapshot);
               new_snapshot->children.push_back(new_child);
            }

            return new_snapshot;
         };

         root_node_snapshot = create_and_add_snapshot_recursive(*animation.root_node, nullptr);

         //
         // create mesh/bone structure (node.mesh_animations) for each animation node
         //

         for (auto & node : node_snapshots) {
            for (auto & m : node.node_animation.mesh_animations()) {
               std::vector<aiBone const *> bones;
               std::vector<ai::node_animation_timeline_t const *> bone_nodes;
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

         auto & ai_scene = *animation.ai_scene;
         for_each_mesh([&](ai::mesh_bone_snapshot_t const & mesh_bone_snapshots){
            // aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform, std::vector<glm::mat4> const & bone_transforms
            auto & mesh = mesh_bone_snapshots.ai_mesh;
            auto & bone_transforms = mesh_bone_snapshots.bone_transforms;
            meshes.push_back({ ai_scene, mesh, bone_transforms });
         });

         advance_to(time_secs);
      }

      std::string name() const {
         return animation.name();
      }

      void advance_to(double time_secs) {
         current_time_secs = time_secs;
         auto animation_time_ticks = ai::animation_secs_to_ticks(*animation.ai_animation, time_secs);

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

      void advance_by(double time_secs) {
         advance_to(current_time_secs + time_secs);
      }

      template <typename CallbackT>
      void for_each_mesh_impl(ai::node_animation_timeline_t const & node_snapshot, CallbackT callback) {
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

      scene_t const & scene;
      unsigned scene_idx;
      ai::animation_t const & animation;
      double current_time_secs;
      ai::node_animation_timeline_t const * root_node_snapshot;
      std::vector<ai::node_animation_timeline_t> node_snapshots;
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


   animation_timeline_t::animation_timeline_t(animation_timeline_t &&) = default;
   animation_timeline_t & animation_timeline_t::operator=(animation_timeline_t &&) = default;
   
   animation_timeline_t::animation_timeline_t(ai::animation_t const & animation, scene_t const & scene, unsigned scene_idx, double time_secs)
      : impl_{ new impl{animation, scene, scene_idx, time_secs} } {
   }

   animation_timeline_t::~animation_timeline_t() = default;

   std::string animation_timeline_t::name() const {
      return impl_->name();
   }

   void animation_timeline_t::advance_to(double time_secs) {
      impl_->advance_to(time_secs);
   }

   void animation_timeline_t::advance_by(double time_secs) {
      impl_->advance_by(time_secs);
   }

   scene_t const & animation_timeline_t::scene() const {
      return impl_->scene;
   }

   unsigned animation_timeline_t::scene_idx() const {
      return impl_->scene_idx;
   }

   std::vector<mesh_t> const & animation_timeline_t::meshes() const {
      return impl_->meshes;
   }

   double animation_timeline_t::current_time_secs() const {
      return impl_->current_time_secs;
   }


   //
   // scene_t implementation
   //

   struct scene_t::impl {
      impl(aiScene const * ai_scene)
         : ai_scene_(ai_scene) {
         animations_.reserve(ai_scene->mNumAnimations);

         for (auto idx = 0U; idx < ai_scene->mNumAnimations; idx++) {
            animations_.push_back({ *ai_scene, *ai_scene->mAnimations[idx] });
            // to snapshot: animation_timeline_t(animation_t const & animation, double time_secs)
         }

         ai::log_scene_info(*ai_scene);

         //utils::log(utils::LOG_INFO, "== Animation Hierarchies ==\n");
         //for (auto & animation : animations_) {
         //   utils::log(utils::LOG_INFO, " - animation %s (%d animated nodes)\n", animation.name().c_str(), animation.nodes.size());
         //   glpp::log_animation_nodes(*animation.root_node, "   - ");
         //}

         // TODO: incorporate global inverse transform here? (third parameter)
         ai::for_each_mesh(*ai_scene, *ai_scene->mRootNode, {}, [&](aiMesh const & mesh, glm::mat4 const & transform) {
            meshes_.push_back({*ai_scene, mesh, transform});
         });
      }
      
      std::vector<std::string> animation_names() const {
         std::vector<std::string> result;
         for (auto & a : animations_) {
            result.push_back(a.name());
         }
         return result;
      }

      unsigned animation_idx(std::string const & name) const {
         for (auto idx = 0U; idx < animations_.size(); idx++) {
            if (animations_[idx].name() == name) return idx;
         }
         throw std::runtime_error("animation '" + name + "' not found in scene");
      }

      ai::animation_t const & animation(unsigned idx) const {
         return animations_[idx];
      }

      std::unique_ptr<const aiScene> ai_scene_;
      std::vector<ai::animation_t> animations_;
      std::vector<mesh_t> meshes_;
   };

   scene_t::scene_t(scene_t &&) = default;
   scene_t & scene_t::operator=(scene_t&&) = default;

   namespace {
      template <typename T>
      unsigned make_hash(T t) {
         return std::hash<T>()(t);
      }
   }
   scene_t::scene_t(aiScene const * ai_scene)
      : impl_{new impl {ai_scene} }
      , id_{make_hash(ai_scene) } {
   }

   scene_t::~scene_t() = default;

   scene_t scene_t::create_from_file(std::string const & filename) {
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

      return { scene };
   }

   std::vector<mesh_t> const & scene_t::meshes() const {
      return impl_->meshes_;
   }

   std::vector<std::string> scene_t::animation_names() const {
      return impl_->animation_names();
   }

   animation_timeline_t scene_t::create_timeline(unsigned idx) const {
      return{ impl_->animation(idx), *this, idx, 0. };
   }

   animation_timeline_t scene_t::create_timeline(std::string const & name) const {
      return create_timeline(impl_->animation_idx(name));
   }

} // namespace glpp
