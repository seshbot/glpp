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

   gl_FragColor = vec4(factor, 0., 0., 1.); // c; // * vec4(1., .9, 1., 1.);
}
