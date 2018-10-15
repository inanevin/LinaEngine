#version 330 core //This line states we are using core functionality of OpenGL 3.3

//As we stated in glVertexAttribPointer we declare position at the location 0.
layout (location = 0) in vec3 aPos;




void main()
{
	//We are using built in GLSL variable gl_Position for passing our Position data which we stored as Vertex structs.
	//By using gl_Position we pass our vertex data(in this case just a position) next step of the rendering pipeline.
	gl_Position = vec4(0.25 * aPos, 1.0);

}