#ifdef _MSC_VER
#  include <windows.h>
#endif

#include <map>
#include "game.h"
#include <glpp/glpp.h>
#include <glpp/utils.h>
#ifdef _MSC_VER
#   include <glpp/gles2.h>
#else
#   include <glpp/gl2.h>
#endif

// TODO: remove this
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <cstdlib>

// TODO:
// - animate imported meshes
// - fire mesh
// - grass texture
// - pass lights in uniforms
// - day/night cycle
// - text
// - audio

namespace gl {
#ifdef _MSC_VER
   using namespace gles2;
#else
   using namespace gl2;
#endif
}

namespace {
   glpp::shader vert_shader(std::string name) { return glpp::shader::create_from_file(utils::fmt("../../shaders/%s.vert", name.c_str()), glpp::shader::Vertex); }
   glpp::shader frag_shader(std::string name) { return glpp::shader::create_from_file(utils::fmt("../../shaders/%s.frag", name.c_str()), glpp::shader::Fragment); }

   glpp::program create_program(std::string name) {
      utils::log(utils::LOG_INFO, "compiling '%s' shader programs... ", name.c_str());

      auto program = glpp::program{ vert_shader(name), frag_shader(name) };

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }

      return program;
   };

   void reload_program(glpp::program & program, std::string name) {
      utils::log(utils::LOG_INFO, "reloading '%s' shader programs... ", name.c_str());

      program.reload(vert_shader(name), frag_shader(name));

      utils::log(utils::LOG_INFO, "success\n");

      auto logs = program.compile_logs();
      if (logs.length() > 0) {
         utils::log(utils::LOG_INFO, "%s", logs.c_str());
      }
   };


   // turns key presses into a directional vector. consumed by entity controller
   class player_controls_t {
   public:
      using action_func = std::function < bool(glpp::Key, glpp::KeyAction) > ;

      player_controls_t()
         : dir_flags_(0), direction_(0., 0.) {
      }

      void register_action_handler(glpp::Key k, glpp::KeyAction a, action_func f) { actions_[{k, a}] = f; }
      void register_action_handlers(std::vector<glpp::Key> ks, glpp::KeyAction a, action_func f) { for (auto k : ks) register_action_handler(k, a, f); }

      bool is_moving() const { return dir_flags_ != 0; }
      glm::vec2 const & direction() const { return direction_; }

      void handle_key_action(glpp::Key key, glpp::KeyAction action) {
         auto action_it = actions_.find({ key, action });
         if (actions_.end() != action_it) {
            if (action_it->second(key, action)) return;
         }

         enum class dir { none = 0, up = 1, down = 2, left = 4, right = 8 };

         auto key_dir = [key] {
            switch (key) {
            case glpp::KEY_A: case glpp::KEY_LEFT: return dir::left;
            case glpp::KEY_S: case glpp::KEY_DOWN: return dir::down;
            case glpp::KEY_D: case glpp::KEY_RIGHT: return dir::right;
            case glpp::KEY_W: case glpp::KEY_UP: return dir::up;
            default: return dir::none;
            }
         }();

         if (key_dir == dir::none) {
            utils::log(utils::LOG_INFO, "key pressed: %s\n", glpp::to_string(key).c_str());
            return;
         }
         if (action == glpp::KEY_ACTION_PRESS) dir_flags_ |= static_cast<uint16_t>(key_dir);
         if (action == glpp::KEY_ACTION_RELEASE) dir_flags_ &= ~static_cast<uint16_t>(key_dir);

         float horiz_vec = 0.;
         if (dir_flags_ & static_cast<uint16_t>(dir::left)) horiz_vec -= 1.f;
         if (dir_flags_ & static_cast<uint16_t>(dir::right)) horiz_vec += 1.f;
         float vert_vec = 0.;
         if (dir_flags_ & static_cast<uint16_t>(dir::up)) vert_vec += 1.f;
         if (dir_flags_ & static_cast<uint16_t>(dir::down)) vert_vec -= 1.f;

         auto dir = glm::vec2(horiz_vec, vert_vec);
         direction_ = glm::length(dir) == 0. ? dir : glm::normalize(dir);
      }

   private:
      std::uint16_t dir_flags_;
      glm::vec2 direction_;
      using action_key = std::pair < glpp::Key, glpp::KeyAction > ;
      std::map<action_key, action_func> actions_;
   };

   // TODO: lifetime (create/delete), movement (update), render (vertices, buffer) policies
   template <typename CreatePolicy, typename UpdatePolicy, typename DeletePolicy>
   class particle_emitter_buffer_t
      : public CreatePolicy   // CreatePolicy provides create_particles(emitter, t)
      , public UpdatePolicy   // UpdatePolicy provides update_particle(emitter, idx, t)
      , public DeletePolicy { // DeletePolicy provides should_delete(emitter, idx)
   public:
      using cont_t = std::vector < glm::vec3 >;
      using idx_t = std::size_t;

      glm::vec3 vel_{0., -1000., 0.};

      // see http://research.microsoft.com/pubs/70320/RealTimeRain_MSTR.pdf for rain simulation
      void create_particle() {
         auto pos = glm::vec3{
            (float)(std::rand() % 1400) - 300.f, // (float)(std::rand() % 460), // - 230.,
            400.,
            400.f -(float)(std::rand() % 1200) };// (float)(std::rand() % 350) - 250. };

         add(pos, vel_);
      }

      void update(double time_since_last) {
         current_time_ += time_since_last;
         CreatePolicy::create_particles(*this, time_since_last);
         // glm likes floats
         auto t = static_cast<float>(time_since_last);

         auto idx = idx_t(0);
         while (idx < count()) {
            UpdatePolicy::update_particle(*this, idx, t);

            if (DeletePolicy::should_delete(*this, idx)) {
               del_at(idx);
            }
            else {
               // only increment if we didnt delete an element
               idx++;
            }
         }

         if (particle_positions_.size() > 0)
            buffer_.update({ glm::value_ptr(particle_positions_[0]), particle_positions_.size() * 3 });
      }

      glpp::buffer_t buffer() { return buffer_; }

      double time() const { return current_time_; }
      idx_t count() const { return particle_velocities_.size(); }
      idx_t add(glm::vec3 pos, glm::vec3 vel) { particle_create_times_.push_back(current_time_);  particle_positions_.push_back(pos); particle_velocities_.push_back(vel); return count() - 1; }
      void del_at(idx_t idx) { del_at_(particle_create_times_, idx);  del_at_(particle_positions_, idx); del_at_(particle_velocities_, idx); }
      double create_time_at(idx_t idx) { return particle_create_times_[idx]; }
      glm::vec3 & pos_at(idx_t idx) { return particle_positions_[idx]; }
      glm::vec3 & vel_at(idx_t idx) { return particle_velocities_[idx]; }

   private:
      glpp::buffer_t buffer_ = {glpp::buffer_t::Usage::Dynamic};
      double current_time_ = 0.;
      std::vector<double> particle_create_times_;
      std::vector<glm::vec3> particle_positions_;
      std::vector<glm::vec3> particle_velocities_;

      template <typename T>
      static void del_at_(std::vector<T> & vec, idx_t idx) { auto last = vec.size() - 1; if (idx != last) std::swap(vec[idx], vec[last]); vec.erase(vec.begin() + last); }
   };

   template <unsigned CREATE_PER_SEC>
   class constant_create_policy_t {
   protected:
      const double TIME_BETWEEN_CREATE = (double)1. / CREATE_PER_SEC;
      double next_create_time_ = 0.;

      template <typename T>
      void create_particles(T & emitter, double time_since_last) {
         auto now = emitter.time();
         while (next_create_time_ < now) {
            emitter.create_particle();
            next_create_time_ += TIME_BETWEEN_CREATE;
         }
      }
   };

   class constant_update_policy_t {
   protected:
      using idx_t = std::size_t;

      template <typename T>
      void update_particle(T & emitter, idx_t idx, float time_since_last) {
         auto & pos = emitter.pos_at(idx);
         pos += emitter.vel_at(idx) * time_since_last;
      }
   };

   template <unsigned TTL_MS>
   class constant_time_delete_policy_t {
   public:
      const double TTL_SECS = TTL_MS / 1000.;
      using idx_t = std::size_t;

      template <typename T>
      bool should_delete(T & emitter, idx_t idx) {
         return emitter.time() - emitter.create_time_at(idx) > TTL_SECS;
      }
   };

   template <unsigned TTL_MS>
   class constant_depth_delete_policy_t {
   public:
      const double TTL_SECS = TTL_MS / 1000.;
      using idx_t = std::size_t;

      template <typename T>
      bool should_delete(T & emitter, idx_t idx) {
         return emitter.pos_at(idx).y <= 0.;
      }
   };

   using constant_particle_emitter_buffer_t = particle_emitter_buffer_t <
      constant_create_policy_t<6000>,
      constant_update_policy_t,
      constant_depth_delete_policy_t<0> >;
}

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

void print_node_info(aiScene const & scene, aiNode const & node, int depth = 0) {
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
      print_node_info(scene, *node.mChildren[idx], depth + 1);
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



// assimp structure:
// node (locate by name)
// - meshes and bones are different nodes - mesh nodes reference bone nodes
// - default_local_xform (overridden by animation channel transforms)
// - global_xform (derived from local_xform and parent local_xforms)

// animation
// - channel[]
//   - node (locate by node name)
//   - position_key[] (all key frames have mTime and mValue)
//   - rotation_key[]
//   - scaling_key[]
//   - local_xform(t) (derived from above key at time t. Note that keys could rotate back to beginning, allow for t overflow)

// mesh
// - primitive_types (point, line, tri - TODO: this affects size of index buffer! for now just assume tri)
// - bone[] (remember to handle if !mesh.mHasBones - set bone data to 0)
//   - offset_matrix
//   - node (locate by node name)
//   - global_xform (derived from node global_xform * offset_matrix)
//   - vertex_weight[]
//     - vertex_id
//     - weight
// - vertices[]
//   - pos (mesh.mVertices[x])
//   - bone_weights[] (generally limit to 4)
//     - bone
//     - weight
// - normals[] (indexed by vertex index)

// shader:
// attibute vec3 pos; // ... etc
// attribute int bone_ids[]; // practical to use ivec4
// attribute float bone_weights[]; // practical to use vec4
// uniform mat4 bones[];
// ...
// mat4 bone_transform = bones[bone_id[0]] * bone_weights[0]
// bone_transform += bones[bone_id[1]] * bone_weights[1]
// gl_Position = wvp * bone_transform * pos

struct node_animation_t;

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
   std::string name() const { return{ node_.mName.C_Str() }; }

   bool has_animation() const { return nullptr != animation_; }
   aiNodeAnim const & ai_animation() const {
      if (!has_animation()) throw std::runtime_error("no animation data for node");
      return *animation_;
   }
   unsigned animation_idx() const { return anim_idx_; }

   aiMatrix4x4 const & ai_default_tranform() const { return node_.mTransformation; }

   node_animation_t const * parent = nullptr;
   std::vector<node_animation_t const *> children;

   std::vector<mesh_animation_t const *> const & mesh_animations() const { return mesh_animations_; }

private:
   friend struct animation_t;

   void attach_animation(aiNodeAnim const & animation_in, unsigned anim_idx) {
      assert(!animation_ && anim_idx_ == 0 && "unexpectedly set node animation multiple times");
      animation_ = &animation_in;
      anim_idx_ = anim_idx;
      assert((animation_->mNodeName == node_.mName) && "associating wrong animation with animation node");
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
      nodes = std::move(other.nodes);
      mesh_animations = std::move(other.mesh_animations);
      return *this;
   }

   ~animation_t() = default;

   animation_t(aiScene const & scene, aiAnimation const & animation)
      : ai_animation(&animation)
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
               bones.push_back({*bone, *bone_node});
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
   }

   std::string name() const { return{ai_animation->mName.C_Str()}; }

   aiAnimation const * ai_animation;
   aiScene const * ai_scene;
   node_animation_t const * root_node;
   std::vector<node_animation_t> nodes; // storage for node anim hierarchy
   std::vector<mesh_animation_t> mesh_animations;
};


//struct scene_t {
//   struct vertex_t {
//      glm::vec3 position;
//      glm::ivec4 bone_ids;
//      glm::vec4 bone_weights;
//   };
//
//   struct bone_t {
//      glm::mat4 offset;
//   };
//   struct mesh_bones_t {
//      aiMesh const & mesh;
//      std::vector<bone_t const *> bones;
//   };
//
//   scene_t(aiScene const & scene_in)
//      : scene(scene_in)
//   {
//      animations.reserve(scene.mNumAnimations);
//
//      for (auto idx = 0U; idx < scene.mNumAnimations; idx++) {
//         animations.emplace_back(scene, *scene.mAnimations[idx]);
//      }
//   }
//
//   aiScene const & scene;
//   std::vector<animation_t> animations;
//
//   std::vector<mesh_bones_t> meshes;
//   std::vector<bone_t> bones;
//};

struct node_animation_snapshot_t {
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
   static frame<ResultTransformT> make_frame(unsigned num_keys, KeyT* keys, double time_ticks, double duration_ticks) {
      if (num_keys == 0) return{};

      unsigned key_index = 0;
      for (auto idx = 0U; idx < num_keys; idx++) {
         auto & key = keys[idx];
         if (key.mTime < time_ticks) {
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

      return{key_index, frame_xform};
   }

   node_animation_snapshot_t(node_animation_t const & node_animation_in, node_animation_snapshot_t const * parent_in, double time_ticks_in, double time_ticks_total)
      : node_animation(node_animation_in)
      , parent(parent_in)
      , time_ticks(time_ticks_in)
   {
      if (!node_animation.has_animation()) {
         local_transform = to_mat4(node_animation.ai_default_tranform());
      }
      else {
         auto & ai_anim = node_animation.ai_animation();

         // interpolate key frame from pos, rot and scale keys
         auto rot_frame = make_frame<aiQuaternion>(ai_anim.mNumRotationKeys, ai_anim.mRotationKeys, time_ticks, time_ticks_total);
         auto pos_frame = make_frame<aiVector3D>(ai_anim.mNumPositionKeys, ai_anim.mPositionKeys, time_ticks, time_ticks_total);
         auto scale_frame = make_frame<aiVector3D>(ai_anim.mNumScalingKeys, ai_anim.mScalingKeys, time_ticks, time_ticks_total);

         rot_key_idx = rot_frame.key_index;
         pos_key_idx = pos_frame.key_index;
         scale_key_idx = scale_frame.key_index;

         auto ai_xform = aiMatrix4x4(rot_frame.transform.GetMatrix());
         //    aiMatrix4x4& mat = mTransforms[a];
         //    mat = aiMatrix4x4( presentRotation.GetMatrix());
         ai_xform.a1 *= scale_frame.transform.x; ai_xform.b1 *= scale_frame.transform.x; ai_xform.c1 *= scale_frame.transform.x;
         ai_xform.a2 *= scale_frame.transform.y; ai_xform.b2 *= scale_frame.transform.y; ai_xform.c2 *= scale_frame.transform.y;
         ai_xform.a3 *= scale_frame.transform.z; ai_xform.b3 *= scale_frame.transform.z; ai_xform.c3 *= scale_frame.transform.z;
         ai_xform.a4 = pos_frame.transform.x; ai_xform.b4 = pos_frame.transform.y; ai_xform.c4 = pos_frame.transform.z;

         // create local transform from above
         local_transform = to_mat4(ai_xform);
      }

      // create global_transform from parent + local
      if (!parent) {
         global_transform = local_transform;
      }
      else {
         global_transform = parent->global_transform * local_transform;
      }
   }

   std::string name() const { return node_animation.name(); }

   aiNode const & node() const { return node_animation.ai_node(); }

   node_animation_t const & node_animation;
   node_animation_snapshot_t const * parent;
   std::vector<node_animation_snapshot_t const *> children;
   double time_ticks;

   int rot_key_idx;
   int pos_key_idx;
   int scale_key_idx;
   glm::mat4 local_transform;
   glm::mat4 global_transform;

   struct mesh_animation_snapshot_t {
      aiMesh const & ai_mesh;
      // bone_nodes correlates to bone_transforms, kept separate so transforms can be passed easily to a shader
      std::vector<node_animation_snapshot_t const *> bone_nodes;
      // bone transforms are duplicated for each node snapshot so its trivial to pass to a shader
      std::vector<glm::mat4> bone_transforms;
   };
   std::vector<mesh_animation_snapshot_t> mesh_animations;
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

   animation_snapshot_t(animation_t const & animation_in, double time_secs = 0)
      : animation(&animation_in)
   {
      advance_to(time_secs);
   }

   void advance_to(double time_secs) {
      auto ticks_per_sec = animation->ai_animation->mTicksPerSecond != 0.0 ? animation->ai_animation->mTicksPerSecond : 25.0;
      auto animation_time_ticks = time_secs * ticks_per_sec;
      animation_time_ticks = (animation->ai_animation->mDuration > 0.) ? std::fmod(animation_time_ticks, animation->ai_animation->mDuration) : 0.;

      node_snapshots.reserve(animation->nodes.size());
      std::map<std::string, node_animation_snapshot_t const *> node_snapshots_by_name;

      std::function<node_animation_snapshot_t const *(node_animation_t const &, node_animation_snapshot_t const *)> create_and_add_snapshot_recursive = [&](node_animation_t const & n, node_animation_snapshot_t const * parent) {
         node_snapshots.emplace_back(n, parent, animation_time_ticks, animation->ai_animation->mDuration);
         auto * new_snapshot = &node_snapshots.back();

         auto ins = node_snapshots_by_name.insert(std::make_pair(new_snapshot->name(), new_snapshot));
         assert(ins.second && "node snapshot already existed with this name");

         for (auto & child : n.children) {
            auto * new_child = create_and_add_snapshot_recursive(*child, &node_snapshots.back());
            new_snapshot->children.push_back(new_child);
         }

         return new_snapshot;
      };

      root_node_snapshot = create_and_add_snapshot_recursive(*animation->root_node, nullptr);

      for (auto & node : node_snapshots) {
         std::map<std::string, glm::mat4> bone_transforms_by_name;

         auto node_name = node.name();
         for (auto & m : node.node_animation.mesh_animations()) {
            auto mesh_node_name = m->node->name();
            std::vector<node_animation_snapshot_t const *> bone_nodes;
            std::vector<glm::mat4> bone_transforms;
            bone_transforms.reserve(m->bones.size());
            for (auto & b : m->bones) {
               auto it = node_snapshots_by_name.find(b.node.name());
               assert(it != node_snapshots_by_name.end());
               auto bone_node = it->second;
               auto bone_node_name = bone_node->name();
               // NOTE
               auto bone_transform = bone_node->global_transform;// * to_mat4(b.ai_bone.mOffsetMatrix);
               bone_transforms.push_back(bone_transform);
               auto boneIt = bone_transforms_by_name.find(bone_node->name());
               if (boneIt == bone_transforms_by_name.end()) {
                  bone_transforms_by_name[bone_node->name()] = bone_transform;
               }
               else {
                  if (boneIt->second != bone_transform) {
                     utils::log(utils::LOG_WARN, "ASSERT FAILURE: mat1 %s\n", glm::to_string(boneIt->second).c_str());
                     utils::log(utils::LOG_WARN, "                mat2 %s\n", glm::to_string(bone_transform).c_str());

                     assert(boneIt->second == bone_transform);
                  }
               }

            }
            node.mesh_animations.push_back({m->ai_mesh, bone_nodes, bone_transforms});
         }

         assert(node_bone_transforms_.find(node_name) == node_bone_transforms_.end());
         if (bone_transforms_by_name.size() > 0) {
            auto & transforms = node_bone_transforms_[node_name];
            auto & names = node_bone_names_[node_name];
            for (auto & bone_name_pair : bone_transforms_by_name) {
               transforms.push_back(bone_name_pair.second);
               names.push_back(bone_name_pair.first);
            }
         }
      }
   }

   template <typename CallbackT>
   void for_each_mesh_impl(node_animation_snapshot_t const & node_snapshot, CallbackT callback) {
      for (auto & mesh_animation : node_snapshot.mesh_animations) {
         callback(mesh_animation.ai_mesh, mesh_animation.bone_nodes, mesh_animation.bone_transforms);
      }

       for (auto * child : node_snapshot.children) {
          for_each_mesh_impl(*child, callback);
       }
   }

   // callback void(aiMesh const & mesh, std::vector<node_animation_snapshot_t const *> const & bone_nodes, std::vector<glm::mat4> const & bone_transforms)
   // elements of bone_transforms match bone_node elements (transforms given separately so they can be used as-is for shader)
   template <typename CallbackT>
   void for_each_mesh(CallbackT callback) {
      for_each_mesh_impl(*root_node_snapshot, callback);
   }

   animation_t const * animation;
   node_animation_snapshot_t const * root_node_snapshot;
   std::vector<node_animation_snapshot_t> node_snapshots;

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




#ifdef WIN32
int CALLBACK WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR lpCmdLine,
   int nCmdShow
)
#else
int main()
#endif
{
   utils::log(utils::LOG_INFO, "starting (cwd: %s)\n", utils::getcwd().c_str());

   Assimp::Importer importer;
   auto * scene = importer.ReadFile("dude-anim.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
   if (!scene) {
      utils::log(utils::LOG_ERROR, "could not import scene: %s\n", importer.GetErrorString());
      return 0;
   }

   utils::log(utils::LOG_INFO, "scene imported: %d meshes, %d materials, %d textures, %d animations\n", 
      scene->mNumMeshes,
      scene->mNumMaterials,
      scene->mNumTextures,
      scene->mNumAnimations
   );


   auto * node = scene->mRootNode;
   if (node) {
      utils::log(utils::LOG_INFO, "== Scene Node Hierarchy ==\n");
      print_node_info(*scene, *node);
   }

   std::vector<animation_t> animations;
   animations.reserve(scene->mNumAnimations);

   std::function<void(node_animation_t const &, std::string)> print_rec = [&](node_animation_t const & n, std::string indent) {
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
         print_rec(*c, "  " + indent);
      }
   };
   utils::log(utils::LOG_INFO, "== Animation Hierarchies ==\n");
   for (auto idx = 0U; idx < scene->mNumAnimations; idx++) {
      animations.emplace_back(*scene, *scene->mAnimations[idx]);
      // to snapshot: animation_snapshot_t(animation_t const & animation, double time_secs)
      auto & animation = animations.back();
      utils::log(utils::LOG_INFO, " - animation %s (%d animated nodes)\n", animation.name().c_str(), animation.nodes.size());
      print_rec(*animation.root_node, "   - ");
   }


#if 1
   auto mat_name = [scene](unsigned idx) {
      aiString name;
      auto & m = *scene->mMaterials[idx];
      if (AI_SUCCESS != m.Get(AI_MATKEY_NAME, name)) return std::string("NONAME");
      return std::string(name.C_Str());
   };

   utils::log(utils::LOG_INFO, "== Materials ==\n");
   for (auto idx = 0U; idx < scene->mNumMaterials; idx++) {
      auto & m = *scene->mMaterials[idx];

      std::string props;
      for (auto pidx = 0U; pidx < m.mNumProperties; pidx++) {
         auto & p = *m.mProperties[pidx];
         if (props.empty()) props += " "; else props += ", "; props += p.mKey.C_Str();
      }

      utils::log(utils::LOG_INFO, " - mat%d (%s): %d props (%s)\n", idx, mat_name(idx).c_str(), m.mNumProperties, props.c_str());
   }

   utils::log(utils::LOG_INFO, "== Meshes ==\n");
   for (auto idx = 0U; idx < scene->mNumMeshes; idx++) {
      auto & m = *scene->mMeshes[idx];
      utils::log(utils::LOG_INFO, " - mesh%d: %d verts (%d%s faces), %d bones, mat %s, %d anim meshes\n",
         idx, m.mNumVertices, m.mNumFaces, m.mPrimitiveTypes == aiPrimitiveType_TRIANGLE ? " tri" : "",
         m.mNumBones, mat_name(m.mMaterialIndex).c_str(), m.mNumAnimMeshes);

      for (auto bidx = 0U; bidx < m.mNumBones; bidx++) {
         auto & b = *m.mBones[bidx];
         utils::log(utils::LOG_INFO, "   - bone%d (%s): %d weights\n", bidx, b.mName.C_Str(), b.mNumWeights);
      }
   }

   utils::log(utils::LOG_INFO, "== Animations ==\n");
   for (auto idx = 0U; idx < scene->mNumAnimations; idx++) {
      auto & a = *scene->mAnimations[idx];
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
#endif

   struct bounds_t { glm::vec3 lower; glm::vec3 upper; };
   auto find_mesh_bounds = [](aiMesh const & m) -> bounds_t {
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
      return{ {l.x, l.y, l.z}, {u.x, u.y, u.z} };
   };

   try {
      glpp::init();
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      exit(EXIT_FAILURE);
   }


   player_controls_t controls;

   try {
      bool should_reload_program = false;
      auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
         if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
            ctx.win().set_should_close();
         if (key == glpp::KEY_R && action == glpp::KEY_ACTION_PRESS)
            should_reload_program = true;

         controls.handle_key_action(key, action);
      };

      glpp::context context{ key_handler };

#ifdef USE_GLEW
      GLenum err = glewInit();
      if (err != GLEW_OK)
      {
         utils::log(utils::LOG_ERROR, "GLEW init error\n");
         exit(EXIT_FAILURE);
      }

      if (!GLEW_VERSION_2_1)
      {
         utils::log(utils::LOG_ERROR, "GLEW v2.1 not available!\n");
         exit(EXIT_FAILURE);
      }
#endif

      auto extensions = context.extensions();
      utils::log(utils::LOG_INFO, "%s\n", context.info(false).c_str());
      utils::log(utils::LOG_INFO, "GL EXTENSIONS [%s]:\n", std::to_string(extensions.size()).c_str());
      for (auto e : extensions)
         utils::log(utils::LOG_INFO, " - %s\n", e.c_str());

      gl::enable(gl::enable_cap_t::depth_test);
      gl::enable(gl::enable_cap_t::cull_face);
      //gl::enable(gl::enable_cap_t::multisample);
      gl::enable(gl::enable_cap_t::blend);

#ifndef WIN32
      gl::point_size(3.);
      gl::enable(gl::enable_cap_t::point_smooth);
#endif

      gl::blend_func(gl::blending_factor_src_t::src_alpha, gl::blending_factor_dest_t::one_minus_src_alpha);

      gl::cull_face(gl::cull_face_mode_t::back);
      
      //
      // load game data
      //

      struct player_controller : public game::world_t::player_controller_t {
         player_controls_t const & controls_;
         player_controller(player_controls_t const & controls) : controls_(controls) {}

         virtual glm::vec2 get_relative_velocity() const override {
            if (!controls_.is_moving()) return{};
            return controls_.direction();
         }
      };

      class sprite_repository : public game::world_view_t::sprite_repository_t {
      public:
         sprite_repository(game::creature_info_table const & creature_db)
            : creature_db_(creature_db)
            , player_sprite_sheet_({ "../../res/dude-walk.png" }, 64, 72)
            , bullet_sprite_sheet_({ "../../res/sprites.png" }, {
               { { 0, 608 }, { 32, 32 } },
               { { 0, 544 }, { 64, 64 } },
            })
            , player_sprite_(
            {
               { player_sprite_sheet_, { 0, 1, 2, 3, 4, 5, 6, 7 } },
               { player_sprite_sheet_, { 8, 9, 10, 11, 12, 13, 14, 15 } },
               { player_sprite_sheet_, { 16, 17, 18, 19, 20, 21, 22, 23 } },
               { player_sprite_sheet_, { 24, 25, 26, 27, 28, 29, 30, 31 } },
               { player_sprite_sheet_, { 32, 33, 34, 35, 36, 37, 38, 39 } },
               { player_sprite_sheet_, { 40, 41, 42, 43, 44, 45, 46, 47 } },
               })
            , bullet_sprite_(
            {
               { bullet_sprite_sheet_, {0} }
            })
            , big_rock_sprite_(
            {
               { bullet_sprite_sheet_, {1} }
            })
         { }

         glpp::sprite_t const & find_creature_sprite(game::creature_t const & creature) const override {
            return player_sprite_;
         }

         float creature_sprite_updating(std::size_t db_idx, game::creature_t const & creature, glpp::sprite_cursor_t & cursor, game::moment_t & moment) const override {
            auto const & plan = creature_db_.plan(db_idx);
            if (plan.type == game::plan_t::type_t::walk_on_spot) return 1.;

            if (moment.speed() < 10.) {
               cursor.set_idx(0);
               return 0.;
            }

            auto y = moment.dir().y;
            auto abs_x = std::abs(moment.dir().x);
            if (abs_x > 0.25f) {
               if (y < -.1f) cursor.set_animation_idx(1);
               else if (y > 0.1f) cursor.set_animation_idx(3);
               else cursor.set_animation_idx(2);
            }
            else if (moment.dir().y > 0.5f) cursor.set_animation_idx(4);
            else cursor.set_animation_idx(0);

            return 1.;
         }

         glpp::sprite_t const & find_particle_sprite(game::particle_t const & particle) const override {
            return bullet_sprite_;
         }

         float particle_sprite_updating(std::size_t db_idx, game::particle_t const & particle, glpp::sprite_cursor_t & cursor, game::moment_t & moment) const override {
            return 1.;
         }

      private:
         game::creature_info_table const & creature_db_;

         glpp::sprite_sheet player_sprite_sheet_;
         glpp::sprite_sheet bullet_sprite_sheet_;
         glpp::sprite_t player_sprite_;
         glpp::sprite_t bullet_sprite_;
         glpp::sprite_t big_rock_sprite_;
      };

      //
      // populate world
      //

      game::creature_info_table creature_db;
      game::particle_info_table particle_db;

      player_controller controller(controls);
      sprite_repository sprite_repository(creature_db);

      game::world_t world(creature_db, particle_db, controller);
      game::world_view_t world_view(creature_db, particle_db, sprite_repository);

      for (auto i = 0; i < 20; i++) {
         world.create_creature(game::creature_t::types::person, { game::random_world_location(), {} });
      }

      // world.create_creature(game::creature_t::types::person, { {0., 0.}, {} });

      struct sprite_render_callback_t : public glpp::pass_t::render_callback {
         sprite_render_callback_t(game::world_view_t::iterator itBegin, game::world_view_t::iterator itEnd)
         : itBegin_(itBegin)
         , itEnd_(itEnd)
         , it_(itBegin_) {
         }

         virtual bool prepare_next(glpp::program & p) const override {
            if (it_ == itEnd_) return false;

            auto & current_render_info = *it_;
            auto tex_id = current_render_info.tex_id;

            bool should_set_texture = (it_ == itBegin_) || (tex_id != current_tex_id_);
            current_tex_id_ = tex_id;

            auto & sprite = *current_render_info.sprite;
            auto & moment = *current_render_info.moment;

            p.uniform("model").set(moment.sprite_transform());

            if (should_set_texture) {
               auto sprite_tex = sprite.current_animation().texture();
               p.uniform("texture_wh").set(glm::vec2(sprite_tex.dims().x, sprite_tex.dims().y));
               glpp::texture_unit_t tex_unit{ 1 };
               tex_unit.activate();
               sprite_tex.bind();
               p.uniform("texture").set(tex_unit);
            }
            p.uniform("sprite_xy").set(sprite.current_frame().position);
            p.uniform("sprite_wh").set(sprite.current_frame().dimensions);

            it_++;
            return true;
         }

      private:
         game::world_view_t::iterator itBegin_;
         game::world_view_t::iterator itEnd_;
         mutable game::world_view_t::iterator it_;
         mutable glpp::texture_t::id_type current_tex_id_ = 0;

         sprite_render_callback_t(sprite_render_callback_t const &) {}
         sprite_render_callback_t & operator=(sprite_render_callback_t const &) { return *this; }
      };


      controls.register_action_handler(glpp::Key::KEY_SPACE, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         auto & player = creature_db.moment(world.player_id());
         auto bullet_pos = player.pos() + glm::vec2(0., 30.f) + player.dir() * 40.f;
         auto bullet_vel = player.vel() + player.dir() * 400.f;
         auto bullet_moment = game::moment_t{ bullet_pos, bullet_vel };

         auto bullet_id = world.create_particle(game::particle_t::bullet, bullet_moment, 2.f);

         return true;
      });

      bool do_special_thing = false;
      controls.register_action_handler(glpp::Key::KEY_X, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         do_special_thing = true;
         return true;
      });
      float view_height = 1.;
      controls.register_action_handlers({glpp::Key::KEY_KP_ADD, glpp::Key::KEY_EQUAL}, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view_height += .2f;
         view_height = std::min(1.f, view_height);
         return true;
      });
      controls.register_action_handlers({glpp::Key::KEY_KP_SUBTRACT, glpp::Key::KEY_MINUS}, glpp::KeyAction::KEY_ACTION_PRESS, [&](glpp::Key, glpp::KeyAction){
         view_height -= .2f;
         view_height = std::max(0.f, view_height);
         return true;
      });

      // 
      // load shaders
      //

      auto prg_3d = create_program("3d");
      auto prg_3d_particle = create_program("3d_particle");
      auto prg_3d_shadow = create_program("3d_shadow");
      auto prg_sprite = create_program("sprite");
      auto prg_post = create_program("post");


      //
      // load vertex data
      //

      static const float unit_square_verts[] = {
         -1., 1., 0., 1.,
         1., 1., 1., 1.,
         -1., -1., 0., 0.,
         1., -1., 1., 0.,
      };

      static const unsigned short unit_square_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto screen_vertices_spec = glpp::describe_buffer({ unit_square_verts, unit_square_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);

      // get a GL name for our texture
      // load data into texture
      // bind texture id to texture unit
      // bind texture unit to uniform

      // https://open.gl/textures has a good run-down

      // https://github.com/adobe/angle adobe wrapper??

      auto set_time_cb = [](glpp::uniform & u){u.set(static_cast<float>(glpp::get_time())); };
      auto set_dims_cb = [&context](glpp::uniform & u){
         auto dims = context.win().frame_buffer_dims();
         u.set(glm::vec2{ dims.x, dims.y });
      };

      auto shadow_tex = std::unique_ptr<glpp::cube_map_texture_t>();
      auto post_tex = std::unique_ptr<glpp::texture_t>();
      auto shadow_fbo = std::unique_ptr<glpp::frame_buffer_t>();
      auto tex_fbo = std::unique_ptr<glpp::frame_buffer_t>();
      auto msaa_fbo = std::unique_ptr<glpp::frame_buffer_t>();

      auto set_post_tex_cb = [&post_tex](glpp::uniform & u) { glpp::texture_unit_t tu{ 2 }; tu.activate(); post_tex->bind();  u.set(tu); };
      auto post_pass = prg_post.pass()
         .with(screen_vertices_spec)
         .set_uniform_action("texture", set_post_tex_cb)
         .set_uniform_action("t", set_time_cb);

      static const float sprite_verts[] = {
         -.5, 1., 0., 1.,
         .5, 1., 1., 1.,
         -.5, 0., 0., 0.,
         .5, 0., 1., 0.,
      };

      static const unsigned short sprite_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto sprite_vertices_spec = glpp::describe_buffer({ sprite_verts, sprite_indices })
         .attrib("p", 2)
         .attrib("tex_coords", 2);

      auto sprite_pass = prg_sprite.pass()
         .with(sprite_vertices_spec)
         .set_uniform("proj", glm::ortho<float>(0., 800., 0., 600., -1., 1.));





      auto make_mesh_vert_buffer = [](aiMesh const & m, glm::mat4 const & xform) -> glpp::buffer_t {
         auto get_mesh_indices = [](aiMesh const & m) {
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

         auto xform_vert = [&xform](float * in, float * out) {
            auto vert = xform * glm::vec4{ in[0], in[1], in[2], 1. };
            out[0] = vert.x;
            out[1] = vert.y;
            out[2] = vert.z;
         };

         auto * raw_vert_data = reinterpret_cast<float*>(m.mVertices);
         auto elem_count = m.mNumVertices * 3;

         auto indices = get_mesh_indices(m);

         //
         // apply local transform to model
         //

         std::vector<float> vert_data;
         vert_data.reserve(elem_count);
         auto * xformed_vert_data = vert_data.data();

         for (auto i = 0U; i < m.mNumVertices; i++) {
            auto data_in = raw_vert_data + (i * 3);
            auto data_out = xformed_vert_data + (i * 3);
            xform_vert(data_in, data_out);
         }

#if 0
         utils::log(utils::LOG_INFO, "\n == dumping %d verts == \n", m.mNumVertices);
         for (auto idx = 0U; idx < m.mNumVertices; idx++) {
            utils::log(utils::LOG_INFO, " %d - %.1f, %.1f, %.1f\n", idx, m.mVertices[idx].x, m.mVertices[idx].y, m.mVertices[idx].z);
         }

         auto idx = 0;
         for (auto i : indices) utils::log(utils::LOG_INFO, " %d%s", i, idx++ % 3 == 2 ? "," : "");
         utils::log(utils::LOG_INFO, "\n");
#endif

         return{
            { xformed_vert_data, elem_count },
            { indices.data(), indices.size() }
         };
      };

      auto make_mesh_normal_buffer = [](aiMesh const & m, glm::mat4 const & xform) -> glpp::buffer_t {
         assert(m.HasNormals());
         auto * raw_data = reinterpret_cast<float*>(m.mNormals);
         auto elem_count = m.mNumVertices * 3;

         //
         // apply local transform to model
         //

         std::vector<float> vert_data;
         vert_data.reserve(elem_count);
         auto * xformed_data = vert_data.data();

         auto xform_vert = [&xform](float * in, float * out) {
            auto vert = xform * glm::vec4{ in[0], in[1], in[2], 0. };
            out[0] = vert.x;
            out[1] = vert.y;
            out[2] = vert.z;
         };

         for (auto i = 0U; i < m.mNumVertices; i++) {
            auto data_in = raw_data + (i * 3);
            auto data_out = xformed_data + (i * 3);
            xform_vert(data_in, data_out);
         }

         return{
            { xformed_data, elem_count }
         };
      };


      //
      // state callbacks
      //

      auto get_view_cb = [&] {
         auto center_2d = game::center_world_location();
         auto center = glm::vec3{ center_2d.x, 0.f, -center_2d.y };
         auto eye = center + glm::vec3{ 0.f, 400.f * view_height, 400.f };
         auto result = glm::lookAt(eye, center, glm::vec3{ 0., 1., 0. });
         return result;
      };

      auto get_proj_cb = [] {
         // 0, 0 is the bottom left of the lookAt target!
         return glm::ortho<float>(-400., 400., -300., 300., -1000., 1000.);
         // return glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1000.f);
      };

      auto set_view_cb = [&get_view_cb](glpp::uniform & u) {
         u.set(get_view_cb());
      };

      auto make_shadow_pass = [&](glpp::program & program, aiScene const & scene, aiMesh const & mesh, glm::mat4 const & pos) {
         auto verts = glpp::describe_buffer(make_mesh_vert_buffer(mesh, pos))
            .attrib("p", 3);

         return program.pass()
            .with(verts);
      };

      auto make_mesh_pass = [&](glpp::program & program, aiScene const & scene, aiMesh const & mesh, glm::mat4 const & pos) {
         auto verts = glpp::describe_buffer(make_mesh_vert_buffer(mesh, pos))
            .attrib("p", 3);

         auto normals = glpp::describe_buffer(make_mesh_normal_buffer(mesh, pos))
            .attrib("normal", 3);

         aiColor4D color(1.f, 0.f, 1.f, 1.f);
         aiGetMaterialColor(scene.mMaterials[mesh.mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color);

         auto mesh_colour = glm::vec4(color.r, color.g, color.b, color.a);

         return program.pass()
            .with(verts)
            .with(normals)
            .set_uniform("colour", mesh_colour)
            .set_uniform_action("t", set_time_cb);
      };

      std::vector<glpp::pass_t> d3_shadow_passes;
      std::vector<glpp::pass_t> d3_body_passes;

      aiNode * node = scene->mRootNode;
      if (node) for_each_mesh(*scene, *node, glm::mat4{}, [&](aiMesh const & mesh, glm::mat4 const & pos) {
         // NOTE
         // TODO: get bone index in here as well
         //       create bone transform buffer - pass as uniform
         //       pass bone indices as attribute
         d3_shadow_passes.push_back(make_shadow_pass(prg_3d_shadow, *scene, mesh, pos));
         d3_body_passes.push_back(make_mesh_pass(prg_3d, *scene, mesh, pos));
      });


      static const float ground_verts[] = {
         0.,   0., -900.,   0., 1., 0.,
         800., 0., -900.,   0., 1., 0.,
         0.,   0.,  0.,     0., 1., 0.,
         800., 0.,  0.,     0., 1., 0.,
      };
      static const unsigned short ground_indices[] = {
         0, 2, 1,
         1, 2, 3,
      };

      auto ground_buffer_desc = glpp::describe_buffer({ground_verts, ground_indices})
         .attrib("p", 3)
         .attrib("normal", 3);


      auto ground_pass = prg_3d.pass()
         .with(ground_buffer_desc)
         .set_uniform("colour", glm::vec4(.8f, .8f, .16f, 1.f))
         .set_uniform("model", glm::mat4{})
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_proj_cb() * get_view_cb()); })
         .set_uniform("normal_matrix", glm::mat4{})
         .set_uniform_action("t", set_time_cb);


      constant_particle_emitter_buffer_t emitter;
      auto set_screensize_cb = [&](glpp::uniform & u) {
         auto dims = context.win().frame_buffer_dims();
         u.set(glm::vec2{dims.x, dims.y});
      };


      auto emitter_buffer_desc = glpp::describe_buffer(emitter.buffer())
         .attrib("position", 3);
      auto particle_pass = prg_3d_particle.pass()
         .with(emitter_buffer_desc)
         //.set_uniform("texture", glpp::texture_t{ "../../res/drop.png" })
         .set_uniform_action("screen_size", set_screensize_cb)
         .set_uniform("proj", glm::perspective<float>(45.f, 800.f / 600.f, 10.f, 1500.f))
         //.set_uniform("view", glm::lookAt<float>(glm::vec3{400., 200., 100.}, glm::vec3{400., 0., -300.}, glm::vec3{0., 1., 0.}));
         //.set_uniform("proj", glm::ortho<float>(0., 800., 0., 600., 0., 1000.))
         .set_uniform_action("view", set_view_cb);


      struct shadow_render_callback_t : public glpp::pass_t::render_callback {
         shadow_render_callback_t(
            game::world_view_t::iterator itBegin,
            game::world_view_t::iterator itEnd,
            glm::mat4 const & view_matrix,
            glm::mat4 const & proj_matrix)
            : itEnd_(itEnd)
            , it_(itBegin)
            , proj_view_matrix_(proj_matrix * view_matrix) {
         }

         bool prepare_next(glpp::program & p) const override {
            if (it_ == itEnd_) return false;

            auto & current_render_info = *it_;
            auto & moment = *current_render_info.moment;

            auto model_transform = moment.mesh_transform();
            p.uniform("model").set(model_transform);
            p.uniform("mvp").set(proj_view_matrix_ * model_transform);

            it_++;
            return true;
         }

      private:
         shadow_render_callback_t(shadow_render_callback_t const &) {}
         shadow_render_callback_t & operator=(shadow_render_callback_t const &) { return *this; }

         game::world_view_t::iterator itEnd_;
         mutable game::world_view_t::iterator it_;
         glm::mat4 proj_view_matrix_;
      };

      struct mesh_render_callback_t : public glpp::pass_t::render_callback {
         mesh_render_callback_t(
            game::world_view_t::iterator itBegin,
            game::world_view_t::iterator itEnd,
            glm::mat4 const & view_matrix,
            glm::mat4 const & proj_matrix)
            : itEnd_(itEnd)
            , it_(itBegin)
            , proj_view_matrix_(proj_matrix * view_matrix) {
         }

         bool prepare_next(glpp::program & p) const override {
            if (it_ == itEnd_) return false;

            auto & current_render_info = *it_;
            auto & moment = *current_render_info.moment;

            auto model_transform = moment.mesh_transform();
            p.uniform("model").set(model_transform);
            p.uniform("mvp").set(proj_view_matrix_ * model_transform);
            p.uniform("normal_matrix").set(glm::transpose(glm::inverse(model_transform)));

            it_++;
            return true;
         }

      private:
         mesh_render_callback_t(mesh_render_callback_t const &) {}
         mesh_render_callback_t & operator=(mesh_render_callback_t const &) { return *this; }

         game::world_view_t::iterator itEnd_;
         mutable game::world_view_t::iterator it_;
         glm::mat4 proj_view_matrix_;
      };


      //
      // debug diamond
      //

      // verts are the same as normals! (we can reuse the same buffer for each)
      const float diamond_mesh_verts[] = {
         0., 1., 0.,  // 0: top 
         0., 0., 1.,   1., 0., 0.,    0., 0., -1.,    -1., 0., 0.,   // 1-4: mid band
         0., -1., 0., // 5: bottom
      };

      const unsigned short diamond_mesh_indices[] = {
         0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 1, // top
         1, 5, 2,   2, 5, 3,   3, 5, 4,   4, 5, 1, // bottom
      };

      auto get_diamond_model_matrix = [] {
         auto moment = game::moment_t{ {400.f, 300.f}, {} };
         moment.set_dir({0., 1.});
         moment.rotate((float)glpp::get_time());
         return moment.mesh_transform();
      };

      auto diamond_model = get_diamond_model_matrix();

      auto diamond_vert_buffer = glpp::describe_buffer({ diamond_mesh_verts, diamond_mesh_indices })
         .attrib("p", 3);
      auto diamond_normal_buffer = glpp::describe_buffer({ diamond_mesh_verts })
         .attrib("normal", 3);
      auto diamond_pass = prg_3d.pass()
         .with(diamond_vert_buffer)
         .with(diamond_normal_buffer)
         .set_uniform("colour", glm::vec4(.1f, .8f, .2f, 1.f))
         .set_uniform_action("model", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); })
         .set_uniform_action("mvp", [&](glpp::uniform & u) { u.set(get_proj_cb() * get_view_cb() * get_diamond_model_matrix()); })
         .set_uniform_action("normal_matrix", [&](glpp::uniform & u) { u.set(get_diamond_model_matrix()); });



      auto animation_snapshot = animation_snapshot_t{ animations[0], 0. };
      utils::log(utils::LOG_INFO, "== Animation '%s' Meshes ==\n", animation_snapshot.animation->name().c_str());

      animation_snapshot.for_each_mesh([](aiMesh const & mesh, std::vector<node_animation_snapshot_t const *> const & bone_nodes, std::vector<glm::mat4> const & bone_transforms) {
         utils::log(utils::LOG_INFO, " - mesh %s: %d bones, %d transforms\n", mesh.mName.C_Str(), bone_nodes.size(), bone_transforms.size());
      });

      std::string bone_node_names;
      for (auto & name : animation_snapshot.node_names_with_bones()) {
         if (bone_node_names.length() > 0) bone_node_names += ", ";
         auto bone_node_count = animation_snapshot.node_bone_transforms(name).size();
         bone_node_names += name + "[" + std::to_string(bone_node_count) + "]";
      }
      utils::log(utils::LOG_INFO, "== Nodes with bones: %s\n", bone_node_names.c_str());

      std::string dudebody_bones;
      int bone_idx = 0;
      for (auto name : animation_snapshot.node_bone_names("DudeBody")) {
         if (dudebody_bones.length() > 0) dudebody_bones += ", ";
         dudebody_bones += name + "[" + std::to_string(bone_idx) + "]";
      }
      utils::log(utils::LOG_INFO, "== DudeBody bones %s\n", dudebody_bones.c_str());

      struct bone_render_callback_t : public glpp::pass_t::render_callback {
         bone_render_callback_t(
            animation_snapshot_t const & snapshot,
            glm::mat4 const & view_matrix,
            glm::mat4 const & proj_matrix)
            : snapshot_(snapshot)
            , itBegin_(snapshot.node_bone_transforms("DudeBody").begin())
            , itEnd_(snapshot.node_bone_transforms("DudeBody").end())
            , it_(itBegin_)
            , proj_view_matrix_(proj_matrix * view_matrix) {
               auto & names = snapshot.node_bone_names("DudeBody");
               names_.assign(names.begin(), names.end());
         }
         std::vector<std::string> names_;

         bool prepare_next(glpp::program & p) const override {
            if (it_ == itEnd_) return false;

            auto size = std::distance(itBegin_, itEnd_);
            auto time = glpp::get_time();

            // NOTE
            auto world_transform = glm::translate(glm::vec3{ 400.f, 0., -300.f }) * glm::scale(glm::vec3{ 100. });
            auto local_transform = glm::scale(glm::vec3{ .2f });

            auto model_transform = world_transform * *it_ * local_transform;
            
            auto dist = std::distance(itBegin_, it_);

            static int last_mesh_to_show = -1;
            static auto start_time = time;
            auto mesh_to_show = (int)(time - start_time) % size;
            if (last_mesh_to_show != mesh_to_show && dist == mesh_to_show) {
               utils::log(utils::LOG_INFO, "** SHOWING MESH %:10s %d/%d (%s)\n", names_[mesh_to_show].c_str(), mesh_to_show, size, glm::to_string(*it_ * glm::vec4{0., 0., 0., 1.}).c_str());
               last_mesh_to_show = mesh_to_show;
            }

            auto r = 0.f;
            auto g = 0.f;
            auto b = 0.f;
            auto a = 1.f; // (dist == mesh_to_show) ? 1.f : 0.;

            if (dist < 3) {
               r = 0.5f * dist;
            } else if (dist < 5) {
               g = 0.5f * (dist - 2);
            } else if (dist < 7) {
               b = 0.5f * (dist - 4);
            } else {
               r = 1.f;
               b = 1.f;
            }
            if (dist == mesh_to_show) {
               r = 1.f;
               g = 1.f;
               b = 0.f;
            }

            p.uniform("colour").set(glm::vec4(r, g, b, a));
            p.uniform("model").set(model_transform);
            p.uniform("mvp").set(proj_view_matrix_ * model_transform);
            p.uniform("normal_matrix").set(glm::transpose(glm::inverse(model_transform)));

            it_++;
            return true;
         }

      private:
         bone_render_callback_t(bone_render_callback_t const & s) : snapshot_(s.snapshot_) { }
         bone_render_callback_t & operator=(bone_render_callback_t const &) { return *this; }

         animation_snapshot_t const & snapshot_;
         std::vector<glm::mat4>::const_iterator itBegin_;
         std::vector<glm::mat4>::const_iterator itEnd_;
         mutable std::vector<glm::mat4>::const_iterator it_;
         glm::mat4 proj_view_matrix_;
      };


      auto bone_pass = prg_3d.pass()
         .with(diamond_vert_buffer)
         .with(diamond_normal_buffer);

      //
      // game loop
      //

      double last_tick = glpp::get_time();

      while (!context.win().closing())
      {
         if (should_reload_program) {
            try {
               ::reload_program(prg_3d, "3d");
               ::reload_program(prg_3d_particle, "3d_particle");
               ::reload_program(prg_3d_shadow, "3d_shadow");
               ::reload_program(prg_post, "post");
               ::reload_program(prg_sprite, "sprite");
            }
            catch (glpp::shader_compile_error const & ex) {
               utils::log(utils::LOG_ERROR, "%s\n", ex.what());
               utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());
            }
            should_reload_program = false;
         }

         auto dims = context.win().frame_buffer_dims();

         const int shadow_texture_width = 100;
         auto shadow_texture_dims = glpp::dim_t{ shadow_texture_width, shadow_texture_width };

#ifdef WIN32 
         const glpp::texture_format_t tex_fmt = glpp::texture_format_t::BGRA;
#else
         const glpp::texture_format_t tex_fmt = glpp::texture_format_t::RGBA;
#endif
         if (!shadow_fbo || shadow_fbo->dims() != shadow_texture_dims) {
            shadow_tex.reset(new glpp::cube_map_texture_t(shadow_texture_width, tex_fmt));
            shadow_fbo.reset(new glpp::frame_buffer_t(*shadow_tex));
         }
         if (!tex_fbo || tex_fbo->dims() != dims) {
            post_tex.reset(new glpp::texture_t(dims, tex_fmt));
            tex_fbo.reset(new glpp::frame_buffer_t(*post_tex));
         }
         if (!msaa_fbo || msaa_fbo->dims() != dims) {
            msaa_fbo.reset(new glpp::frame_buffer_t(dims, 8));
         }

         double this_tick = glpp::get_time();
         double time_since_last_tick = this_tick - last_tick;

         const double MAX_TICK_SECONDS = 1. / 15.;
         if (time_since_last_tick > MAX_TICK_SECONDS) time_since_last_tick = MAX_TICK_SECONDS;

         //
         // update world
         //

         world.update(time_since_last_tick);
         world_view.update(time_since_last_tick);
         emitter.update(time_since_last_tick);


         //
         // render
         //

         // gl::clear_color(.7f, .87f, .63f, 1.);
         gl::clear_color(1.f, 1.f, 1.f, 1.);
         gl::clear(
            gl::clear_buffer_flags_t::color_buffer_bit |
            gl::clear_buffer_flags_t::depth_buffer_bit);


         //
         // draw to shadow texture
         //

         // const PositionalLight c_light1 = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), vec3(.9, .8, .1), .1);
         struct face_info {
            glpp::frame_buffer_t::CubeFace face;
            glm::vec3 view_direction;
            glm::vec3 up_direction;
            std::string name;
         };

         const face_info faces[6] = {
            { glpp::frame_buffer_t::POSITIVE_X, { 1., 0., 0. }, { 0., -1., 0. }, "x_pos" },
            { glpp::frame_buffer_t::NEGATIVE_X, { -1., 0., 0. }, { 0., -1., 0. }, "x_neg" },
            { glpp::frame_buffer_t::POSITIVE_Y, { 0., 1., 0. }, { 0., 0., 1. }, "y_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Y, { 0., -1., 0. }, { 0., 0., -1. }, "y_neg" },
            { glpp::frame_buffer_t::POSITIVE_Z, { 0., 0., 1. }, { 0., -1., 0. }, "z_pos" },
            { glpp::frame_buffer_t::NEGATIVE_Z, { 0., 0., -1. }, { 0., -1., 0. }, "z_neg" },
         };

         const auto light_pos = glm::vec3{ 400., 30., -300. };
         const auto light_proj = glm::perspective((float)glm::radians(90.), 1.f, 10.f, 400.f);
         const auto light_view = glm::lookAt(light_pos, light_pos + faces[0].view_direction, faces[0].up_direction);

         gl::disable(gl::enable_cap_t::blend);
         gl::cull_face(gl::cull_face_mode_t::front);
         gl::clear_color(1., 1., 1., 1.);

         // HOLY CRAP this took me ages to figure out people
         gl::viewport(0, 0, shadow_texture_width, shadow_texture_width);

         for (auto face_idx = 0; face_idx < 6; face_idx++) {
            auto & face = faces[face_idx];
            const auto view = glm::lookAt(light_pos, light_pos + face.view_direction, face.up_direction);
            shadow_fbo->bind(face.face);
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);

            for (auto & pass : d3_shadow_passes) {
               pass.draw_batch(
                  shadow_render_callback_t{
                  world_view.creatures_begin(),
                  world_view.creatures_end(),
                  view, light_proj },
                  glpp::DrawMode::Triangles);
            }

            if (do_special_thing) {
               shadow_tex->save_current_framebuffer(std::string("screen_" + face.name + ".png"));
            }
         }
         shadow_fbo->unbind();
         gl::cull_face(gl::cull_face_mode_t::back);
         gl::enable(gl::enable_cap_t::blend);

         prg_3d.use();
         glpp::texture_unit_t tu{ 3 };
         tu.activate();
         shadow_tex->bind();
         prg_3d.uniform("shadow_texture").set(tu);

         //
         // draw to anti-aliasing frame buffer
         //

         gl::viewport(0, 0, dims.x, dims.y);
         msaa_fbo->bind();
         {
            gl::clear_color(1., 1., 1., 1.);
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);

            static bool show_light_perspective = false;
            if (do_special_thing) show_light_perspective = !show_light_perspective;

            //if (!show_light_perspective)
            //   ground_pass.draw(glpp::DrawMode::Triangles);

            //sprite_pass.draw_batch(
            //   sprite_render_callback_t{
            //      world_view.creatures_begin(),
            //      world_view.creatures_end() },
            //   glpp::DrawMode::Triangles);

            //gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);
            auto view = show_light_perspective ? light_view : get_view_cb();
            auto proj = show_light_perspective ? light_proj : get_proj_cb();
//            for (auto & pass : d3_body_passes) {
//               pass.draw_batch(
//                  mesh_render_callback_t{
//                     world_view.creatures_begin(),
//                     world_view.creatures_end(),
//                     view, proj},
////                     get_view_cb(), get_proj_cb()},
//                  glpp::DrawMode::Triangles);
//            }

            //diamond_pass.draw(glpp::DrawMode::Triangles);

            gl::clear(gl::clear_buffer_flags_t::depth_buffer_bit);

            animation_snapshot.advance_to(glpp::get_time());
            bone_pass.draw_batch(
               bone_render_callback_t{ animation_snapshot, view, proj },
               glpp::DrawMode::Triangles);

            //particle_pass.draw(glpp::DrawMode::Points);
         }
         // TODO: tex_fbo should be a non-MSAA renderbuffer (not texture)
         tex_fbo->bind(glpp::frame_buffer_t::Draw);
         msaa_fbo->bind(glpp::frame_buffer_t::Read);
         {
            gl::clear(
               gl::clear_buffer_flags_t::color_buffer_bit |
               gl::clear_buffer_flags_t::depth_buffer_bit);

            msaa_fbo->blit_to_draw_buffer();
         }
         tex_fbo->unbind();
         msaa_fbo->unbind();

         post_pass.draw(glpp::DrawMode::Triangles);

         last_tick = this_tick;

         do_special_thing = false;

         context.win().swap();
      }
   }
   catch (glpp::shader_compile_error const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());
      utils::log(utils::LOG_ERROR, "%s\n", ex.log().c_str());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }
   catch (std::exception const & ex) {
      utils::log(utils::LOG_ERROR, "%s\n", ex.what());

      glpp::shutdown();
      exit(EXIT_FAILURE);
   }

   glpp::shutdown();

   exit(EXIT_SUCCESS);
}
