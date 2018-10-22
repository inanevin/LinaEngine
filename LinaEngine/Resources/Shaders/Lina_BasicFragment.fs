#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
in vec2 texCoord0;

uniform vec3 color;
uniform sampler2D diffuse;

void main()
{
	vec4 textureColor = texture2D(diffuse, texCoord0.xy);

	if(textureColor == vec4(0,0,0,0))
		gl_FragColor = vec4(color, 1);
	else
		gl_FragColor = textureColor * vec4(color, 1);
}