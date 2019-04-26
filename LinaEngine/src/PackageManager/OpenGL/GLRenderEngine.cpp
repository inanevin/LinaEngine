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
#include "glad/glad.h"


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
	static bool addShader(GLuint shaderProgram, const LinaString& text, GLenum type, LinaArray<GLuint>* shaders);
	static void addAllAttributes(GLuint program, const LinaString& vertexShaderText, uint32 version);
	static bool checkShaderError(GLuint shader, int flag, bool isProgram, const LinaString& errorMessage);
	static void addShaderUniforms(GLuint shaderProgram, const LinaString& shaderText, LinaMap<LinaString, GLint>& uniformMap, LinaMap<LinaString, GLint>& samplerMap);

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
		glTexImage2D(textureTarget, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

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

	uint32 GLRenderEngine::CreateVertexArray_Impl(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage)
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
			BufferUsage attribUsage = bufferUsage;
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

	uint32 GLRenderEngine::CreateSampler_Impl(SamplerFilter minFilter, SamplerFilter magFilter, SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy)
	{
		uint32 result = 0;
		glGenSamplers(1, &result);
		glSamplerParameteri(result, GL_TEXTURE_WRAP_S, wrapU);
		glSamplerParameteri(result, GL_TEXTURE_WRAP_T, wrapV);
		glSamplerParameteri(result, GL_TEXTURE_MAG_FILTER, magFilter);
		glSamplerParameteri(result, GL_TEXTURE_MIN_FILTER, minFilter);
		if (anisotropy != 0.0f && minFilter != FILTER_NEAREST && minFilter != FILTER_LINEAR) {
			glSamplerParameterf(result, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
		}
		return result;
	}

	uint32 GLRenderEngine::ReleaseSampler_Impl(uint32 sampler)
	{
		if (sampler == 0) {
			return 0;
		}
		glDeleteSamplers(1, &sampler);
		return 0;
	}

	uint32 GLRenderEngine::CreateUniformBuffer_Impl(const void* data, uintptr dataSize, BufferUsage usage)
	{
		uint32 ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, data, usage);
		return ubo;
	}

	uint32 GLRenderEngine::ReleaseUniformBuffer_Impl(uint32 buffer)
	{
		if (buffer == 0) {
			return 0;
		}
		glDeleteBuffers(1, &buffer);
		return 0;
	}

	uint32 GLRenderEngine::CreateShaderProgram_Impl(const LinaString& shaderText)
	{
		GLuint shaderProgram = glCreateProgram();

		if (shaderProgram == 0)
		{
			LINA_CORE_ERR("Error creating shader program!");
			return (uint32)-1;
		}

		LinaString version = getShaderVersion();
		LinaString vertexShaderText = "#version " + version +
			"\n#define VS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;
		LinaString fragmentShaderText = "#version " + version +
			"\n#define FS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;

		ShaderProgram programData;
		if (!addShader(shaderProgram, vertexShaderText, GL_VERTEX_SHADER,
			&programData.shaders)) {
			return (uint32)-1;
		}
		if (!addShader(shaderProgram, fragmentShaderText, GL_FRAGMENT_SHADER,
			&programData.shaders)) {
			return (uint32)-1;
		}

		glLinkProgram(shaderProgram);
		if (checkShaderError(shaderProgram, GL_LINK_STATUS,
			true, "Error linking shader program")) {
			return (uint32)-1;
		}

		glValidateProgram(shaderProgram);
		if (checkShaderError(shaderProgram, GL_VALIDATE_STATUS,
			true, "Invalid shader program")) {
			return (uint32)-1;
		}

		addAllAttributes(shaderProgram, vertexShaderText, getVersion());
		addShaderUniforms(shaderProgram, shaderText, programData.uniformMap,
			programData.samplerMap);

		shaderProgramMap[shaderProgram] = programData;
		return shaderProgram;
	}

	uint32 GLRenderEngine::ReleaseShaderProgram_Impl(uint32 shader)
	{
		if (shader == 0) {
			return 0;
		}
		LinaMap<uint32, ShaderProgram>::iterator programIt = shaderProgramMap.find(shader);
		if (programIt == shaderProgramMap.end()) {
			return 0;
		}
		const struct ShaderProgram* shaderProgram = &programIt->second;

		for (LinaArray<uint32>::const_iterator it = shaderProgram->shaders.begin();
			it != shaderProgram->shaders.end(); ++it)
		{
			glDetachShader(shader, *it);
			glDeleteShader(*it);
		}
		glDeleteProgram(shader);
		shaderProgramMap.erase(programIt);
		return 0;
	}



	void GLRenderEngine::UpdateVertexArray_Impl(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		if (vao == 0)  return;

		LinaMap<uint32, VertexArray>::iterator it = vaoMap.find(vao);
		if (it == vaoMap.end()) return;

		const struct VertexArray* vaoData = &it->second;
		BufferUsage usage;

		if (bufferIndex >= vaoData->instanceComponentsStartIndex)
			usage = BufferUsage::USAGE_DYNAMIC_DRAW;
		else
			usage = vaoData->bufferUsage;
		

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

	void GLRenderEngine::SetShaderUniformBuffer_Impl(uint32 shader, const LinaString& uniformBufferName, uint32 buffer)
	{
		SetShader(shader);
		glBindBufferBase(GL_UNIFORM_BUFFER, shaderProgramMap[shader].uniformMap[uniformBufferName], buffer);
	}

	void GLRenderEngine::UpdateVertexArrayBuffer_Impl(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		if (vao == 0) {
			return;
		}

		LinaMap<uint32, VertexArray>::iterator it = vaoMap.find(vao);
		if (it == vaoMap.end()) {
			return;
		}
		const struct VertexArray* vaoData = &it->second;
		BufferUsage usage;

		if (bufferIndex >= vaoData->instanceComponentsStartIndex) {
			usage = BufferUsage::USAGE_DYNAMIC_DRAW;
		}
		else {
			usage = vaoData->bufferUsage;
		}

		setVAO(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vaoData->buffers[bufferIndex]);
		if (vaoData->bufferSizes[bufferIndex] >= dataSize) {
			glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
			vaoData->bufferSizes[bufferIndex] = dataSize;
		}
	}

	void GLRenderEngine::UpdateUniformBuffer_Impl(uint32 buffer, const void* data, uintptr dataSize)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		Memory::memcpy(dest, data, dataSize);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}


	void GLRenderEngine::setVAO(uint32 vao)
	{
		if (vao == m_BoundVAO) 	return;

		glBindVertexArray(vao);
		m_BoundVAO = vao;
	}

	LinaString GLRenderEngine::getShaderVersion()
	{
		if (!shaderVersion.empty()) {
			return shaderVersion;
		}

		uint32 version = getVersion();

		if (version >= 330) {
			shaderVersion = LinaEngine::Internal::ToString(version);
		}
		else if (version >= 320) {
			shaderVersion = "150";
		}
		else if (version >= 310) {
			shaderVersion = "140";
		}
		else if (version >= 300) {
			shaderVersion = "130";
		}
		else if (version >= 210) {
			shaderVersion = "120";
		}
		else if (version >= 200) {
			shaderVersion = "110";
		}
		else {
			int32 majorVersion = version / 100;
			int32 minorVersion = (version / 10) % 10;
			LINA_CORE_ERR( "Error: OpenGL Version {0}.{1} does not support shaders.\n", majorVersion, minorVersion);
			return "";
		}

		return shaderVersion;
	}

	uint32 GLRenderEngine::getVersion()
	{
		if (version != 0) {
			return version;
		}

		int32 majorVersion;
		int32 minorVersion;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		version = (uint32)(majorVersion * 100 + minorVersion * 10);
		return version;
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


	static bool addShader(GLuint shaderProgram, const LinaString& text, GLenum type, LinaArray<GLuint>* shaders)
	{
		GLuint shader = glCreateShader(type);

		if (shader == 0)
		{
			LINA_CORE_ERR("Error creating shader type {0}", type);
			return false;
		}

		const GLchar* p[1];
		p[0] = text.c_str();
		GLint lengths[1];
		lengths[0] = (GLint)text.length();

		glShaderSource(shader, 1, p, lengths);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar InfoLog[1024];

			glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
			LINA_CORE_ERR("Error compiling shader type {0}: '{1}'\n", shader, InfoLog);
			return false;
		}

		glAttachShader(shaderProgram, shader);
		shaders->push_back(shader);
		return true;
	}

	static bool checkShaderError(GLuint shader, int flag, bool isProgram, const LinaString& errorMessage)
	{
		GLint success = 0;
		GLchar error[1024] = { 0 };

		if (isProgram) {
			glGetProgramiv(shader, flag, &success);
		}
		else {
			glGetShaderiv(shader, flag, &success);
		}

		if (!success) {
			if (isProgram) {
				glGetProgramInfoLog(shader, sizeof(error), NULL, error);
			}
			else {
				glGetShaderInfoLog(shader, sizeof(error), NULL, error);
			}

			LINA_CORE_ERR("{0}: '{1}'\n", errorMessage.c_str(), error);
			return true;
		}
		return false;
	}

	static void addAllAttributes(GLuint program, const LinaString& vertexShaderText, uint32 version)
	{
		if (version >= 320) {
			// Layout is enabled. Return.
			return;
		}

		// FIXME: This code assumes attributes are listed in order, which isn't
		// true for all compilers. It's safe to ignore for now because OpenGL versions
		// requiring this aren't being used.
		GLint numActiveAttribs = 0;
		GLint maxAttribNameLength = 0;

		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);

		//	DEBUG_LOG_TEMP2("Adding attributes!");
		//	DEBUG_LOG_TEMP("%i %i", numActiveAttribs, maxAttribNameLength);
		LinaArray<GLchar> nameData(maxAttribNameLength);
		for (GLint attrib = 0; attrib < numActiveAttribs; ++attrib) {
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;

			glGetActiveAttrib(program, attrib, nameData.size(),
				&actualLength, &arraySize, &type, &nameData[0]);
			glBindAttribLocation(program, attrib, (char*)&nameData[0]);
			//		DEBUG_LOG_TEMP2("Adding attribute!");
			//		DEBUG_LOG_TEMP("%s: %d", (char*)&nameData[0], attrib);
		}
	}

	static void addShaderUniforms(GLuint shaderProgram, const LinaString& shaderText, LinaMap<LinaString, GLint>& uniformMap, LinaMap<LinaString, GLint>& samplerMap)
	{
		GLint numBlocks;
		glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
		for (int32 block = 0; block < numBlocks; ++block) {
			GLint nameLen;
			glGetActiveUniformBlockiv(shaderProgram, block,
				GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

			LinaArray<GLchar> name(nameLen);
			glGetActiveUniformBlockName(shaderProgram, block, nameLen, NULL, &name[0]);
			LinaString uniformBlockName((char*)&name[0], nameLen - 1);
			uniformMap[uniformBlockName] = glGetUniformBlockIndex(shaderProgram, &name[0]);
		}

		GLint numUniforms = 0;
		glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numBlocks);

		// Would get GL_ACTIVE_UNIFORM_MAX_LENGTH, but buggy on some drivers
		LinaArray<GLchar> uniformName(256);
		for (int32 uniform = 0; uniform < numUniforms; ++uniform) {
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;
			glGetActiveUniform(shaderProgram, uniform, uniformName.size(),
				&actualLength, &arraySize, &type, &uniformName[0]);
			if (type != GL_SAMPLER_2D) {
				LINA_CORE_ERR("Non-sampler2d uniforms currently unsupported!");
				continue;
			}
			LinaString name((char*)&uniformName[0], actualLength - 1);
			samplerMap[name] = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
		}
	}


}

