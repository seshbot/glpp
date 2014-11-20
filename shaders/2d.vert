#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

attribute mediump vec4 position;
attribute mediump float red_colour;
attribute mediump vec2 tex_coords;

varying mediump vec2 v_tex_coords;
varying mediump float v_red_colour;

void main() {
   gl_Position = position;
   v_red_colour = red_colour;
   v_tex_coords = tex_coords;
}
