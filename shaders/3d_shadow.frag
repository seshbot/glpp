#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

struct PositionalLight {
	mediump vec3 world_position;
	mediump vec3 ambient;
	mediump vec3 diffuse;
	mediump float attenuation;
};

const PositionalLight c_light = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), vec3(.9, .8, .1), .1);

varying mediump vec4 frag_position;

// see https://github.com/fabiensanglard/dEngine/blob/master/data/shaders/f_shadowMapGenerator.glsl
mediump vec4 pack(mediump float val) {
   const mediump vec4 c_pack_factors = vec4( 256.0 * 256.0 * 256.0,    256.0 * 256.0  ,    256.0       ,    1.0);
   const mediump vec4 bitMask     = vec4(0.0                    , 1.0 / 256.0       ,  1.0 / 256.0   , 1.0 / 256.0);

   mediump vec4 packed_value = vec4(fract(c_pack_factors*val));
   packed_value -= packed_value.xxyz * bitMask;
   return packed_value;
}

void main() {
//   mediump float normalizedDistance = frag_position.z / frag_position.w;
//   normalizedDistance = (normalizedDistance + 1.0) / 2.0;
//   normalizedDistance += 0.0005;

   mediump vec3 vLight = frag_position.xyz - c_light.world_position;
   mediump float dist_squared = dot(vLight, vLight);

   gl_FragColor = pack(dist_squared);
}
