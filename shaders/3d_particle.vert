#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump vec2 screen_size;
uniform mediump mat4 proj;
uniform mediump mat4 view;

attribute mediump vec3 position;

const float SPRITE_SIZE = .16;


void main() {
    // good description here:  http://stackoverflow.com/questions/17397724/point-sprites-for-particle-system
  vec4 viewPos = view * vec4(position, 1.);
  vec4 projVoxel = proj * vec4(.5 * SPRITE_SIZE, .5 * SPRITE_SIZE, viewPos.z, viewPos.w);
  vec2 projSize = screen_size * projVoxel.xy / projVoxel.w;

  gl_PointSize = 1.2 * (projSize.x + projSize.y);
  gl_Position = proj * viewPos;
}
