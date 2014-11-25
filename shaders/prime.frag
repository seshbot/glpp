#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform sampler2D texture;
struct SubTexture {
   mediump float startx;
   mediump float starty;
   mediump float width;
   mediump float height;
};

uniform int sub_tex_count;
uniform SubTexture sub_tex[3];

uniform mediump vec2 texture_dims;
uniform mediump vec2 win_dims;

varying mediump vec2 v_tex_coords;

mediump vec2 tex_coords(int idx) {
   // create scaled coordinates (0..1 -> 0..win/img)
   mediump vec2 scaled = vec2(
      v_tex_coords.x * win_dims.x / texture_dims.x,
	  v_tex_coords.y * win_dims.y / texture_dims.y);

   // then pick the part of the texture our image spans
   mediump float start_ratio_x = sub_tex[idx].startx / texture_dims.x;
   mediump float start_ratio_y = sub_tex[idx].starty / texture_dims.y;
   mediump float width_ratio_x = sub_tex[idx].width / texture_dims.x;
   mediump float width_ratio_y = sub_tex[idx].height / texture_dims.y;

   return vec2(
     start_ratio_x + mod(scaled.x / 16., width_ratio_x),
     start_ratio_y + mod(scaled.y / 16., width_ratio_y));
}

mediump vec4 acc_texture2D() {
   // accumulate colour for this texture
   mediump vec4 c = vec4(1., 1., 1., 0.);

   for (int idx = 0; idx < sub_tex_count; idx++) {
     mediump vec4 c0 = texture2D(texture, tex_coords(idx));
	 mediump float a = max(c.a, c0.a);
	 c = c * c0;
	 c.a = a;
   }

   return c;
}

void main() {
   mediump vec4 c = acc_texture2D();
   
   if (c.a == 0.) discard;

   gl_FragColor = c;
}
