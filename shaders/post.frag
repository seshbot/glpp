#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;
uniform mediump float factor;

varying mediump vec2 v_tex_coords;

void main() {
   mediump vec4 c = texture2D(texture, v_tex_coords);

   gl_FragColor = c * vec4(1., .2, 1., 1.);
}
