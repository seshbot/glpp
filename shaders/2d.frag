#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;
uniform mediump float time;

varying mediump vec2 v_tex_coords;
varying mediump float v_red_colour;

void main() {
   mediump float g = .2 + sin(time) * .5;
   gl_FragColor = vec4(v_red_colour, g, .4, 1.);
   gl_FragColor = texture2D(texture, v_tex_coords);
}
