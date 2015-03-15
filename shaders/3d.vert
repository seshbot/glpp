#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;
uniform mediump mat4 local;
uniform mediump mat4 model;
uniform mediump mat4 view;
uniform mediump mat4 proj;
uniform mediump mat4 normal_matrix;

attribute mediump vec3 p;
attribute mediump vec3 normal;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;


void main() {
   frag_normal = normalize(vec4(normal_matrix * local * vec4(normal, 0.)).xyz);
   gl_Position = proj * view * model * local * vec4(p, 1.);
   frag_position = vec4(model * vec4(p, 1.)).xyz;
}
