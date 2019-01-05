/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderingEngine_OpenGL
Timestamp: 1/2/2019 11:44:41 PM

*/
#include "LinaPch.hpp"
#include "RenderingEngine_OpenGL.hpp"  
#include "glew.h"
#include "Lina/Utility/Math/Color.hpp"
#include "Shaders/Shader_GLSL.hpp"

namespace LinaEngine
{
	GLuint VBO;

	RenderingEngine_OpenGL::RenderingEngine_OpenGL() : RenderingEngine()
	{
		// Initialize GLEW
		GLenum res = glewInit();
		LINA_CORE_ASSERT(res == GLEW_OK, "Glew is not initialized properly");

		test = new Shader_GLSL();

		/* CREATE VERTEX BUFFER */
		Vector3F vertices[3];
		vertices[0] = Vector3F(-1.0f, -1.0f, 0.0f);
		vertices[1] = Vector3F(1.0f, -1.0f, 0.0f);
		vertices[2] = Vector3F(0.0f, 1.0f, 0.0f);

		// Generate buffers
		glGenBuffers(1, &VBO);

		// Bind buffer. VBO will contain array of vertices. (element buffer = array contains indices of vertices in another buffer)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Fill the binded object with data.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		/* CREATE VERTEX BUFFER */

		// * ADD SHADERS, COMPILE, BIND *//

		
		test->AddShader(Shader_GLSL::LoadShader(ResourceConstants::GLSL_BasicVertexPath), GL_VERTEX_SHADER);
		test->AddShader(Shader_GLSL::LoadShader(ResourceConstants::GLSL_BasicFragmentPath), GL_FRAGMENT_SHADER);
		test->CompileShaders();
		test->Bind();

		

		test->AddUniform("gWorld", "mat4");
		

		// * ADD SHADERS, COMPILE, BIND *//

	}

	RenderingEngine_OpenGL::~RenderingEngine_OpenGL()
	{
		delete test;
	}

	void RenderingEngine_OpenGL::OnUpdate()
	{
		/* MAIN LOOP RENDER */
		// Clear buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// vertex attribute index 0 is fixed for vertex position, so activate the attribute.
		glEnableVertexAttribArray(0);

		// Update the pipeline state of the buffer we want to use.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Tell the pipeline how to interpret the data. First attrib is 0 as we only use it for now, but it will be the index of shaders once they come into place.
		// 3 is # of components, xyz.
		// 4th param is whether to normalize the data, 5th is stride, # of bytes bw two instances of that attribute in the buffer. (Pass the size of struct, only one type of data for now.)
		// Last param is the offset inside the structure where the pipeline will find our attribute.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Draw the geometry. Every vertex is one point. 0 is the index of the first vertex. 1 is the number of vertices to draw.
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Good practice to disable each vertex attribute when not used.
		glDisableVertexAttribArray(0);

		static float sc = 0.0f;
		sc += 0.01f;

		
		Matrix4F worldMatrix = Matrix4F().InitIdentityMatrix();
		test->SetUniform("gWorld", worldMatrix);

		/* MAIN LOOP RENDER */

		RenderingEngine::OnUpdate();


	}
}

