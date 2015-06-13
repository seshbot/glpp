#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump vec2 screen_size;
uniform mediump vec3 eye;
uniform mediump mat4 proj;
uniform mediump mat4 view;

attribute mediump vec2 offset_coords;
attribute mediump vec2 tex_coords;
attribute mediump vec3 position;

varying mediump vec2 frag_tex_coords;
varying mediump vec3 frag_position;

void main() {
    // good description here:  http://stackoverflow.com/questions/17397724/point-sprites-for-particle-system

    vec3 eye_to_center = position - eye;
	vec2 horiz_direction = normalize(vec2(eye_to_center.x, eye_to_center.z));
	vec2 horiz_direction_rot90 = vec2(horiz_direction.y, -horiz_direction.x);
	float c = dot(horiz_direction, vec2(0., -1.));
	float s = sin(acos(c)); // dot(horiz_direction_rot90, vec2(0., -1.));

	mat4 rot = mat4(c,  0., s,  0.,
					0., 1., 0., 0.,
					-s, 0., c,  0.,
					0., 0., 0., 1.);

	vec4 rotated_offset = rot * vec4(offset_coords, 0., 0.);

	vec3 vertex_pos = position + rotated_offset.xyz;
	vec4 viewPos = view * vec4(vertex_pos, 1.);

	gl_Position = proj * viewPos;
	frag_position = position;
	frag_tex_coords = tex_coords;
}
