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

#ifdef LLF_GRAPHICS_OPENGL

#include "RenderingEngine_OpenGL.hpp"  
#include "Lina/Utility/Math/Color.hpp"
#include "Lina/Transform.hpp"
#include "Lina/Application.hpp"
#include "Lina/Input/InputEngine.hpp"
#include "Lina/Events/Action.hpp"
#include "Shader_GLSLBasic.hpp"
#include "Lina/Rendering/Vertex.hpp"


namespace LinaEngine
{
#define BIND_EVENT_FN(x) std::bind(&RenderingEngine_OpenGL::x, this, std::placeholders::_1)

	GLuint VBO;
	GLuint IBO;
	Transform t;

	RenderingEngine_OpenGL::RenderingEngine_OpenGL() : RenderingEngine()
	{
		testTexture = Texture(GL_TEXTURE_2D);
	}

	RenderingEngine_OpenGL::~RenderingEngine_OpenGL()
	{
		ensure(test);
		delete test;
	}

	void RenderingEngine_OpenGL::Start()
	{
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, KEY_K, [this]() { this->Test(); });
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, KEY_K, std::bind(&Camera::OnKeyPress, cam, std::placeholders::_1));
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, std::bind(&Camera::OnKeyPress, cam, std::placeholders::_1));

		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, [this](int i) { cam.OnKeyPress(i); });
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, );

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		CreateVertexBuffer();
		CreateIndexBuffer();


		test = new Shader_GLSLBasic();
		test->Initialize();
		test->Enable();

		PerspectiveInformation p;
		p.FOV = 60;
		p.zFar = 1000;
		p.zNear = 0.1f;
		p.width = GetMainWindow().GetWidth();
		p.height = GetMainWindow().GetHeight();
		cam = Camera(p);

		testTexture.Load(ResourceConstants::BasicTexturePath);



	}

	void RenderingEngine_OpenGL::OnUpdate()
	{
		/* MAIN LOOP RENDER */
		// Clear buffer
		glClear(GL_COLOR_BUFFER_BIT);


		static float sc = 0.0f;


		t.SetScale(0.5f, 0.5f, 0.5f);
		t.SetPosition(0, 0.0f, 5.0f);
		t.SetRotation(0.0f, 0.0f, 0.0f);

		Matrix4F worldViewMatrix = cam.GetViewProjection() * t.GetWorldTransformation();

		test->SetWVP(worldViewMatrix);

		// vertex attribute index 0 is fixed for vertex position, so activate the attribute, 1 is texture coordinates..
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Update the pipeline state of the buffer we want to use.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);


		// Tell the pipeline how to interpret the data. First attrib is 0 as we only use it for now, but it will be the index of shaders once they come into place.
		// 3 is # of components, xyz.
		// 4th param is whether to normalize the data, 5th is stride, # of bytes bw two instances of that attribute in the buffer. (Pass the size of struct, only one type of data for now.)
		// Last param is the offset inside the structure where the pipeline will find our attribute.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		testTexture.Bind(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		// Good practice to disable each vertex attribute when not used.
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		cam.OnInput(app->GetInputEngine());

		
		RenderingEngine::OnUpdate();

	}

	void RenderingEngine_OpenGL::OnWindowEvent(Event & e)
	{

	}


	void RenderingEngine_OpenGL::CreateVertexBuffer()
	{


		Vertex Vertices[4] = { Vertex(Vector3F(-1.0f, -1.0f, 0.5773f), Vector2F(0.0f, 0.0f)),
							 Vertex(Vector3F(0.0f, -1.0f, -1.15475f), Vector2F(0.5f, 0.0f)),
							 Vertex(Vector3F(1.0f, -1.0f, 0.5773f),  Vector2F(1.0f, 0.0f)),
							 Vertex(Vector3F(0.0f, 1.0f, 0.0f),      Vector2F(0.5f, 1.0f)) };

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

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

	void RenderingEngine_OpenGL::SetApplication(Application& p)
	{
		check(&p);
		this->app = &p;
	}



}

#endif