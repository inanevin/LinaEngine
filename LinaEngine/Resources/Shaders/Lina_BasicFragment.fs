#version 330 core

//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
out vec4 FragColor;


void main()
{
	FragColor = vec4(0.5, 0.0, 1.0, 1.0); // We are passing the color value as rgba.
	//FragColor = texture(texture1, TexCoords); //texture is built in GLSL function which return vec4 color values.
											  //the function expects a texture file and texture coordinates.
}