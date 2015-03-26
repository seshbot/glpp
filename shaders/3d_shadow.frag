#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

varying mediump vec4 frag_position;


// see https://github.com/fabiensanglard/dEngine/blob/master/data/shaders/f_shadowMapGenerator.glsl
mediump vec4 pack(mediump float val) {
   const vec4 packFactors = vec4( 256.0 * 256.0 * 256.0,    256.0 * 256.0  ,    256.0       ,    1.0);
   const vec4 bitMask     = vec4(0.0                    , 1.0 / 256.0       ,  1.0 / 256.0   , 1.0 / 256.0);
   vec4 packedValue = vec4(fract(packFactors*val));
   packedValue -= packedValue.xxyz * bitMask;
   return packedValue;
}


void main() {
   mediump float normalizedDistance  = frag_position.z / frag_position.w;
   normalizedDistance = (normalizedDistance + 1.0) / 2.0;
   normalizedDistance += 0.0005;

//   mediump vec3 vLight = frag_position - c_light.position;
//   mediump float dist_squared = dot(vLight, vLight);

   gl_FragColor = pack(normalizedDistance);
}
