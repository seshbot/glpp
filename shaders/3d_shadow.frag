#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

struct PositionalLight {
	mediump vec3 position;
	mediump vec3 ambient;
	mediump vec3 diffuse;
	mediump float attenuation;
};

uniform mediump vec4 colour;

varying mediump vec3 frag_position;

const PositionalLight c_light = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), vec3(.9, .8, .1), .1);

void main() {
   mediump vec3 vLight = frag_position - c_light.position;
   mediump float dist_squared = dot(vLight, vLight);

   // TODO: https://github.com/fabiensanglard/dEngine/blob/master/data/shaders/f_shadowMapGenerator.glsl
   gl_FragColor = vec4(1., 1., 1., 1.);
}
