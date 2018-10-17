#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
in vec2 texCoord0;

uniform sampler2D sampler;

void main()
{
	gl_FragColor = texture2D(sampler, texCoord0.xy);
}