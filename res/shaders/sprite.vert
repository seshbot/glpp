#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump mat4 model;
uniform mediump mat4 proj;
uniform mediump ivec2 sprite_wh;

attribute mediump vec2 p;
attribute mediump vec2 tex_coords;

varying mediump vec2 v_tex_coords;

void main() {
   v_tex_coords = tex_coords;
   gl_Position = proj * model * vec4(p.x * float(sprite_wh.x), p.y * float(sprite_wh.y), 0., 1.);
}
