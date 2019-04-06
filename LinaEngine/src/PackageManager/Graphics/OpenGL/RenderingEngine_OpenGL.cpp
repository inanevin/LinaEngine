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
#include "PackageManager/Graphics/OpenGL/Shaders/Shader_GLSLBasic.hpp"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include "Lina/Camera.hpp"
#include "Lina/Transform.hpp"

namespace LinaEngine
{


	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	typedef Texture_OpenGL Texture2D;
	Texture2D baseTexture;
	Texture2D overlayTexture;
	Shader_GLSLBasic basicShader;
	Camera sceneCamera;
	Transform cubeTransforms[] = {
		Transform(Vector3F(0.0f,  0.0f,  0.0f)),
		Transform(Vector3F(2.0f,  5.0f, -15.0f)),
		Transform(Vector3F(-1.5f, -2.2f, -2.5f)),
		Transform(Vector3F(-3.8f, -2.0f, -12.3f)),
		Transform(Vector3F(2.4f, -0.4f, -3.5f)),
		Transform(Vector3F(-1.7f,  3.0f, -7.5f)),
		Transform(Vector3F(1.3f, -2.0f, -2.5f))
	};

	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	


	RenderingEngine_OpenGL::RenderingEngine_OpenGL() : RenderingEngine()
	{
		
	}

	RenderingEngine_OpenGL::~RenderingEngine_OpenGL()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void RenderingEngine_OpenGL::Test()
	{
		
		
	}

	void RenderingEngine_OpenGL::Initialize()
	{
		RenderingEngine::Initialize();
	}

	void RenderingEngine_OpenGL::Start()
	{
		/*ActionParams<int> params;
		params.binding = &keyData;
		params.actionType = ActionType::KeyPressed;
		params.condition = LINA_KEY_F2;
		params.callback = BIND_ACTION(RenderingEngine_OpenGL::Test, this);
		app->GetInputEngine().SubscribeToAction(params);

		test = new ActionTest();
		test->SetAction(&app->GetInputEngine());
		*/
		sceneCamera.SetPerspectiveInformation(PerspectiveInformation(60.0f, m_WindowProps.Width, m_WindowProps.Height, 0.01f, 100.0f));
		sceneCamera.m_Transform.SetPosition(0, 0, -6);


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

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
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

		baseTexture = Texture2D(GL_TEXTURE0, true, "base1.png");
		overlayTexture = Texture2D(GL_TEXTURE1, true, "overlay.png");

		basicShader.Use();
		basicShader.SetTextureUnit1();
		basicShader.SetTextureUnit2();
		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//	glBindVertexArray(0);

		
		
	}

	void RenderingEngine_OpenGL::OnUpdate()
	{
		RenderingEngine::OnUpdate();

		sceneCamera.OnInput(app->GetInputEngine());
		
		glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		baseTexture.Use();
		overlayTexture.Use();
		basicShader.Use();

		// camera/view transformation
		//glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//float radius = 10.0f;
		//float camX = sin(glfwGetTime()) * radius;
		//float camZ = cos(glfwGetTime()) * radius;
		//float camX = 0.0f;
		//float camZ = 0.0f;
		//view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//glm::mat4 projection = glm::mat4(1.0f);
		//projection = glm::perspective(glm::radians(45.0f), (float)m_WindowProps.Width / (float)m_WindowProps.Height, 0.1f, 100.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

		sceneCamera.m_Transform.SetRotationX(-15);
		//basicShader.SetView(view);
		//basicShader.SetProjection(projection);

		


		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < 7; i++)
		{
			Matrix4F transformation = cubeTransforms[i].GetWorldTransformation();
			Matrix4F camView = sceneCamera.GetViewProjection();
			Matrix4F WVP = camView * transformation;
			basicShader.SetWVP(WVP);

			/*glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;

			if (i == 2 || i == 5 || i == 8)
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			else
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			basicShader.SetModel(model);	*/
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



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
	/*	if (activation)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		isWireframeModeActive = activation;*/
	}



}

#endif