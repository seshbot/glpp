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
uniform samplerCube shadow_texture;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;

const mediump vec3 c_sky_light_dir = vec3(-1., -1., -1.);
const mediump vec3 c_sky_light_intensity = vec3(.1, .2, .8) * .2;

const PositionalLight c_light1 = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), vec3(.9, .8, .1), .1);
const PositionalLight c_light2 = PositionalLight(vec3(200., 30., -200.), vec3(0., 0., 0.), vec3(.2, .8, .9), .1);

const mediump vec3 c_ambient_intensity = vec3(.2, .6, .8) * .01;



mediump float unpack(mediump vec4 packedZValue)
{
   const mediump vec4 unpackFactors = vec4( 1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0 );
   return dot(packedZValue,unpackFactors);
}

mediump float getShadowFactor(mediump vec4 lightZ)
{
   mediump vec4 packedZValue = textureCube(shadow_texture, lightZ.xyz);

   mediump float unpackedZValue = unpack(packedZValue);

   return float((unpackedZValue+0.0001) > lightZ.z);
}



lowp vec4 gamma(lowp vec4 c) {
	// return vec4(sqrt(c.rgb), c.a);  // 2.0 gamma correction (use below for 2.2)
	return vec4(pow(c.r, 1. / 2.2), pow(c.g, 1. / 2.2), pow(c.b, 1. / 2.2), c.a);
}

mediump vec3 light(PositionalLight light, mediump vec3 mat, mediump vec3 n) {
   // calculate attenuation (light strenght based on inverse square law)
   mediump float dist = distance(frag_position, light.position) / 50.;
   mediump float att = 1. / (1. + light.attenuation * dist + light.attenuation * dist * dist);

   // from http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
   // att = clamp(1.0 - dist*dist/(radius*radius), 0.0, 1.0); att *= att

   // calculate lambertian intensity
   mediump vec3 dir = normalize(frag_position - light.position);
   mediump float intensity = max(0., dot(n, -dir));

   // phong calculation
   mediump vec3 val = att * (mat * light.ambient + mat * light.diffuse * intensity);

   return val;
}

void main() {
   mediump vec3 n = normalize(frag_normal);
   mediump vec3 l = normalize(-c_sky_light_dir);
   mediump float diffuse_intensity = clamp( dot( n,l ), 0.0, 1.0 );

   mediump vec3 diffuse = colour.rgb * diffuse_intensity * c_sky_light_intensity;
   mediump vec3 ambient = colour.rgb * c_ambient_intensity;

   gl_FragColor = gamma(vec4(ambient + diffuse + light(c_light1, colour.rgb, n), 1.));
}
