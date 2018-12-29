#version 330 core //This line states we are using core functionality of OpenGL 3.3

//As we stated in glVertexAttribPointer we declare position at the location 0.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

//In order to send data to the next step of the rendering pipeline we can use out word.
out float red;
out float blue;

out vec2 TexCoords;

void main()
{
	//We are using built in GLSL variable gl_Position for passing our Position data which we stored as Vertex structs.
	//By using gl_Position we pass our vertex data(in this case just a position) next step of the rendering pipeline.
	gl_Position = vec4(0.25 * aPos, 1.0);

	//We can give values to the data which we send to the fragment shader.
	red = aPos.x;
	blue = 0.5;

	TexCoords = aTexCoords;
}