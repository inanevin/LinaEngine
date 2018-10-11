#version 330 core

//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
out vec4 FragColor;

//In order to use the data that sent from the vertex shader we should use in keyword with the exact type and name.
in float red;
in float blue;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
	//FragColor = vec4(red, 0.0, blue, 1.0); // We are passing the color value as rgba.
	FragColor = texture(texture1, TexCoords); //texture is built in GLSL function which return vec4 color values.
											  //the function expects a texture file and texture coordinates.
}