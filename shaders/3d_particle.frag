#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

void main() {
//    lowp vec2 uv = vec2(frag_uv.x, frag_uv.y + timer);
//    lowp vec4 colour = texture2D(sampler, uv);
//    if (colour.a == 0.) discard;

    gl_FragColor = vec4(.8, .8, .85, .5);
}
