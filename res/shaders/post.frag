#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

uniform mediump float t;
uniform sampler2D texture;

varying mediump vec2 v_tex_coords;

// http://stackoverflow.com/questions/9234724/how-to-change-hue-of-a-texture-with-glsl
lowp vec4 hsv_adjust(lowp vec4 color, lowp float hueAdjust)
{
    const lowp vec4 kRGBToYPrime = vec4 (0.299, 0.587, 0.114, 0.0);
    const lowp vec4 kRGBToI     = vec4 (0.596, -0.275, -0.321, 0.0);
    const lowp vec4 kRGBToQ     = vec4 (0.212, -0.523, 0.311, 0.0);

    const lowp vec4 kYIQToR   = vec4 (1.0, 0.956, 0.621, 0.0);
    const lowp vec4 kYIQToG   = vec4 (1.0, -0.272, -0.647, 0.0);
    const lowp vec4 kYIQToB   = vec4 (1.0, -1.107, 1.704, 0.0);

    // Convert to YIQ
    lowp float   YPrime  = dot (color, kRGBToYPrime);
    lowp float   I      = dot (color, kRGBToI);
    lowp float   Q      = dot (color, kRGBToQ);

    // Calculate the hue and chroma
    lowp float   hue     = atan (Q, I);
    lowp float   chroma  = sqrt (I * I + Q * Q);

    // Make the user's adjustments
    hue += hueAdjust;

    // Convert back to YIQ
    Q = chroma * sin (hue);
    I = chroma * cos (hue);

    // Convert back to RGB
    lowp vec4    yIQ   = vec4 (YPrime, I, Q, 0.0);
    color.r = dot (yIQ, kYIQToR);
    color.g = dot (yIQ, kYIQToG);
    color.b = dot (yIQ, kYIQToB);

	return color;
}

lowp vec3 post_adjust(lowp vec3 rgb, mediump vec2 xy) {
    // from http://glsl.heroku.com/e#12543.1

    // Gamma first...
	rgb = sqrt(rgb);
    //rgb = pow(rgb, vec3(0.45));

    lowp float CONTRAST = 1.2;
    lowp float SATURATION = 1.;
    lowp float BRIGHTNESS = .6;
    rgb = mix(vec3(.5), mix(vec3(dot(vec3(.2125, .7154, .0721), rgb*BRIGHTNESS)), rgb*BRIGHTNESS, SATURATION), CONTRAST);
    // Vignette...
    rgb *= .4+0.5*pow(40.0*xy.x*xy.y*(1.0-xy.x)*(1.0-xy.y), 0.1 );
    return rgb;
}

void main() {
   lowp vec4 colour = texture2D(texture, v_tex_coords);

   //colour = hsv_adjust(colour, sin(2. * t));
   lowp vec3 post_colour = post_adjust(colour.rgb, v_tex_coords);
   //gl_FragColor = vec4(post_colour.rgb, colour.a);
   gl_FragColor = vec4(sqrt(colour.rgb), colour.a);
}
