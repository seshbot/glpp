#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;

varying mediump vec3 frag_position;
varying mediump vec2 frag_tex_coords;


const lowp vec3 COLOUR_FIRE_LOW = vec3(.75, .40, .05);
const lowp vec3 COLOUR_FIRE_MID = vec3(.80, .55, .10);
const lowp vec3 COLOUR_FIRE_HOT = vec3(.95, .70, .15);

struct PositionalLight {
	mediump vec3 world_position;
	mediump vec3 ambient;
	mediump vec3 diffuse;
	mediump float attenuation_linear;
	mediump float attenuation_square;
};

const PositionalLight c_light = PositionalLight(vec3(400., 30., -424.), COLOUR_FIRE_LOW * .2, COLOUR_FIRE_LOW, .0, .00008);
uniform PositionalLight shadow_lights[1];
uniform PositionalLight lights[1];

const mediump vec4 c_ambient_intensity = vec4(.2, .6, .8, 1.) * .08;


mediump vec4 light(PositionalLight light) {
   // calculate attenuation (light strengtht based on inverse square law)
   mediump float dist = distance(frag_position, light.world_position);
   mediump float att = 1. / (0.25 + light.attenuation_linear * dist + light.attenuation_square * dist * dist) - .05;

   // phong calculation
   return att * vec4(light.diffuse, 1.);
}


void main() {
    mediump vec4 colour = texture2D(texture, frag_tex_coords);

    //gl_FragColor = vec4(.75, .40, .05, .1);
	gl_FragColor = colour * .4 * light(shadow_lights[0]) + colour * .4 * light(lights[0]) + colour * c_ambient_intensity;
}
