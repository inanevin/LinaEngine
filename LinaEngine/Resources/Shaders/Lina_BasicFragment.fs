#version 330 core

in vec4 color;

//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
out vec4 fragColor;

void main()
{
	fragColor = color;
}