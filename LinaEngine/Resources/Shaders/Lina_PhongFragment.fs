#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
in vec2 texCoord0;
out vec4 fragColor;

uniform vec3 baseColor;
uniform vec3 ambientLight;
uniform sampler2D sampler;

void main()
{
	vec4 totalLight = vec4(ambientLight,1);
	vec4 color = vec4(baseColor,1);
	vec4 textureColor = texture(sampler, texCoord0.xy);
	
	if(textureColor != vec4(0,0,0,0))
		color *= textureColor;
	
	fragColor = color * totalLight;
}