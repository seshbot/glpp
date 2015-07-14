#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;
uniform lowp float override_alpha;

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

uniform PositionalLight shadow_lights[1];
uniform PositionalLight lights[4];
uniform lowp vec4 colour;

uniform lowp vec3 sky_light_colour; // = vec3(.5, .5, .8);
uniform lowp float sky_light_intensity; // = .001;
uniform lowp vec3 ambient_colour; // = vec3(.2, .6, .8);
uniform lowp float ambient_intensity; // = .0;


mediump vec4 light(PositionalLight light) {
   // calculate attenuation (light strengtht based on inverse square law)
   mediump float dist = distance(frag_position, light.world_position);
   mediump float att = 1. / (.6 + light.attenuation_linear * dist + light.attenuation_square * dist * dist);

   // phong calculation
   return att * vec4(light.diffuse, 1.);
}


void main() {
    mediump vec4 col = colour;
	
	if (col.a == 0.)
	{
		col = vec4(vec3(1.), .2); // texture2D(texture, frag_tex_coords);
	}

   mediump vec4 diffuse = col * vec4(sky_light_colour, 1.) * sky_light_intensity;
   mediump vec4 ambient = col * vec4(ambient_colour, 1.) * ambient_intensity;

   gl_FragColor = 
    (ambient + 
     diffuse + 
	 col * 
       (light(shadow_lights[0]) +
        light(lights[0]) +
        light(lights[1]) +
        light(lights[2]) +
        light(lights[3]))
    );

   if (override_alpha >= 0.) {
      gl_FragColor = vec4(gl_FragColor.rgb, override_alpha);
   }
}
