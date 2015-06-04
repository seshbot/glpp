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

   //
   // animation_t::impl implementation
   //

   struct animation_t::impl {
   public:
      impl(aiScene const & scene)
         : ai_scene(&scene)
         , name_("default")
         , ticks_per_second(0)
         , duration_ticks(0)
         , root_node(nullptr)
      {
      }

      impl(aiScene const & scene, aiAnimation const & animation)
         : ai_scene(&scene)
         , name_(ai::get_name(animation))
         , ticks_per_second(animation.mTicksPerSecond)
         , duration_ticks(animation.mDuration)
         , root_node(nullptr)
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
         std::map<std::string, ai::node_animation_t*> nodes_by_name;
         auto lookup_node_anim = [&](aiNode const * node)-> ai::node_animation_t * {
            if (!node) return{};
            auto it = nodes_by_name.find(std::string{ node->mName.C_Str() });
            assert(it != nodes_by_name.end());
            return it->second;
         };
         auto lookup_bone_anim = [&](aiBone const * bone)-> ai::node_animation_t * {
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
               std::vector<ai::mesh_animation_t::bone_t> bones;
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

      std::string name() const {
         return name_;
      }

      //aiAnimation const * ai_animation;
      aiScene const * ai_scene;

      std::string name_;
      double ticks_per_second;
      double duration_ticks;
      ai::node_animation_t const * root_node;
      glm::mat4 global_inverse_transform;
      std::vector<ai::node_animation_t> nodes; // storage for node anim hierarchy
      std::vector<ai::mesh_animation_t> mesh_animations;
   };


   namespace {
      template <typename T>
      std::size_t make_hash(T t) {
         return std::hash<T>()(t);
      }

      template <typename T, typename U>
      std::size_t make_hash(T t, U u) {
         auto seed = make_hash(t);
         // from http://www.boost.org/doc/libs/1_37_0/doc/html/hash/reference.html#boost.hash_combine
         return seed ^ make_hash(u) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
   }


   //
   // animation_timeline_t::impl implementation
   //

   struct animation_timeline_t::impl {
      impl(animation_t::impl const & animation_in, double time_secs)
         : name_(animation_in.name())
         , ticks_per_second(animation_in.ticks_per_second)
         , duration_ticks(animation_in.duration_ticks)
         , current_time_secs(time_secs)
      {
         // not animated
         if (!animation_in.root_node) {
            auto & ai_scene = *animation_in.ai_scene;

            // TODO: incorporate global inverse transform here? (third parameter)
            ai::for_each_mesh(ai_scene, *ai_scene.mRootNode, {}, [&](aiMesh const & mesh, glm::mat4 const & transform) {
               meshes.push_back({ ai_scene, mesh, transform });
            });
         }
         else {
            auto animation_time_ticks = ai::animation_secs_to_ticks(ticks_per_second, duration_ticks, time_secs);

            //
            // create node animation snapshots
            //

            node_snapshots.reserve(animation_in.nodes.size());
            std::map<std::string, ai::node_animation_timeline_t const *> node_snapshots_by_name;

            std::function<ai::node_animation_timeline_t const *(ai::node_animation_t const &, ai::node_animation_timeline_t const *)> create_and_add_snapshot_recursive = [&](ai::node_animation_t const & n, ai::node_animation_timeline_t const * parent) {
               node_snapshots.emplace_back(n, parent, animation_in.global_inverse_transform, animation_time_ticks, duration_ticks);
               auto * new_snapshot = &node_snapshots.back();

               auto ins = node_snapshots_by_name.insert(std::make_pair(new_snapshot->name(), new_snapshot));
               assert(ins.second && "node snapshot already existed with this name");

               for (auto & child : n.children) {
                  auto * new_child = create_and_add_snapshot_recursive(*child, new_snapshot);
                  new_snapshot->children.push_back(new_child);
               }

               return new_snapshot;
            };

            root_node_snapshot = create_and_add_snapshot_recursive(*animation_in.root_node, nullptr);

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
                  node.mesh_bone_snapshots.push_back({ m->ai_mesh, node, bones, bone_nodes, bone_offsets,{} });
               }
            }

            auto & ai_scene = *animation_in.ai_scene;
            for_each_mesh([&](ai::mesh_bone_snapshot_t const & mesh_bone_snapshots) {
               // aiScene const & scene, aiMesh const & mesh, glm::mat4 const & default_transform, std::vector<glm::mat4> const & bone_transforms
               auto & mesh = mesh_bone_snapshots.ai_mesh;
               auto & bone_transforms = mesh_bone_snapshots.bone_transforms;
               meshes.push_back({ ai_scene, mesh, bone_transforms });
            });

            advance_to(time_secs);
         }
      }

      std::string const & name() const {
         return name_;
      }

      void advance_to(double time_secs) {
         current_time_secs = time_secs;
         auto animation_time_ticks = ai::animation_secs_to_ticks(ticks_per_second, duration_ticks, time_secs);

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

      std::string name_;
      double ticks_per_second;
      double duration_ticks;
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


   //
   // scene_t::impl implementation
   //

   struct scene_t::impl {
      impl(scene_t const & concept, aiScene const * ai_scene)
         : ai_scene_(ai_scene)
         , default_animation_(concept, *ai_scene) {
         animations_.reserve(ai_scene->mNumAnimations);

         for (auto idx = 0U; idx < ai_scene->mNumAnimations; idx++) {
            animations_.push_back({ concept, *ai_scene, *ai_scene->mAnimations[idx], idx });
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

      animation_t const & animation(unsigned idx) const {
         return animations_[idx];
      }

      std::unique_ptr<const aiScene> ai_scene_;
      animation_t default_animation_;
      std::vector<animation_t> animations_;
      std::vector<mesh_t> meshes_;
   };


   //
   // animation_timeline_t implementation
   //

   animation_timeline_t::animation_timeline_t(animation_timeline_t &&) = default;
   animation_timeline_t & animation_timeline_t::operator=(animation_timeline_t &&) = default;

   animation_timeline_t::animation_timeline_t(animation_t const & animation, double time_secs)
      : animation_(&animation)
      , impl_{ new impl{ *animation.impl_, time_secs } } {
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

   std::vector<mesh_t> const & animation_timeline_t::meshes() const {
      return impl_->meshes;
   }

   double animation_timeline_t::current_time_secs() const {
      return impl_->current_time_secs;
   }


   //
   // animation_t impelmentation
   //

   animation_t::animation_t(scene_t const & scene, aiScene const & ai_scene)
      : scene_idx_(DEFAULT_ANIMATION_IDX)
      , id_(make_hash(&scene.impl_->ai_scene_))
      , scene_id_(scene.id())
      , impl_(new impl(ai_scene)) {
   }

   animation_t::animation_t(scene_t const & scene, aiScene const & ai_scene, aiAnimation const & ai_animation, unsigned scene_idx)
   : scene_idx_(scene_idx)
   , id_(make_hash(&scene.impl_->ai_scene_, &ai_animation))
   , scene_id_(scene.id())
   , impl_(new impl(ai_scene, ai_animation)) {
   }

   animation_t::animation_t(animation_t &&) = default;
   animation_t & animation_t::operator=(animation_t &&) = default;
   animation_t::~animation_t() = default;

   std::string animation_t::name() const {
      return impl_->name();
   }

   animation_timeline_t animation_t::create_timeline() const {
      return{ *this, 0. };
   }

   //
   // scene_t implementation
   //

   scene_t::scene_t(scene_t && other) = default; // : impl_(std::move(other.impl_)), id_(other.id_) {}
   scene_t & scene_t::operator=(scene_t && other) = default; // { impl_ = std::move(other.impl_); id_ = other.id_; return *this; }

   scene_t::scene_t(aiScene const * ai_scene)
      : id_{ make_hash(ai_scene) }
      , impl_{new impl {*this, ai_scene} } {
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

   animation_t const & scene_t::animation(unsigned idx) const {
      return impl_->animation(idx);
   }

   animation_t const & scene_t::animation(std::string const & name) const {
      return animation(impl_->animation_idx(name));
   }

   animation_t const & scene_t::default_animation() const {
      return impl_->default_animation_;
   }

   //
   // mesh_t implementation
   //

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
} // namespace glpp
