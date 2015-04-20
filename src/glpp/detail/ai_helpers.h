#ifndef GLPP_AI_HELPERS__H
#define GLPP_AI_HELPERS__H


#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>

#include <string>
#include <functional>


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
   glm::mat4 to_mat4(aiMatrix4x4 const & from);
   glm::mat4 to_mat4(aiQuaternion const & from);

   void log_node_info(aiScene const & scene, aiNode const & node, int depth = 0);
   void log_scene_info(aiScene const & scene);

   std::string get_name(aiNode const & node);
   std::string get_name(aiAnimation const & animation);
   std::string get_name(aiNodeAnim const & node);
   std::string get_name(aiMesh const & mesh);

   struct frame_info_t { glm::mat4 transform; int rotation_key_idx; int scale_key_idx; int position_key_idx; };
   frame_info_t calculate_animation_frame(aiNodeAnim const & node_anim, double time_ticks, double time_ticks_total, frame_info_t const & prev_frame);

   void for_each_mesh(
      aiScene const & scene,
      aiNode const & node,
      glm::mat4 const & parent_transform,
      std::function<void(aiMesh const &, glm::mat4 const &)> callback);

   struct bounds_t { glm::vec3 lower; glm::vec3 upper; };
   bounds_t find_mesh_bounds(aiMesh const & m);

   double animation_secs_to_ticks(aiAnimation const & animation, double secs);
} // namespace ai

} // namespace glpp


#endif
