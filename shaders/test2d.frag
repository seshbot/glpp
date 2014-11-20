#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;

varying mediump vec2 v_tex_coords;

void main() {
   gl_FragColor = texture2D(texture, v_tex_coords);
}
