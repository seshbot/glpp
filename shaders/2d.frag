#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;
uniform sampler2D texture;
uniform mediump float offset;

varying mediump vec2 v_tex_coords;

void main() {
   mediump vec4 c = texture2D(texture, v_tex_coords);
   if (c.a == 0.) discard;

   mediump float tint_fact = 0.5 * sin(4. * t) + 0.5;
   mediump vec4 tint = vec4(0., tint_fact, 0., 1.);
   gl_FragColor = tint;
}
