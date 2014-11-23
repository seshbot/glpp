#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;
uniform sampler2D texture;

varying mediump vec2 v_tex_coords;

void main() {
   mediump float xoff = 0.01 * sin(20. * t + 40. * v_tex_coords.y);

   gl_FragColor = texture2D(texture, vec2(v_tex_coords.x + xoff, v_tex_coords.y));
}
 