/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLRenderingEngine
Timestamp: 1/2/2019 11:44:41 PM

*/
#include "LinaPch.hpp"

#ifdef LLF_GRAPHICS_OPENGL

#include "GLRenderingEngine.hpp"  
#include "Lina/Utility/Math/Color.hpp"
#include "Lina/ECS/Data/Transform.hpp"
#include "Lina/Core/Application.hpp"
#include "Lina/Input/InputEngine.hpp"
#include "Lina/Events/Action.hpp"
#include "Lina/Rendering/Vertex.hpp"
#include "Texture_OpenGL.hpp"
#include "PackageManager/Graphics/OpenGL/Shaders/Shader_GLSLBasic.hpp"
#include "glm/glm.hpp"
#include "Lina/ECS/ECS.hpp"
#include "Lina/ECS/Components/TransformComponent.hpp"
#include "Lina/ECS/Components/MovementControlComponent.hpp"
#include "Lina/ECS/Systems/ECSMovementControlSystem.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include "Lina/Camera.hpp"
#include "Lina/Events/Event.hpp"

namespace LinaEngine
{

	static GLint GetOpenGLFormat(LinaEngine::GLRenderingEngine::PixelFormat format);
	static GLint GetOpenGLInternalFormat(LinaEngine::GLRenderingEngine::PixelFormat format, bool compress);

	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	typedef Texture_OpenGL Texture2D;
	Texture2D baseTexture;
	Texture2D overlayTexture;
	Shader_GLSLBasic basicShader;
	Camera sceneCamera;

	ECS ecs;
	EntityHandle entity;
	TransformComponent transformComponent;
	MovementControlComponent movementComponent;
	ECSSystemList mainSystems;
	ECSMovementControlSystem movementControlSystem;
	Transform workingTransformation;


	Transform cubeTransforms[] = {
		Transform(Vector3F(0.0f,  0.0f,  0.0f)),
		Transform(Vector3F(2.0f,  5.0f, -15.0f)),
		Transform(Vector3F(-1.5f, -2.2f, -2.5f)),
		Transform(Vector3F(-3.8f, -2.0f, -12.3f)),
		Transform(Vector3F(2.4f, -0.4f, -3.5f)),
		Transform(Vector3F(-1.7f,  3.0f, -7.5f)),
		Transform(Vector3F(1.3f, -2.0f, -2.5f))
	};




	GLRenderingEngine::GLRenderingEngine() : RenderingEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> Rendering Engine OpenGL ({0})", typeid(*this).name());
		
	}

	GLRenderingEngine::~GLRenderingEngine()
	{
		LINA_CORE_TRACE("[Destructor] -> Rendering Engine OpenGL ({0})", typeid(*this).name());

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void GLRenderingEngine::Test()
	{
		
		
	}

	void GLRenderingEngine::Initialize()
	{
		RenderingEngine::Initialize();

		LINA_CORE_TRACE("[Initialization] -> Rendering Engine OpenGL ({0})", typeid(*this).name());
	}

	void GLRenderingEngine::Start()
	{
		LINA_CORE_TRACE("[Start] -> Rendering Engine OpenGL ({0})", typeid(*this).name());

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

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);



		baseTexture = Texture2D(GL_TEXTURE0, true, "base1.png");
		overlayTexture = Texture2D(GL_TEXTURE1, true, "overlay.png");

		basicShader.Use();
		basicShader.SetTextureUnit1();
		basicShader.SetTextureUnit2();

		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(1.0f, 0.0f, 0.0f) * 10.0f, Application::Get().GetInputEngine().GetHorizontalInput()));
		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(0.0f, 1.0f, 0.0f) * 10.0f, Application::Get().GetInputEngine().GetVerticalInput()));

		transformComponent.transform.SetPosition(Vector3F(0.0f, 0.0f, 10.0f));

		entity = ecs.MakeEntity(transformComponent, movementComponent);
		workingTransformation = ecs.GetComponent<TransformComponent>(entity)->transform;

		mainSystems.AddSystem(movementControlSystem);
	}

	void GLRenderingEngine::OnUpdate()
	{
		RenderingEngine::OnUpdate();

		sceneCamera.OnInput(app->GetInputEngine());
		glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		baseTexture.Use();
		overlayTexture.Use();
		basicShader.Use();


		glBindVertexArray(VAO);

		ecs.UpdateSystems(mainSystems, 0.01f);
		ecs.GetComponent<TransformComponent>(entity)->transform.Rotate(Vector3F::UP(), Math::Sin(0.2f) * 2);
		for (unsigned int i = 0; i < 1; i++)
		{
			Matrix4F transformation = ecs.GetComponent<TransformComponent>(entity)->transform.GetWorldTransformation();
			Matrix4F camView = sceneCamera.GetViewProjection();
			Matrix4F WVP = camView * transformation;
			basicShader.SetWVP(WVP);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

	}

	void GLRenderingEngine::OnWindowEvent(Event & e)
	{
		if (e.GetEventType() == EventType::WindowResize)
		{
			WindowResizeEvent& event = (WindowResizeEvent&)e;
			sceneCamera.SetPerspectiveInformation(PerspectiveInformation(60.0f, event.GetWidth(), event.GetHeight(), 0.01f, 100.0f));
		}
	}

	void GLRenderingEngine::OnEvent(Event & e)
	{
		if (e.GetEventType() == EventType::WindowResize)
		{
			WindowResizeEvent& event = (WindowResizeEvent&)e;
			sceneCamera.SetPerspectiveInformation(PerspectiveInformation(60.0f, event.GetWidth(), event.GetHeight(), 0.01f, 100.0f));
		}
	}


	void GLRenderingEngine::SetWireframeMode(bool activation)
	{
	/*	if (activation)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		isWireframeModeActive = activation;*/
	}

	uint32 GLRenderingEngine::CreateTexture2D(int32 width, int32 height, const void * data, int pixelDataFormat, int internalPixelFormat, bool generateMipMaps, bool compress)
	{

		GLint format = GetOpenGLFormat((PixelFormat)pixelDataFormat);
		GLint internalFormat = GetOpenGLInternalFormat((PixelFormat)internalPixelFormat, compress);
		GLenum textureTarget = GL_TEXTURE_2D;
		GLuint textureHandle;
		return 0;
	}


	static GLint GetOpenGLFormat(LinaEngine::GLRenderingEngine::PixelFormat format)
	{
		switch (format) {
		case GLRenderingEngine::FORMAT_R: return GL_RED;
		case GLRenderingEngine::FORMAT_RG: return GL_RG;
		case GLRenderingEngine::FORMAT_RGB: return GL_RGB;
		case GLRenderingEngine::FORMAT_RGBA: return GL_RGBA;
		case GLRenderingEngine::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case GLRenderingEngine::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;

		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}

	static GLint GetOpenGLInternalFormat(LinaEngine::GLRenderingEngine::PixelFormat format, bool compress)
	{
		switch (format) {
		case GLRenderingEngine::FORMAT_R: return GL_RED;
		case GLRenderingEngine::FORMAT_RG: return GL_RG;
		case GLRenderingEngine::FORMAT_RGB:
			if (compress) {
				//GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
				return GL_COMPRESSED_SRGB;
			}
			else {
				return GL_RGB;
				//return GL_SRGB;
			}
		case GLRenderingEngine::FORMAT_RGBA:
			if (compress) {
				// TODO: Decide on the default RGBA compression scheme
	//			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
				return GL_COMPRESSED_SRGB_ALPHA;
			}
			else {
				return GL_RGBA;
				//return GL_SRGB_ALPHA;
			}
		case GLRenderingEngine::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case GLRenderingEngine::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}
}

#endif