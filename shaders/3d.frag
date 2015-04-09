#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

// for some light colours: http://planetpixelemporium.com/tutorialpages/light.html
// fire (black body palette) http://pages.cs.wisc.edu/~dekruijf/docs/capstone.pdf:
const lowp vec3 COLOUR_FIRE_LOW = vec3(.75, .40, .05);
const lowp vec3 COLOUR_FIRE_MID = vec3(.80, .55, .10);
const lowp vec3 COLOUR_FIRE_HOT = vec3(.95, .70, .15);

struct PositionalLight {
	mediump vec3 world_position;
	mediump vec3 ambient;
	mediump vec3 diffuse;
	mediump float attenuation;
};

const PositionalLight c_light = PositionalLight(vec3(400., 30., -300.), vec3(0., 0., 0.), COLOUR_FIRE_LOW, .4);

uniform mediump mat4 model;
uniform mediump vec4 colour;
uniform samplerCube shadow_texture;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;

const mediump vec3 c_sky_light_dir = vec3(-1., -1., -1.);
const mediump vec3 c_sky_light_intensity = vec3(.1, .2, .8) * .4;

const mediump vec3 c_ambient_intensity = vec3(.2, .6, .8) * .4;

mediump float random(mediump vec4 seed4) {
   mediump float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
   return fract(sin(dot_product) * 43758.5453);
}

mediump float unpack(mediump vec4 packed_dist)
{
   const mediump vec4 unpackFactors = vec4( 1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0 );
   return dot(packed_dist,unpackFactors);
}

mediump float sample_shadow_dist(mediump vec3 uvw) {
     mediump vec4 packed_dist = textureCube(shadow_texture, uvw);
     mediump float unpacked_dist = unpack(packed_dist) * 800.;
	 return unpacked_dist;
}

const mediump vec2 SAMP_1 = vec2(-0.94201624,-0.39906216);
const mediump vec2 SAMP_2 = vec2(0.94558609,-0.76890725);
const mediump vec2 SAMP_3 = vec2(-0.094184101,-0.92938870);
const mediump vec2 SAMP_4 = vec2(0.34495938,0.29387760);

mediump float calc_shadow_factor()
{
   mediump vec3 pos_from_light = c_light.world_position - frag_position;
   mediump vec3 sample_norm1 = normalize(cross(vec3(0., 1., 0.), pos_from_light));
   mediump vec3 sample_norm2 = normalize(cross(sample_norm1, pos_from_light));

   mediump float dist = distance(c_light.world_position, frag_position);
   mediump float sample_range = dist / 100.;
   const mediump float sample_fact = .25;

   // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
   // float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
   // bias = clamp(bias, 0,0.01);

   mediump float visibility = 1.;
   if (sample_shadow_dist(-pos_from_light)                                       < (dist - 2.)) visibility -= sample_fact;
   if (sample_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_1.x + sample_norm2 * SAMP_1.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   //if (sample_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_2.x + sample_norm2 * SAMP_2.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   if (sample_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_3.x + sample_norm2 * SAMP_3.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   if (sample_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_4.x + sample_norm2 * SAMP_4.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   return visibility;
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

varying lowp vec4 frag_bone_indices;
varying lowp vec4 frag_bone_weights;
varying mediump mat4 frag_bone_xform;
uniform mediump mat4 bones[20];
varying mediump float frag_bone_weight_acc;

void main() {
   mediump vec3 n = normalize(frag_normal);
   mediump vec3 l = normalize(-c_sky_light_dir);
   mediump float diffuse_intensity = clamp( dot( n,l ), 0.0, 1.0 );

   mediump vec3 diffuse = colour.rgb * diffuse_intensity * c_sky_light_intensity;
   mediump vec3 ambient = colour.rgb * c_ambient_intensity;
   
   mediump float shadow_factor = 1.; //calc_shadow_factor();
   gl_FragColor = gamma(vec4(ambient + diffuse + shadow_factor * light(c_light, colour.rgb, n), colour.a));
   //gl_FragColor = gamma(vec4(ambient + diffuse, 1.));
   //gl_FragColor = vec4(vec3(frag_bone_weights.x), 1.); //vec4(0., 0., 0., 1.);
   gl_FragColor = vec4(vec3(int(frag_bone_indices.x - 1.)) / 4., 1.);
}
