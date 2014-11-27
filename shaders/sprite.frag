#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;
uniform mediump vec2 texture_wh;
uniform mediump ivec2 sprite_xy;
uniform mediump ivec2 sprite_wh;

varying mediump vec2 v_tex_coords;

mediump vec2 sprite_uv() {
   mediump vec2 sprite_frame_xy = vec2(float(sprite_xy.x) / texture_wh.x, float(sprite_xy.y) / texture_wh.y);
   mediump vec2 sprite_frame_wh = vec2(float(sprite_wh.x) / texture_wh.x, float(sprite_wh.y) / texture_wh.y);

   return sprite_frame_xy + v_tex_coords * sprite_frame_wh;
}

void main() {
   mediump vec4 c = texture2D(texture, sprite_uv());
   if (c.a == 0.) discard;

   gl_FragColor = c;
}
