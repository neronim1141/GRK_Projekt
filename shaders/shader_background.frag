// Author @patriciogv - 2015
// http://patriciogonzalezvivo.com

#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform float scale;

in vec3 interpNormal;
in vec2 interpTexCoord;

float random (in vec3 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec3 _st) {
    vec3 i = floor(_st);
    vec3 f = fract(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec3(1.0, 0.0,0));
    float c = random(i + vec3(0.0, 1.0,0));
    float d = random(i + vec3(1.0, 1.0,0));

    vec3 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define NUM_OCTAVES 5

float fbm ( in vec3 _st) {
    float v = 0.0;
    float a = 0.5;
    vec3 shift = vec3(100.0);
    // Rotate to reduce axial bias
	//x
    mat3 rot = mat3(
	1,  0, 0,
	0, cos(0.5), -sin(0.5),
	0,  sin(0.5), cos(0.5));

    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot* _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

void main() {
    vec3 st = interpNormal/scale;
    // st += st * abs(sin(time*0.1)*3.0);
    vec3 color = vec3(0.0);

    vec3 q = vec3(0.);
    q.x = fbm( st + 0.00*time);
    q.y = fbm( st + vec3(1.0));

    vec3 r = vec3(0.);
    r.x = fbm( st + 1.0*q + vec3(8.684,9.200,6.378)+ 0.078*time );
    r.y = fbm( st + 1.0*q + vec3(3.851,8.300,2.788)+ -0.042*time);
    float f = fbm(st+r);

    color = mix(vec3(0.132,0.172,1.000),
                vec3(0.965,0.947,0.841),
                clamp((f*f)*4.0,0.0,0.0));

    color = mix(color,
                vec3(0.071,0.754,0.825),
                clamp(length(q),0.0,1.0));

    color = mix(color,
                vec3(0.184,0.846,1.000),
                clamp(length(r.x),0.0,1.0));


	vec3 normal = normalize(interpNormal);
    gl_FragColor =  vec4((f*f*f+.6*f*f+.5*f)*color, 1.0);
}