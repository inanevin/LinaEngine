/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLRenderEngine
Timestamp: 4/15/2019 12:37:37 PM

*/

#include "LinaPch.hpp"
#include "PackageManager/OpenGL/GLRenderEngine.hpp"  
#include "PackageManager/OpenGL/GLGraphicsDefines.hpp"


#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Components/MovementControlComponent.hpp"
#include "ECS/Components/RenderableMeshComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Systems/ECSMovementControlSystem.hpp"
#include "PackageManager/PAMInputEngine.hpp"
#include "Core/Application.hpp"

namespace LinaEngine::Graphics
{


	GLint GetOpenGLFormat(PixelFormat dataFormat);
	GLint GetOpenGLInternalFormat(PixelFormat internalFormat, bool compress);

	using namespace ECS;


	EntityComponentSystem ecs;
	EntityHandle entity;
	TransformComponent transformComponent;
	MovementControlComponent movementComponent;
	ECSSystemList mainSystems;
	ECSMovementControlSystem movementControlSystem;
	Transform* workingTransformation;


	GLRenderEngine::GLRenderEngine() : RenderEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> GLRenderEngine ({0})", typeid(*this).name());
		m_MainWindow = std::make_unique<PAMWindow>();
	}

	GLRenderEngine::~GLRenderEngine()
	{
		LINA_CORE_TRACE("[Destructor] -> GLRenderEngine ({0})", typeid(*this).name());
	}

	void GLRenderEngine::Initialize_Impl()
	{
		transformComponent.transform.SetPosition(Vector3F(0.0f, 0.0f, 10.0f));

		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(1.0f, 0.0f, 0.0f) * 3, Application::Get().GetInputDevice().GetHorizontalKeyAxis()));
		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(0.0f, 1.0f, 0.0f) * 3, Application::Get().GetInputDevice().GetVerticalKeyAxis()));

		entity = ecs.MakeEntity(transformComponent, movementComponent);
		workingTransformation = &ecs.GetComponent<TransformComponent>(entity)->transform;

		mainSystems.AddSystem(movementControlSystem);

	}

	void GLRenderEngine::Tick_Impl()
	{
		m_MainWindow->Tick();
		glClearColor(0.5f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ecs.UpdateSystems(mainSystems, 0.01f);
		LINA_CORE_TRACE("Position: {0} " , ecs.GetComponent<TransformComponent>(entity)->transform.GetPosition().ToString());
	}

	uint32 GLRenderEngine::CreateTexture2D_Impl(int32 width, int32 height, const void * data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
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

		if (generateMipMaps) 
		{
			glGenerateMipmap(textureTarget);
		}
		else 
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		return textureHandle;
	}

	uint32 GLRenderEngine::CreateDDSTexture2D_Impl(uint32 width, uint32 height, const unsigned char * buffer, uint32 fourCC, uint32 mipMapCount)
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

	uint32 GLRenderEngine::ReleaseTexture2D_Impl(uint32 texture2D)
	{
		if (texture2D == 0) {
			return 0;
		}
		glDeleteTextures(1, &texture2D);
		return 0;
	}

	uint32 GLRenderEngine::CreateVertexArray_Impl(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, int bufferUsage)
	{
		unsigned int numBuffers = numVertexComponents + numInstanceComponents + 1;

		GLuint VAO;
		GLuint* buffers = new GLuint[numBuffers];
		uintptr* bufferSizes = new uintptr[numBuffers];

		glGenVertexArrays(1, &VAO);
		setVAO(VAO);

		glGenBuffers(numBuffers, buffers);

		for (uint32 i = 0, attribute = 0; i < numBuffers - 1; i++) 
		{
			enum BufferUsage attribUsage = static_cast<BufferUsage>(bufferUsage);
			bool inInstancedMode = false;

			if (i >= numVertexComponents) 
			{
				attribUsage = BufferUsage::USAGE_DYNAMIC_DRAW;
				inInstancedMode = true;
			}

			uint32 elementSize = vertexElementSizes[i];
			const void* bufferData = inInstancedMode ? nullptr : vertexData[i];
			uintptr dataSize = inInstancedMode ? elementSize * sizeof(float) : elementSize * sizeof(float) * numVertices;

			glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
			glBufferData(GL_ARRAY_BUFFER, dataSize, bufferData, attribUsage);
			bufferSizes[i] = dataSize;

			uint32 elementSizeDiv = elementSize / 4;
			uint32 elementSizeRem = elementSize % 4;

			for (uint32 j = 0; j < elementSizeDiv; j++)
			{
				glEnableVertexAttribArray(attribute);
				glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE,	elementSize * sizeof(GLfloat), (const GLvoid*)(sizeof(GLfloat) * j * 4));

				if (inInstancedMode) 
					glVertexAttribDivisor(attribute, 1);

				attribute++;
			}
			if (elementSizeRem != 0)
			{
				glEnableVertexAttribArray(attribute);
				glVertexAttribPointer(attribute, elementSize, GL_FLOAT, GL_FALSE, elementSize * sizeof(GLfloat), (const GLvoid*)(sizeof(GLfloat) * elementSizeDiv * 4));
				
				if (inInstancedMode) 
					glVertexAttribDivisor(attribute, 1);
				
				attribute++;
			}
		}

		uintptr indicesSize = numIndices * sizeof(uint32);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, bufferUsage);

		bufferSizes[numBuffers - 1] = indicesSize;

		struct VertexArray vaoData;
		vaoData.buffers = buffers;
		vaoData.bufferSizes = bufferSizes;
		vaoData.numBuffers = numBuffers;
		vaoData.numElements = numIndices;
		vaoData.bufferUsage = bufferUsage;
		vaoData.instanceComponentsStartIndex = numVertexComponents;
		vaoMap[VAO] = vaoData;
		return VAO;
	}

	uint32 GLRenderEngine::ReleaseVertexArray_Impl(uint32 vao)
	{
		if (vao == 0) return 0;

		LinaMap<uint32, VertexArray>::iterator it = vaoMap.find(vao);

		if (it == vaoMap.end()) return 0;

		const struct VertexArray* vaoData = &it->second;

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(vaoData->numBuffers, vaoData->buffers);

		delete[] vaoData->buffers;
		delete[] vaoData->bufferSizes;

		vaoMap.erase(it);

		return 0;
	}

	void GLRenderEngine::UpdateVertexArray_Impl(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		if (vao == 0)  return;

		LinaMap<uint32, VertexArray>::iterator it = vaoMap.find(vao);
		if (it == vaoMap.end()) return;

		const struct VertexArray* vaoData = &it->second;
		enum BufferUsage usage;

		if (bufferIndex >= vaoData->instanceComponentsStartIndex) 
			usage = USAGE_DYNAMIC_DRAW;
		else 
			usage = static_cast<BufferUsage>(vaoData->bufferUsage);
		

		setVAO(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vaoData->buffers[bufferIndex]);

		if (vaoData->bufferSizes[bufferIndex] >= dataSize) 
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
			vaoData->bufferSizes[bufferIndex] = dataSize;
		}
	}

	void GLRenderEngine::SetShader_Impl(uint32 shader)
	{
		if (shader == m_BoundShader) return;

		glUseProgram(shader);
		m_BoundShader = shader;
	}

	void GLRenderEngine::SetShaderSampler_Impl(uint32 shader, const LinaString & samplerName, uint32 texture, uint32 sampler, uint32 unit)
	{
		SetShader_Impl(shader);
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindSampler(unit, sampler);
		glUniform1i(shaderProgramMap[shader].samplerMap[samplerName], unit);
	}

	static GLint GetOpenGLFormat(PixelFormat format)
	{
		switch (format)
		{

		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return GL_RG;
		case PixelFormat::FORMAT_RGB: return GL_RGB;
		case PixelFormat::FORMAT_RGBA: return GL_RGBA;
		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return 0; // GL_DEPTH_STENCIL;

		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}

	static GLint GetOpenGLInternalFormat(PixelFormat format, bool compress)
	{
		switch (format)
		{

		case PixelFormat::FORMAT_R: return GL_RED;

		case PixelFormat::FORMAT_RG: return  GL_RG;

		case PixelFormat::FORMAT_RGB:

			if (compress) return GL_COMPRESSED_RGB_S3TC_DXT1_EXT; // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			else return GL_RGB;

		case PixelFormat::FORMAT_RGBA:

			if (compress)  return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			else  return GL_SRGB_ALPHA; 

		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;

		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;

		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}


	void GLRenderEngine::setVAO(uint32 vao)
	{
		if (vao == m_BoundVAO) 	return;

		glBindVertexArray(vao);
		m_BoundVAO = vao;
	}


}

