#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;
uniform mediump mat4 model;
uniform mediump mat4 mvp;
uniform mediump mat4 normal_matrix;
uniform mediump mat4 bones[20];

attribute mediump vec3 p;
attribute mediump vec3 normal;
attribute lowp vec4 bone_indices;
attribute lowp vec4 bone_weights;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;
varying lowp vec4 frag_bone_indices;
varying lowp vec4 frag_bone_weights;
varying mediump mat4 frag_bone_xform;
varying mediump float frag_bone_weight_acc;


void main() {
	frag_bone_weight_acc = 1. - dot(bone_weights, vec4(1., 1., 1., 1.));
   mat4 BoneTransform = bones[int(bone_indices.x)]; // * bone_weights.x;
   //mat4 BoneTransform = bones[1];
   //BoneTransform     += bones[int(bone_indices.y)] * bone_weights.y;
   //BoneTransform     += bones[int(bone_indices.z)] * bone_weights.z;
   //BoneTransform     += bones[int(bone_indices.w)] * bone_weights.w;

   frag_bone_indices = bone_indices;
   frag_bone_weights = bone_weights;
   frag_bone_xform = BoneTransform;

   //BoneTransform = mat4(1.);

   gl_Position = mvp * BoneTransform * vec4(p, 1.);
   frag_position = vec4(model * BoneTransform * vec4(p, 1.)).xyz;

   // TODO: incorporate BoneTransform in normal_matrix
   frag_normal = normalize(vec4(BoneTransform * vec4(normal, 0.)).xyz);
}
