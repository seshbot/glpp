#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;
uniform mediump float offset;

attribute mediump vec2 p;
attribute mediump vec2 tex_coords;

varying mediump vec2 v_tex_coords;

void main() {
   v_tex_coords = tex_coords;
   mediump float offset_phase = 0.5 * sin(4. * t) + 0.5;
   mediump float o = offset;// * offset_phase;
   gl_Position = vec4(p.x * .75 + o , p.y * .75 + o, 0., 1.);
}
