#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump mat4 model;
uniform mediump mat4 mvp;

attribute mediump vec3 p;

varying mediump vec3 frag_position;


void main() {
   gl_Position = mvp * vec4(p, 1.);
   frag_position = vec4(model * vec4(p, 1.)).xyz;
}
