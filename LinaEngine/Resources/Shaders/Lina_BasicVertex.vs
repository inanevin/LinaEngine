#version 330 core //This line states we are using core functionality of OpenGL 3.3

//As we stated in glVertexAttribPointer we declare position at the location 0.
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 texCoord0;
uniform mat4 model;



void main()
{
	gl_Position =  model * vec4(position, 1.0);
	texCoord0 = texCoord;

}