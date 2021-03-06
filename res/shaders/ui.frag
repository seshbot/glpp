#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

const mediump float c_light_intensity = .4;

uniform sampler2D texture;

varying mediump vec2 v_tex_coords;

void main() {
   mediump vec4 c = texture2D(texture, v_tex_coords);
   if (c.a == 0.) discard;

   gl_FragColor = vec4(c.rgb * c_light_intensity, 1.);
}
