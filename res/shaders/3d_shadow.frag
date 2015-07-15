#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

struct PositionalLight {
	lowp float is_directional;
	mediump vec3 world_position;
	mediump vec3 ambient;
	mediump vec3 diffuse;
	mediump float attenuation_linear;
	mediump float attenuation_square;
};

uniform PositionalLight shadow_lights[1];

varying mediump vec3 frag_position;

// see https://github.com/fabiensanglard/dEngine/blob/master/data/shaders/f_shadowMapGenerator.glsl
mediump vec4 pack(mediump float val) {
   const mediump vec4 c_pack_factors = vec4( 256.0 * 256.0 * 256.0,    256.0 * 256.0  ,    256.0       ,    1.0);
   const mediump vec4 bitMask     = vec4(0.0                    , 1.0 / 256.0       ,  1.0 / 256.0   , 1.0 / 256.0);

   mediump vec4 packed_value = vec4(fract(c_pack_factors*val));
   packed_value -= packed_value.xxyz * bitMask;
   return packed_value;
}

void main() {
   //mediump float normalizedDistance = frag_position.z / frag_position.w;
   //normalizedDistance = (normalizedDistance + 1.0) / 2.0;
   //normalizedDistance += 0.0005;

   mediump float dist = distance(frag_position, shadow_lights[0].world_position);

   mediump vec4 packed_ = pack(dist / 800.);
   gl_FragColor = packed_;
}
