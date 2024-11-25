#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 exNormal;
in vec3 exPosition;
uniform sampler2D tex;

void main(void)
{
	// Texture from disc
	vec4 t = texture(tex, texCoord);

	// Procedural texture
	t.r = sin(texCoord.s * 3.1416);
	t.g = sin(texCoord.t * 3.1416);
	t.b = sin((texCoord.s + texCoord.t) * 10.0);

	vec3 n = normalize(exNormal);
	float shade = n.y + n.z;
//	if (t.a < 0.01) discard;
//	else
    vec4 color1 = t * shade * shade; // Over-emphasized fake light

//	outColor = vec4(texCoord.s, texCoord.t, 0, 1);
	vec4 color2 = vec4(abs(n.x), abs(n.y), abs(n.z), 1);
//	outColor = vec4(1) * shade;
    float mixValue = 0;
    if(exPosition.y < 0.5){
        mixValue = 1;
    }

    //float height = sin(exPosition.y);



    outColor = color1 * mixValue + color2 * (1-mixValue);

    //outColor = vec4(abs(n.x), abs(n.y), abs(n.z), 1);
}
