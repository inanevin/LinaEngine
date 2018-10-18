#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
in vec2 texCoord0;

uniform float alpha;
uniform vec3 color;
uniform sampler2D sampler;

void main()
{
	vec4 textureColor = texture2D(sampler, texCoord0.xy);

	if(textureColor == 0)
		gl_FragColor = vec4(color, alpha);
	else
		gl_FragColor = texture2D(sampler, texCoord0.xy) * vec4(color, alpha);
}