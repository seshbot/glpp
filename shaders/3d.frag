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

uniform mediump mat4 model;
uniform mediump vec4 colour;
uniform samplerCube shadow_texture;
uniform mediump mat4 light_vp;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;

const mediump vec3 c_sky_light_dir = vec3(-1., -1., -1.);
const mediump vec3 c_sky_light_intensity = vec3(.1, .2, .8) * .4;

const mediump vec3 c_ambient_intensity = vec3(.2, .6, .8) * .01;

mediump float unpack(mediump vec4 packed_dist)
{
   return packed_dist.x;
   const mediump vec4 unpackFactors = vec4( 1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0 );
   return dot(packed_dist,unpackFactors);
}

mediump float calc_shadow_factor()
{
   mediump vec3 pos_from_light = c_light.world_position - frag_position;
   mediump vec4 packed_dist = textureCube(shadow_texture, -pos_from_light);

   mediump float unpacked_dist = unpack(packed_dist);

   mediump vec4 frag_from_light = light_vp * vec4(frag_position, 1.);
   return float((unpacked_dist+0.05) > (frag_from_light.z / frag_from_light.w));
}


lowp vec4 gamma(lowp vec4 c) {
	// return vec4(sqrt(c.rgb), c.a);  // 2.0 gamma correction (use below for 2.2)
	return vec4(pow(c.r, 1. / 2.2), pow(c.g, 1. / 2.2), pow(c.b, 1. / 2.2), c.a);
}

mediump vec3 light(PositionalLight light, mediump vec3 mat, mediump vec3 n) {
   // calculate attenuation (light strenght based on inverse square law)
   mediump float dist = distance(frag_position, light.world_position) / 50.;
   mediump float att = 1. / (1. + light.attenuation * dist + light.attenuation * dist * dist);

   // from http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
   // att = clamp(1.0 - dist*dist/(radius*radius), 0.0, 1.0); att *= att

   // calculate lambertian intensity
   mediump vec3 dir = normalize(frag_position - light.world_position);
   mediump float intensity = max(0., dot(n, -dir));

   // phong calculation
   mediump vec3 val = att * (mat * light.ambient + mat * light.diffuse * intensity);

   return val;
}

void main() {
   mediump vec3 n = normalize(frag_normal);
   mediump vec3 l = normalize(-c_sky_light_dir);
   mediump float diffuse_intensity = clamp( dot( n,l ), 0.0, 1.0 );

   mediump float shadow_factor = calc_shadow_factor();

   mediump vec3 diffuse = colour.rgb * diffuse_intensity * c_sky_light_intensity;
   mediump vec3 ambient = colour.rgb * c_ambient_intensity;

   gl_FragColor = gamma(vec4(ambient + shadow_factor * diffuse + shadow_factor * light(c_light, colour.rgb, n), 1.));
   //gl_FragColor = gamma(vec4(ambient + shadow_factor * diffuse, 1.));
}
