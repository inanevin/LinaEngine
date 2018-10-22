#version 120 //This line states we are using core functionality of OpenGL 3.3

//As we stated in glVertexAttribPointer we declare position at the location 0.
attribute vec3 pos;
attribute vec2 texCoord;

varying vec2 texCoord0;
uniform mat4 modelViewProjection;

void main()
{
	gl_Position =  modelViewProjection * vec4(pos, 1.0);
	texCoord0 = texCoord;

}