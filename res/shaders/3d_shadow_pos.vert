#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump mat4 model;
uniform mediump mat4 mvp;
uniform mediump mat4 bones[20];

attribute mediump vec3 p;
attribute lowp vec4 bone_indices;
attribute lowp vec4 bone_weights;

varying mediump vec3 frag_position;


void main() {
   vec4 norm_bone_weights = bone_weights;
   norm_bone_weights.w = 1. - dot(bone_weights.xyz, vec3(1., 1., 1.));
   mat4 BoneTransform = bones[int(bone_indices.x)] * norm_bone_weights.x;
   BoneTransform     += bones[int(bone_indices.y)] * norm_bone_weights.y;
   BoneTransform     += bones[int(bone_indices.z)] * norm_bone_weights.z;
   BoneTransform     += bones[int(bone_indices.w)] * norm_bone_weights.w;

   gl_Position = mvp * BoneTransform * vec4(p, 1.);
   frag_position = vec4(model * BoneTransform * vec4(p, 1.)).xyz;
}
