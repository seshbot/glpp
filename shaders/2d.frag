uniform lowp float time;

void main() {
   lowp float g = .2 + sin(time) * .5;
   gl_FragColor = vec4(.4, g, .4, 1.); // vec4(.4, .6, .4, 1.);
}
