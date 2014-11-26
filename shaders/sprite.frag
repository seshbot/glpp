#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

const mediump vec2 img_wh = vec2(256., 512.);

uniform sampler2D texture;
uniform mediump vec2 texture_wh;
uniform mediump ivec2 sprite_xy;
uniform mediump ivec2 sprite_wh;

varying mediump vec2 v_tex_coords;

mediump vec2 sprite_uv() {
   mediump vec2 sprite_frame_xy = vec2(float(sprite_xy.x) / img_wh.x, float(sprite_xy.y) / img_wh.y);
   mediump vec2 sprite_frame_wh = vec2(float(sprite_wh.x) / img_wh.x, float(sprite_wh.y) / img_wh.y);

   return sprite_frame_xy + v_tex_coords * sprite_frame_wh;
}

void main() {
   mediump vec4 c = texture2D(texture, sprite_uv());
   if (c.a == 0.) discard;
   //c.a = 1.;

   lowp float cc = sprite_uv().y; //sprite_wh.x / img_wh.x;   //sprite_wh.x;// / 800.;
   gl_FragColor = c; //vec4(cc, cc, cc, 1.);
}
