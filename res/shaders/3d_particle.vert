#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump vec2 screen_size;
uniform mediump mat4 proj;
uniform mediump mat4 view;

attribute mediump vec2 offset_coords;
attribute mediump vec2 tex_coords;
attribute mediump vec3 position;

varying mediump vec2 frag_tex_coords;
varying mediump vec3 frag_position;

void main() {
    // good description here:  http://stackoverflow.com/questions/17397724/point-sprites-for-particle-system

  vec3 vertex_pos = position + vec3(offset_coords, 0.);
  vec4 viewPos = view * vec4(vertex_pos, 1.);

  gl_Position = proj * viewPos;
  frag_position = position;
  frag_tex_coords = tex_coords;
}
