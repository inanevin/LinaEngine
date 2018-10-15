#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
out vec4 fragColor;
in vec4 color;
void main()
{
	fragColor = color;
}