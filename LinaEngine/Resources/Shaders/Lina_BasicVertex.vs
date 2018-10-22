#version 330 core //This line states we are using core functionality of OpenGL 3.3

//As we stated in glVertexAttribPointer we declare position at the location 0.
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 texCoord0;
uniform mat4 transform;



void main()
{
	gl_Position =  transform * vec4(pos, 1.0);
	texCoord0 = texCoord;

}