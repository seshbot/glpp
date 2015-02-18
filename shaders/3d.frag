#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump vec4 colour;

varying mediump vec3 frag_normal;

// TODO: light colour?
const mediump vec3 c_light_dir = vec3(-1., -1., -1.);

void main() {
   mediump vec3 n = normalize(frag_normal);
   mediump vec3 l = normalize(-c_light_dir);
   mediump float diffuse_intensity = clamp( dot( n,l ), 0.0, 1.0 );

   mediump vec3 diffuse = colour.rgb * diffuse_intensity;
   mediump vec3 ambient = colour.rgb * (1. - diffuse_intensity) * 0.2;

   gl_FragColor = vec4(diffuse + ambient, 1.);
}
