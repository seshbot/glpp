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

const PositionalLight c_light = PositionalLight(vec3(400., 30., -424.), COLOUR_FIRE_LOW * .2, COLOUR_FIRE_LOW, .00008);

uniform mediump vec4 colour;
uniform samplerCube shadow_texture;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;

const mediump vec3 c_sky_light_dir = vec3(-1., -1., -1.);
const mediump vec3 c_sky_light_intensity = vec3(.1, .2, .8) * .00;

const mediump vec3 c_ambient_intensity = vec3(.2, .6, .8) * .0;

mediump float random(mediump vec4 seed4) {
   mediump float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
   return fract(sin(dot_product) * 43758.5453);
}

mediump float unpack(mediump vec4 packed_dist) {
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

mediump float calc_shadow_factor(mediump vec3 world_position) {
   mediump vec3 pos_from_light = world_position - frag_position;
   mediump vec3 sample_norm1 = normalize(cross(vec3(0., 1., 0.), pos_from_light));
   mediump vec3 sample_norm2 = normalize(cross(sample_norm1, pos_from_light));

   mediump float dist = distance(world_position, frag_position);
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
	return vec4(sqrt(c.rgb), c.a);  // 2.0 gamma correction (use below for 2.2)
	//return vec4(pow(c.r, 1. / 2.2), pow(c.g, 1. / 2.2), pow(c.b, 1. / 2.2), c.a);
}

mediump vec3 light(PositionalLight light) {
   // calculate attenuation (light strengtht based on inverse square law)
   mediump float dist = distance(frag_position, light.world_position);
   mediump float att = 1. / (0.25 + light.attenuation * dist * dist) - .05;

   // from http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
   // att = clamp(1.0 - dist*dist/(radius*radius), 0.0, 1.0); att *= att

   
   mediump vec3 n = normalize(frag_normal);
   mediump float shadow_factor = calc_shadow_factor(light.world_position);

   // in shadow, dont bother calculating diffuse factor
   if (shadow_factor < 0.01) {
      return att * light.ambient;
   }

    // calculate lambertian intensity
    mediump vec3 dir = normalize(frag_position - light.world_position);
    mediump float diffuse_intensity = max(0., dot(n, -dir));

    // phong calculation
    return att * (light.ambient + light.diffuse * diffuse_intensity * shadow_factor);
}

const mediump float LOG2 = 1.442695;
const mediump float c_fog_density = 0.0008;
const mediump vec4 c_fog_colour = vec4(1., 1., 1., 1.);
mediump float fog_factor() {
   // various fog calcs: https://code.google.com/p/gles2-bc/source/browse/trunk/Sources/OpenGLES/OpenGLES20/shaders/fog.glsl?r=4
   // for ortho
   return clamp(.95 + .0002 * frag_position.y, 0., 1.);

   // for perspective
   //mediump float z = frag_position4.z / frag_position4.w;
   //return clamp(exp2(-c_fog_density * c_fog_density * z * z * LOG2), 0., 1.);
}

void main() {
   mediump vec3 n = normalize(frag_normal);
   mediump vec3 l = normalize(-c_sky_light_dir);
   mediump float diffuse_intensity = clamp( dot( n,l ), 0.0, 1.0 );

   mediump vec3 diffuse = colour.rgb * diffuse_intensity * c_sky_light_intensity;
   mediump vec3 ambient = colour.rgb * c_ambient_intensity;
   
   gl_FragColor = vec4(ambient + diffuse + colour.rgb * light(c_light), colour.a);
   //gl_FragColor = mix(c_fog_colour, gl_FragColor, fog_factor());
   //gl_FragColor = colour; // vec4(.0, .0, .0, 1.);
}
