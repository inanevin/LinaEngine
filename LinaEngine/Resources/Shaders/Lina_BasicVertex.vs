#version 330 core //This line states we are using core functionality of OpenGL 3.3

//As we stated in glVertexAttribPointer we declare position at the location 0.
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 texCoord0;
uniform mat4 transform;



void main()
{
	
	//color = vec4(clamp(pos, 0.0, 1.0), 1.0);
	
	//We are using built in GLSL variable gl_Position for passing our Position data which we stored as Vertex structs.
	//By using gl_Position we pass our vertex data(in this case just a position) next step of the rendering pipeline.
	
	gl_Position =  transform * vec4(pos, 1.0);
	texCoord0 = texCoord;

}