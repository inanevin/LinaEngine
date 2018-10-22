#version 120 //This line states we are using core functionality of OpenGL 3.3

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

varying vec2 texCoord0;
varying vec3 normal0;
varying vec3 worldPos0;

uniform mat4 model;
uniform mat4 modelViewProjection;


void main()
{	
	gl_Position =  modelViewProjection * vec4(position, 1.0);
	texCoord0 = texCoord;
	normal0 = (model * vec4(normal, 0.0)).xyz;
	worldPos0 = (model * vec4(position, 1.0)).xyz;
}