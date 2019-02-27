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
#include "Lina/Rendering/Vertex.hpp"
#include "Texture_OpenGL.hpp"
#include "PackageManager/Graphics/Shader_GLSLBasic.hpp"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

namespace LinaEngine
{
#define BIND_EVENT_FN(x) std::bind(&RenderingEngine_OpenGL::x, this, std::placeholders::_1)

	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	typedef Texture_OpenGL Texture2D;
	Texture2D containerTexture;
	Texture2D bricksTexture;
	Shader_GLSLBasic basicShader;

	RenderingEngine_OpenGL::RenderingEngine_OpenGL() : RenderingEngine()
	{
		
	}

	RenderingEngine_OpenGL::~RenderingEngine_OpenGL()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void RenderingEngine_OpenGL::Initialize()
	{

	}

	void RenderingEngine_OpenGL::Start()
	{
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, KEY_K, [this]() { this->Test(); });
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, KEY_K, std::bind(&Camera::OnKeyPress, cam, std::placeholders::_1));
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, std::bind(&Camera::OnKeyPress, cam, std::placeholders::_1));

		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, [this](int i) { cam.OnKeyPress(i); });
		//app->GetInputEngine().SubscribeToAction<int>(ActionType::KeyPressed, );

		//basicShader = Shader_GLSLBasic();


		basicShader.Initialize();

		// configure global opengl state
		// -----------------------------
		glEnable(GL_DEPTH_TEST);


		float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
		};

	

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		//glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);


		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);


		containerTexture = Texture2D(GL_TEXTURE0, true, "container.jpg");
		bricksTexture = Texture2D(GL_TEXTURE1, true, "awesomeface2.png");

		basicShader.SetTextureUnit1();
		basicShader.SetTextureUnit2();
	}

	void RenderingEngine_OpenGL::OnUpdate()
	{
		RenderingEngine::OnUpdate();

		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		containerTexture.Use();
		bricksTexture.Use();
		basicShader.Use();

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)m_WindowProps.Width / (float)m_WindowProps.Height, 0.1f, 100.0f);
	
		basicShader.SetModel(model);
		basicShader.SetView(view);
		basicShader.SetProjection(projection);

		glBindVertexArray(VAO);
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);


	}

	void RenderingEngine_OpenGL::OnWindowEvent(Event & e)
	{

	}


	

	void RenderingEngine_OpenGL::SetApplication(Application& p)
	{
		LINA_CORE_ASSERT(&p, "Application is nullptr!");
		this->app = &p;
	}

	void RenderingEngine_OpenGL::SetWireframeMode(bool activation)
	{
		if (activation)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		isWireframeModeActive = activation;
	}



}

#endif