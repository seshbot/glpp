#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float depth;

attribute mediump vec2 p;
attribute mediump vec2 tex_coords;

varying mediump vec2 v_tex_coords;

void main() {
   v_tex_coords = tex_coords;
   gl_Position = vec4(p.x, p.y, depth, 1.);
}
