#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

attribute mediump vec4 position;

void main() {
   gl_Position = position;
}
