#version 410 core

layout(triangles) in;
// Use line_strip for visualization and triangle_strip for solids
layout(triangle_strip, max_vertices = 3) out;
//layout(line_strip, max_vertices = 3) out;
in vec2 teTexCoord[3];
in vec3 teNormal[3];
out vec2 gsTexCoord;
out vec3 gsNormal;
out float height;

uniform sampler2D tex;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

uniform float disp;
uniform int texon;

vec2 random2(vec2 st)
{
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
float noise(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

vec3 random3(vec3 st)
{
    st = vec3( dot(st,vec3(127.1,311.7, 543.21)),
              dot(st,vec3(269.5,183.3, 355.23)),
              dot(st,vec3(846.34,364.45, 123.65)) ); // Haphazard additional numbers by IR
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
// Trivially extended to 3D by Ingemar
float noise(vec3 st)
{
    vec3 i = floor(st);
    vec3 f = fract(st);

    vec3 u = f*f*(3.0-2.0*f);

    return mix(
    			mix( mix( dot( random3(i + vec3(0.0,0.0,0.0) ), f - vec3(0.0,0.0,0.0) ),
                     dot( random3(i + vec3(1.0,0.0,0.0) ), f - vec3(1.0,0.0,0.0) ), u.x),
                mix( dot( random3(i + vec3(0.0,1.0,0.0) ), f - vec3(0.0,1.0,0.0) ),
                     dot( random3(i + vec3(1.0,1.0,0.0) ), f - vec3(1.0,1.0,0.0) ), u.x), u.y),

    			mix( mix( dot( random3(i + vec3(0.0,0.0,1.0) ), f - vec3(0.0,0.0,1.0) ),
                     dot( random3(i + vec3(1.0,0.0,1.0) ), f - vec3(1.0,0.0,1.0) ), u.x),
                mix( dot( random3(i + vec3(0.0,1.0,1.0) ), f - vec3(0.0,1.0,1.0) ),
                     dot( random3(i + vec3(1.0,1.0,1.0) ), f - vec3(1.0,1.0,1.0) ), u.x), u.y), u.z

          	);
}

// FBM Noise
float noiseFBM (vec3 p) {
    int octaves = 40;
    float lacunarity = 1.5;
    float gain = 0.6;

    float amp = 0.35;
    float freq = 0.88;
    float noiseValue = 0;

    for(int i = 0; i < octaves; i++){
        noiseValue += amp * (noise(p * freq));
        freq *= lacunarity;
        amp *= gain;
    }
    return noiseValue;
}

vec3 computeNormal(vec3 p){
    float delta = 0.001;

    // use cross 2 times to create a plane, first try with 1,0,0 otherwise 0,1,0
	vec3 v1 = normalize(cross(p, vec3(1,0,0)));
	if (v1 == vec3(0,0,0)) {
        v1 = normalize(cross(p, vec3(0,1,0)));
	}
	vec3 v2 = normalize(cross(p, v1));
	vec3 v3 = normalize(-v2-v1); // get some linearcomb

	// in local coord v1, v2, v3, move a small amount to find points close to the target point p
	vec3 p1 = v1*delta + p;
	vec3 p2 = v2*delta + p;
	vec3 p3 = v3*delta + p;

	p1 = p1 + noiseFBM(p1)*p1;
	p2 = p2 + noiseFBM(p2)*p2;
	p3 = p3 + noiseFBM(p3)*p3;

	// cross method to get the normal
	vec3 s1 = p2-p1;
	vec3 s2 = p3-p1;
    vec3 n = cross(s1,s2);
    return normalize(n);
}

void computeVertex(int nr)
{
	vec3 p, v1, v2, v3, p1, p2, p3, s1, s2, n;

	p = vec3(gl_in[nr].gl_Position);
	// Add interesting code here

	float pointNoise = noiseFBM(p);
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(normalize(p)+pointNoise*p, 1.0);

	height = length(normalize(p)+pointNoise*p);

    gsTexCoord = teTexCoord[0];

	n = computeNormal(p);
    gsNormal = mat3(camMatrix * mdlMatrix) * n;
    EmitVertex();
}

void main()
{
	computeVertex(0);
	computeVertex(1);
	computeVertex(2);
}
