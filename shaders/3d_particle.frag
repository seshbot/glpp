#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

//uniform sampler2D texture;

void main() {
//    lowp vec2 uv = vec2(frag_uv.x, frag_uv.y + timer);
//    lowp vec4 colour = texture2D(sampler, uv);
//    if (colour.a == 0.) discard;

//    mediump vec4 colour = texture2D(texture, gl_PointCoord);

    gl_FragColor = vec4(.75, .40, .05, .1);
}
