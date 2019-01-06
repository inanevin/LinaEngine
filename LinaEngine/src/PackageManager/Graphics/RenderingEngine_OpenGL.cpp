/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

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
#include "Lina/Utility/Math/Color.hpp"
#include "Shader_GLSL.hpp"
#include "Lina/Transform.hpp"
#include "Lina/Events/ApplicationEvent.hpp"

namespace LinaEngine
{
#define BIND_EVENT_FN(x) std::bind(&RenderingEngine_OpenGL::x, this, std::placeholders::_1)

	GLuint VBO;
	GLuint IBO;

	Transform t;

	RenderingEngine_OpenGL::RenderingEngine_OpenGL() : RenderingEngine()
	{
		// Initialize GLEW
		GLenum res = glewInit();
		LINA_CORE_ASSERT(res == GLEW_OK, "Glew is not initialized properly");

		CreateVertexBuffer();
		CreateIndexBuffer();

		// * ADD SHADERS, COMPILE, BIND *//

		test = new Shader_GLSL();

		test->AddShader(Shader_GLSL::LoadShader(ResourceConstants::GLSL_BasicVertexPath), GL_VERTEX_SHADER);
		test->AddShader(Shader_GLSL::LoadShader(ResourceConstants::GLSL_BasicFragmentPath), GL_FRAGMENT_SHADER);
		test->CompileShaders();
		test->Bind();

		test->AddUniform("gWVP", "mat4");

		PerspectiveInformation p;
		p.FOV = 60;
		p.zFar = 1000;
		p.zNear = 0.1f;
		p.width = GetMainWindow().GetWidth();
		p.height = GetMainWindow().GetHeight();
		
		cam.SetPerspectiveInformation(p);
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


	/*	// vertex attribute index 0 is fixed for vertex position, so activate the attribute.
		glEnableVertexAttribArray(0);

		// Update the pipeline state of the buffer we want to use.
		glBindBuffer(GL_ARRAY_BUFFER, m_VAO);

		// Tell the pipeline how to interpret the data. First attrib is 0 as we only use it for now, but it will be the index of shaders once they come into place.
		// 3 is # of components, xyz.
		// 4th param is whether to normalize the data, 5th is stride, # of bytes bw two instances of that attribute in the buffer. (Pass the size of struct, only one type of data for now.)
		// Last param is the offset inside the structure where the pipeline will find our attribute.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VAB);

		//glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);

		// Draw the geometry. Every vertex is one point. 0 is the index of the first vertex. 1 is the number of vertices to draw.
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Good practice to disable each vertex attribute when not used.
		glDisableVertexAttribArray(0);*/

		static float sc = 0.0f;


		t.SetScale(0.5f, 0.5f, 0.5f);
		t.SetPosition(0, 0.0f, 5.0f);
		t.SetRotation(0.0f, 0.0f, 0.0f);
	
		Matrix4F worldViewMatrix =  cam.GetViewProjection() * t.GetWorldTransformation();
		test->SetUniform("gWVP", worldViewMatrix);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);


		/* MAIN LOOP RENDER */

		RenderingEngine::OnUpdate();

	}

	void RenderingEngine_OpenGL::OnWindowEvent(Event & e)
	{
		
	}


	void RenderingEngine_OpenGL::CreateVertexBuffer()
	{
		/*Vector3F Vertices[3];
		Vertices[0] = Vector3F(-1.0f, -1.0f, 0.0f);
		Vertices[1] = Vector3F(1.0f, -1.0f, 0.0f);
		Vertices[2] = Vector3F(0.0f, 1.0f, 0.0f);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);*/

		Vector3F Vertices[4];
		Vertices[0] = Vector3F(-1.0f, -1.0f, 0.0f);
		Vertices[1] = Vector3F(0.0f, -1.0f, 1.0f);
		Vertices[2] = Vector3F(1.0f, -1.0f, 0.0f);
		Vertices[3] = Vector3F(0.0f, 1.0f, 0.0f);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

		/*
		
		Vector3F vertices[4];
		vertices[0] = Vector3F(-1.0f, -1.0f, 0.0f);
		vertices[1] = Vector3F(0.0f, -1.0f, 1.0f);
		vertices[2] = Vector3F(1.0f, -1.0f, 0.0f);
		vertices[3] = Vector3F(0.0f, 1.0f, 0.0f);

		int vSize = 4;

		// Generate buffers
		glGenBuffers(1, &m_VAO);

		// Bind buffer. VBO will contain array of vertices. (element buffer = array contains indices of vertices in another buffer)
		glBindBuffer(GL_ARRAY_BUFFER, m_VAO);

		// Fill the binded object with data.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vSize, vertices, GL_STATIC_DRAW);*/
	}
	void RenderingEngine_OpenGL::CreateIndexBuffer()
	{
		unsigned int Indices[] = { 0, 3, 1,
								   1, 3, 2,
								   2, 3, 0,
								   0, 1, 2 };

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	}

}

