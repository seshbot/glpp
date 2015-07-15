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
uniform PositionalLight lights[4];

uniform mediump vec3 sky_light_dir;
uniform lowp vec3 sky_light_colour;
uniform lowp float sky_light_intensity;

uniform lowp vec3 ambient_colour;
uniform lowp float ambient_intensity;

uniform lowp float use_texture;
uniform sampler2D texture;
uniform mediump vec4 colour;
uniform samplerCube pos_shadow_texture;
uniform sampler2D dir_shadow_texture;

varying mediump vec4 dir_shadow_texture_coords;

varying mediump vec3 frag_position;
varying mediump vec3 frag_normal;
varying mediump vec2 frag_tex_coords;

mediump float random(mediump vec4 seed4) {
   mediump float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
   return fract(sin(dot_product) * 43758.5453);
}

mediump float unpack(mediump vec4 packed_dist) {
   const mediump vec4 unpackFactors = vec4( 1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0 );
   return dot(packed_dist,unpackFactors);
}

mediump float sample_pos_shadow_dist(mediump vec3 uvw) {
     mediump vec4 packed_dist = textureCube(pos_shadow_texture, uvw);
     mediump float unpacked_dist = unpack(packed_dist) * 800.;
	 return unpacked_dist;
}

mediump float sample_dir_shadow_dist(mediump vec2 uv) {
     mediump vec4 packed_dist = texture2D(dir_shadow_texture, uv);
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
   if (sample_pos_shadow_dist(-pos_from_light)                                       < (dist - 2.)) visibility -= sample_fact;
   if (sample_pos_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_1.x + sample_norm2 * SAMP_1.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   //if (sample_pos_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_2.x + sample_norm2 * SAMP_2.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   if (sample_pos_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_3.x + sample_norm2 * SAMP_3.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   if (sample_pos_shadow_dist(-pos_from_light + (sample_norm1 * SAMP_4.x + sample_norm2 * SAMP_4.y) * sample_range) < (dist - 2.)) visibility -= sample_fact;
   return visibility;
}

mediump vec3 light_impl(PositionalLight light, mediump float shadow_factor) {
   // calculate attenuation (light strengtht based on inverse square law)
   mediump float dist = distance(frag_position, light.world_position);
   mediump float att = clamp(1. / (0.25 + light.attenuation_linear * dist + light.attenuation_square * dist * dist) - .05, 0., 1.);

   // from http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
   // att = clamp(1.0 - dist*dist/(radius*radius), 0.0, 1.0); att *= att

   // in shadow, dont bother calculating diffuse factor
   if (shadow_factor < 0.01) {
      return att * light.ambient;
   }
   
   // calculate lambertian intensity
   mediump vec3 dir = normalize(frag_position - light.world_position);
   mediump vec3 n = normalize(frag_normal);
   mediump float diffuse_intensity = max(0., dot(n, -dir));

   // phong calculation
   return clamp(att * (light.ambient + light.diffuse * diffuse_intensity * shadow_factor), 0., 1.);
}

mediump vec3 shadowed_light(PositionalLight light) {
   if (light.is_directional > 0.5) {
     return light.diffuse;
   }
   else {
      mediump float shadow_factor = calc_shadow_factor(light.world_position);
      return light_impl(light, shadow_factor);
   }
}

mediump vec3 light(PositionalLight light) {
	return light_impl(light, 1.);
}

const mediump float LOG2 = 1.442695;
const mediump float c_fog_density = 0.0008;
const mediump vec4 c_fog_colour = vec4(.25, .25, .25, 1.); // vec4(.2, .6, .8, 1.); //
mediump float fog_factor() {
   // various fog calcs: https://code.google.com/p/gles2-bc/source/browse/trunk/Sources/OpenGLES/OpenGLES20/shaders/fog.glsl?r=4
   // for ortho
   return clamp(.97 + .0001 * frag_position.y, 0., 1.);

   // for perspective
   //mediump float z = frag_position4.z / frag_position4.w;
   //return clamp(exp2(-c_fog_density * c_fog_density * z * z * LOG2), 0., 1.);
}

void main() {
   mediump vec3 n = normalize(frag_normal);
   mediump vec3 l = normalize(-sky_light_dir);
   mediump float diffuse_intensity = clamp( dot( n,l ), 0.0, 1.0 );

   //mediump vec4 diffuse_colour = vec4(1., 0., 1., 1.);
   mediump vec4 diffuse_colour = colour;
   if (use_texture > .5) {
	  diffuse_colour = texture2D(texture, frag_tex_coords);
   }

   mediump vec3 diffuse = diffuse_colour.rgb * diffuse_intensity * sky_light_colour * sky_light_intensity;
   mediump vec3 ambient = diffuse_colour.rgb * ambient_colour * ambient_intensity;
   
   gl_FragColor = vec4(
	ambient + 
	diffuse + 
	diffuse_colour.rgb *
		(shadowed_light(shadow_lights[0]) +
		 light(lights[0]) +
		 light(lights[1]) +
		 light(lights[2]) +
		 light(lights[3])),
	colour.a);

   //gl_FragColor = mix(c_fog_colour, gl_FragColor, fog_factor());
   //gl_FragColor = vec4(c.rgb, 1.); // colour; // vec4(.0, .0, .0, 1.);
   //gl_FragColor = diffuse_colour;
}
