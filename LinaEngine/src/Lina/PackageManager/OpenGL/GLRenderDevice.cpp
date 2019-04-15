/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLRenderDevice
Timestamp: 4/14/2019 5:11:22 PM

*/

#include "LinaPch.hpp"
#include "GLRenderDevice.hpp"  

namespace LinaEngine
{


	/*unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

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
	};*/


/*	GLRenderDevice::GLRenderDevice(Application * app) : application(app)
	{
		LINA_CORE_TRACE("[Constructor] -> GLRenderDevice ({0})", typeid(*this).name());
	}

	GLRenderDevice::~GLRenderDevice()
	{
		LINA_CORE_TRACE("[Destructor] -> GLRenderDevice ({0})", typeid(*this).name());

		/*glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		*/
		//delete m_Window;
	//}

	/*Window* GLRenderDevice::CreateMainWindow(InputDevice* inputDevice, const WindowProperties & properties)
	{
		m_Window = new Window(properties, inputDevice);
		return m_Window;
	}

	void GLRenderDevice::Initialize()
	{
	/*	LINA_CORE_TRACE("[Initialization] -> GLRenderDevice ({0})", typeid(*this).name());
		m_Window->Initialize();

		sceneCamera.SetPerspectiveInformation(PerspectiveInformation(60.0f, m_Window->m_Properties.m_Width, m_Window->m_Properties.m_Height, 0.01f, 100.0f));
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

		ArrayBitmap bitmap;
		bitmap.Load(ResourceConstants::TexturesPath + "bricks.jpg");
		Texture(*this, bitmap, PixelFormat::FORMAT_RGB, true, false);

		//baseTexture = Texture2D(GL_TEXTURE0, true, "base1.png");
		//overlayTexture = Texture2D(GL_TEXTURE1, true, "overlay.png");

		basicShader.Use();
		basicShader.SetTextureUnit1();
		basicShader.SetTextureUnit2();

		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(1.0f, 0.0f, 0.0f) * 10.0f, Application::Get().GetInputDevice()->GetHorizontalInput()));
		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(0.0f, 1.0f, 0.0f) * 10.0f, Application::Get().GetInputDevice()->GetVerticalInput()));

		transformComponent.transform.SetPosition(Vector3F(0.0f, 0.0f, 10.0f));

		entity = ecs.MakeEntity(transformComponent, movementComponent);
		workingTransformation = ecs.GetComponent<TransformComponent>(entity)->transform;

		mainSystems.AddSystem(movementControlSystem);*/
/*
	}

	void GLRenderDevice::OnUpdate()
	{
		/*m_Window->OnUpdate();

		sceneCamera.OnInput(*Application::Get().GetInputDevice());
		glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		//baseTexture.Use();
		//overlayTexture.Use();
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
		*/
	/*}

	void GLRenderDevice::OnEvent(Event & e)
	{
		/*if (e.GetEventType() == EventType::WindowResize)
		{
			WindowResizeEvent& event = (WindowResizeEvent&)e;
			sceneCamera.SetPerspectiveInformation(PerspectiveInformation(60.0f, event.GetWidth(), event.GetHeight(), 0.01f, 100.0f));
		}*/
	/*}


	uint32 GLRenderDevice::CreateTexture2D(int32 width, int32 height, const void * data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
	{
		// Decrlare formats, target & handle
		GLint format = GetOpenGLFormat(pixelDataFormat);
		GLint internalFormat = GetOpenGLInternalFormat(internalPixelFormat, compress);
		GLenum textureTarget = GL_TEXTURE_2D;
		GLuint textureHandle;

		// OpenGL texture params.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);
		glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(textureTarget, 0, internalFormat, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);

		if (generateMipMaps) {
			glGenerateMipmap(textureTarget);
		}
		else {
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		return textureHandle;
	}

	uint32 GLRenderDevice::CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char * buffer, uint32 fourCC, uint32 mipMapCount)
	{
		GLint format;
		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			LINA_CORE_ERR("Invalid compression format for DDS texture\n");
			return 0;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;

		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;
		}

		return textureID;
	}

	uint32 GLRenderDevice::ReleaseTexture2D(uint32 texture2D)
	{
		if (texture2D == 0) {
			return 0;
		}
		glDeleteTextures(1, &texture2D);
		return 0;
	}

	void GLRenderDevice::SetShader(uint32 shader)
	{
		if (shader == m_BoundShader)
			return;

		glUseProgram(shader);
		m_BoundShader = shader;
	}


	static GLint GetOpenGLFormat(PixelFormat format)
	{
		switch (format) {
		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return GL_RG;
		case PixelFormat::FORMAT_RGB: return GL_RGB;
		case PixelFormat::FORMAT_RGBA: return GL_RGBA;
		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;

		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}

	static GLint GetOpenGLInternalFormat(PixelFormat format, bool compress)
	{
		switch (format) {
		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return GL_RG;
		case PixelFormat::FORMAT_RGB:
			if (compress) {
				return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			}
			else {
				return GL_RGB;
				//return GL_SRGB;
			}
		case PixelFormat::FORMAT_RGBA:
			if (compress) {
				// TODO: Decide on the default RGBA compression scheme
	//			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			}
			else {
				return GL_SRGB_ALPHA;
			}
		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}
	*/
}

