#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;

attribute mediump vec4 p;
attribute mediump vec2 tex_coords;

varying mediump vec2 v_tex_coords;

void main() {
   v_tex_coords = tex_coords;
   
   mediump float tt = 4. * (sin(2. * t) + 0.1 * sin(6. * t));
   gl_Position = vec4(p.x * cos(tt) - p.y * sin(tt), p.x * sin(tt) + p.y * cos(tt), p.z, p.w);
}
