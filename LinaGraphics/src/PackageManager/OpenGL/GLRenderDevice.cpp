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
Timestamp: 4/27/2019 10:16:32 PM

*/

#include "glad/glad.h"
#include "PackageManager/OpenGL/GLRenderDevice.hpp"  
#include "Utility/Math/Color.hpp"
#include "PackageManager/Generic/GenericMemory.hpp"
#include "Core/Internal.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "PackageManager/Generic/GenericMemory.hpp"

namespace LinaEngine::Graphics
{
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// MACRO DECLARATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

//#define MAKEFOURCC(a, b, c, d) ((uint32)(uint8)(a) | ((uint32)(uint8)(b) << 8) | ((uint32)(uint8)(c) << 16) | ((uint32)(uint8)(d) << 24 ))
//#define MAKEFOURCCDXT(a) MAKEFOURCC('D', 'X', 'T', a)
//#define MAKEFOURCC(a, b, c, d) ((uint32)(uint8)(a) | ((uint32)(uint8)(b) << 8) | ((uint32)(uint8)(c) << 16) | ((uint32)(uint8)(d) << 24 ))
#define MAKEFOURCCDXT(d) ((uint32)(uint8)('D') | ((uint32)(uint8)('X') << 8) | ((uint32)(uint8)('T') << 16) | ((uint32)(uint8)(d) << 24 )) 
#define FOURCC_DXT1 MAKEFOURCCDXT('1')
#define FOURCC_DXT2 MAKEFOURCCDXT('2')
#define FOURCC_DXT3 MAKEFOURCCDXT('3')
#define FOURCC_DXT4 MAKEFOURCCDXT('4')
#define FOURCC_DXT5 MAKEFOURCCDXT('5')



	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// HARCODED DATA
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------


	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// GLOBALS DECLARATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	GLint GetOpenGLFormat(PixelFormat dataFormat);
	GLint GetOpenGLInternalFormat(PixelFormat internalFormat, bool compress);
	static bool AddShader(GLuint shaderProgram, const std::string& text, GLenum type, LinaArray<GLuint>* shaders);
	static void AddAllAttributes(GLuint program, const std::string& vertexShaderText, uint32 version);
	static bool CheckShaderError(GLuint shader, int flag, bool isProgram, const std::string& errorMessage);
	static void AddShaderUniforms(GLuint shaderProgram, const std::string& shaderText, std::map<std::string, GLint>& uniformBlockMap, std::map<std::string, GLint>& uniformMap, std::map<std::string, GLint>& samplerMap);

	GLRenderDevice::GLRenderDevice()
	{
		LINA_CORE_TRACE("[Constructor] -> GLRenderDevice ({0})", typeid(*this).name());
		m_GLVersion = m_BoundFBO = m_BoundVAO = m_BoundShader = m_ViewportFBO = 0;
	}

	GLRenderDevice::~GLRenderDevice()
	{

	}


	void GLRenderDevice::Initialize(LinaEngine::ECS::LightingSystem& lightingSystemIn, int width, int height, DrawParams& defaultParams)
	{
		// Struct fbo data.
		struct FBOData fboWindowData;
		fboWindowData.width = width;
		fboWindowData.height = height;
		m_FBOMap[0] = fboWindowData;

		m_IsStencilTestEnabled = defaultParams.useStencilTest;
		m_IsDepthTestEnabled = defaultParams.useDepthTest;
		m_IsBlendingEnabled = (defaultParams.sourceBlend != BlendFunc::BLEND_FUNC_NONE || defaultParams.destBlend != BlendFunc::BLEND_FUNC_NONE);
		m_IsScissorsTestEnabled = defaultParams.useScissorTest;
		m_UsedDepthFunction = defaultParams.depthFunc;
		m_ShouldWriteDepth = defaultParams.shouldWriteDepth;
		m_usedStencilFail = defaultParams.stencilFail;
		m_UsedStencilPassButDepthFail = defaultParams.stencilPassButDepthFail;
		m_UsedStencilPass = defaultParams.stencilPass;
		m_UsedStencilFunction = defaultParams.stencilFunc;
		m_UsedStencilTestMask = defaultParams.stencilTestMask;
		m_UsedStencilWriteMask = defaultParams.stencilWriteMask;
		m_UsedStencilComparisonValue = defaultParams.stencilComparisonVal;
		m_UsedFaceCulling = defaultParams.faceCulling;
		m_UsedSourceBlending = defaultParams.sourceBlend;
		m_UsedDestinationBlending = defaultParams.destBlend;

		// Default GL settings.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(m_UsedFaceCulling);
		glEnable(GL_MULTISAMPLE);

		glDepthFunc(GL_LESS);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glFrontFace(GL_CW);
		if (m_UsedFaceCulling != FaceCulling::FACE_CULL_NONE)
			glCullFace(m_UsedFaceCulling);

		if (m_IsBlendingEnabled)
			glBlendFunc(m_UsedSourceBlending, m_UsedDestinationBlending);


		// Set lighting system reference.
		m_LightingSystem = &lightingSystemIn;

	}


	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// TEXTURE OPERATIONS
	// ---------------------------------------------------------------------


	uint32 GLRenderDevice::CreateTexture2D(Vector2 size, const void* data, SamplerParameters samplerParams, bool compress, bool useBorder, Color borderColor)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.textureParams.pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.textureParams.internalPixelFormat, compress);
		GLenum textureTarget = GL_TEXTURE_2D;
		GLuint textureHandle;

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);


		glTexImage2D(textureTarget, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, data);

		// OpenGL texture params.
		SetupTextureParameters(textureTarget, samplerParams);

		// Enable mipmaps if needed.
		if (samplerParams.textureParams.generateMipMaps)
			glGenerateMipmap(textureTarget);
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(textureTarget, 0);

		return textureHandle;
	}

	uint32 GLRenderDevice::CreateCubemapTexture(Vector2 size, SamplerParameters samplerParams, const LinaArray<int32*>& data, uint32 dataSize)
	{
		GLuint textureHandle;
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.textureParams.pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.textureParams.internalPixelFormat, false);

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

		// Loop through each face to gen. image.
		for (GLuint i = 0; i < dataSize; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, data[i]);
		}

		// Specify wrapping & filtering
		SetupTextureParameters(GL_TEXTURE_CUBE_MAP, samplerParams);

		if (samplerParams.textureParams.generateMipMaps)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		else
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		}


		glBindTexture(GL_TEXTURE_2D, 0);
		return textureHandle;
	}

	uint32 GLRenderDevice::CreateCubemapTextureEmpty(Vector2 size, SamplerParameters samplerParams)
	{
		GLuint textureHandle;
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.textureParams.pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.textureParams.internalPixelFormat, false);

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

		// Loop through each face to gen. image.
		for (GLuint i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size.x, size.y, 0, format, GL_FLOAT, NULL);
		}


		if (samplerParams.textureParams.generateMipMaps)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		else
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		return textureHandle;
	}

	uint32 GLRenderDevice::CreateTexture2DMSAA(Vector2 size, SamplerParameters samplerParams, int sampleCount)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.textureParams.pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.textureParams.internalPixelFormat, false);
		GLenum textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
		GLuint textureHandle;

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);

		// Create texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, internalFormat, size.x, size.y, GL_TRUE);

		// Setup texture params
		SetupTextureParameters(textureTarget, samplerParams);

		// Enable mipmaps if needed.
		if (samplerParams.textureParams.generateMipMaps)
			glGenerateMipmap(textureTarget);
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(textureTarget, 0);
		return textureHandle;
	}

	uint32 GLRenderDevice::CreateTexture2DEmpty(Vector2 size, SamplerParameters samplerParams)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.textureParams.pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.textureParams.internalPixelFormat, false);
		GLenum textureTarget = GL_TEXTURE_2D;
		GLuint textureHandle;

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);

		GLubyte texData[] = { 255, 255, 255, 255 };
		glTexImage2D(textureTarget, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, texData);

		// Setup parameters.
		SetupTextureParameters(textureTarget, samplerParams);

		// Enable mipmaps if needed.
		if (samplerParams.textureParams.generateMipMaps)
			glGenerateMipmap(textureTarget);
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(textureTarget, 0);
		return textureHandle;
	}

	void GLRenderDevice::SetupTextureParameters(uint32 textureTarget, SamplerParameters samplerParams)
	{
		// OpenGL texture params.
		glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, samplerParams.textureParams.minFilter);
		glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, samplerParams.textureParams.magFilter);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, samplerParams.textureParams.wrapS);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, samplerParams.textureParams.wrapT);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, samplerParams.textureParams.wrapR);
	}

	uint32 GLRenderDevice::ReleaseTexture2D(uint32 texture2D)
	{
		// Delete the texture binding if exists.
		if (texture2D == 0) return 0;
		glDeleteTextures(1, &texture2D);
		return 0;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// VERTEX ARRAY OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateVertexArray(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage)
	{
		// Define vertex array object, buffers, buffer count & their sizes.
		unsigned int numBuffers = numVertexComponents + numInstanceComponents + 1;
		GLuint VAO;
		GLuint* buffers = new GLuint[numBuffers];
		uintptr* bufferSizes = new uintptr[numBuffers];

		// Generate vertex array object and activate it, then generate necessary buffers.
		glGenVertexArrays(1, &VAO);
		SetVAO(VAO);
		glGenBuffers(numBuffers, buffers);

		// Define attribute for each buffer.
		for (uint32 i = 0, attribute = 0; i < numBuffers - 1; i++)
		{
			// Check vertex component count and switch to dynamic draw if current attribute exceeds. This means we are supposed to do instanced rendering.
			BufferUsage attribUsage = bufferUsage;
			bool inInstancedMode = false;
			if (i >= numVertexComponents)
			{
				attribUsage = BufferUsage::USAGE_DYNAMIC_DRAW;
				inInstancedMode = true;
			}

			// Define element size for the current buffers, as well as buffer data if applicable.
			uint32 elementSize = vertexElementSizes[i];
			const void* bufferData = inInstancedMode ? nullptr : vertexData[i];
			uintptr dataSize = inInstancedMode ? elementSize * sizeof(float) : elementSize * sizeof(float) * numVertices;

			// Bind the current array buffer & set the data.
			glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
			glBufferData(GL_ARRAY_BUFFER, dataSize, bufferData, attribUsage);
			bufferSizes[i] = dataSize;

			// Define element sizes to pass the required part of the array to the attrib pointer call.
			uint32 elementSizeDiv = elementSize / 4;
			uint32 elementSizeRem = elementSize % 4;

			// Attribute pointer for each block of elements.
			for (uint32 j = 0; j < elementSizeDiv; j++)
			{
				glEnableVertexAttribArray(attribute);
				glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE, elementSize * sizeof(GLfloat), (const GLvoid*)(sizeof(GLfloat) * j * 4));

				if (inInstancedMode)
					glVertexAttribDivisor(attribute, 1);

				attribute++;
			}

			// Last elements.
			if (elementSizeRem != 0)
			{
				glEnableVertexAttribArray(attribute);
				glVertexAttribPointer(attribute, elementSize, GL_FLOAT, GL_FALSE, elementSize * sizeof(GLfloat), (const GLvoid*)(sizeof(GLfloat) * elementSizeDiv * 4));

				if (inInstancedMode)
					glVertexAttribDivisor(attribute, 1);

				attribute++;
			}
		}

		// Finally bind the element array buffer.
		uintptr indicesSize = numIndices * sizeof(uint32);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, bufferUsage);
		bufferSizes[numBuffers - 1] = indicesSize;

		// Create vertex array based on our calculated data.
		struct VertexArrayData vaoData;
		vaoData.buffers = buffers;
		vaoData.bufferSizes = bufferSizes;
		vaoData.numBuffers = numBuffers;
		vaoData.numElements = numIndices;
		vaoData.bufferUsage = bufferUsage;
		vaoData.instanceComponentsStartIndex = numVertexComponents;

		// Store the array in our map & return the modified vertex array object.
		m_VAOMap[VAO] = vaoData;
		return VAO;
	}


	uint32 GLRenderDevice::ReleaseVertexArray(uint32 vao, bool checkMap)
	{
		if (!checkMap)
		{
			glDeleteVertexArrays(1, &vao);
			return 0;
		}

		// Terminate if vao is null or does not exist in our mapped objects.
		if (vao == 0) return 0;
		std::map<uint32, VertexArrayData>::iterator it = m_VAOMap.find(vao);
		if (it == m_VAOMap.end()) return 0;

		// Get the vertex array object data from the map.
		const struct VertexArrayData* vaoData = &it->second;

		// Delete the VA & buffers, then data.
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(vaoData->numBuffers, vaoData->buffers);
		delete[] vaoData->buffers;
		delete[] vaoData->bufferSizes;

		// Remove from the map.
		m_VAOMap.erase(it);
		return 0;
	}

	uint32 GLRenderDevice::CreateSkyboxVertexArray()
	{
		unsigned int skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		//glBindVertexArray(skyboxVAO);
		SetVAO(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		return skyboxVAO;
	}

	uint32 GLRenderDevice::CreateScreenQuadVertexArray()
	{
		// screen quad VAO
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		SetVAO(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		return quadVAO;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// TEXTURE SAMPLER OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateSampler(SamplerParameters samplerParams)
	{
		// OpenGL Texture Sampler parameters.
		uint32 result = 0;
		glGenSamplers(1, &result);
		glSamplerParameteri(result, GL_TEXTURE_WRAP_S, samplerParams.textureParams.wrapS);
		glSamplerParameteri(result, GL_TEXTURE_WRAP_T, samplerParams.textureParams.wrapT);
		glSamplerParameteri(result, GL_TEXTURE_MAG_FILTER, samplerParams.textureParams.magFilter);
		glSamplerParameteri(result, GL_TEXTURE_MIN_FILTER, samplerParams.textureParams.minFilter);

		// Set anisotropy if applicable.
		if (samplerParams.anisotropy != 0.0f && samplerParams.textureParams.minFilter != FILTER_NEAREST && samplerParams.textureParams.minFilter != FILTER_LINEAR)
			glSamplerParameterf(result, GL_TEXTURE_MAX_ANISOTROPY, samplerParams.anisotropy);

		return result;
	}

	uint32 GLRenderDevice::ReleaseSampler(uint32 sampler)
	{
		// Delete the sampler binding if exists.
		if (sampler == 0) return 0;
		glDeleteSamplers(1, &sampler);
		return 0;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// UNFORM BUFFER OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateUniformBuffer(const void* data, uintptr dataSize, BufferUsage usage)
	{
		// Bind a new uniform buffer to GL.
		uint32 ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, data, usage);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		return ubo;
	}

	uint32 GLRenderDevice::ReleaseUniformBuffer(uint32 buffer)
	{
		// Delete the buffer if exists.
		if (buffer == 0) return 0;
		glDeleteBuffers(1, &buffer);
		return 0;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// SHADER PROGRAM OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateShaderProgram(const std::string& shaderText, bool usesGeometryShader)
	{
		// Shader program instance.
		GLuint shaderProgram = glCreateProgram();

		if (shaderProgram == 0)
		{
			LINA_CORE_ERR("Error creating shader program!");
			return (uint32)-1;
		}

		// Modify the shader text to include the version data.
		std::string version = GetShaderVersion();
		std::string vertexShaderText = "#version " + version + "\n#define VS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;
		std::string fragmentShaderText = "#version " + version + "\n#define FS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;

		// Add the shader program, terminate if fails.
		ShaderProgram programData;
		if (!AddShader(shaderProgram, vertexShaderText, GL_VERTEX_SHADER, &programData.shaders))
			return (uint32)-1;

		if (usesGeometryShader)
		{
			std::string geometryShaderText = "#version " + version + "\n#define GS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;
			if (!AddShader(shaderProgram, geometryShaderText, GL_GEOMETRY_SHADER, &programData.shaders))
				return (uint32)-1;
		}

		if (!AddShader(shaderProgram, fragmentShaderText, GL_FRAGMENT_SHADER, &programData.shaders))
			return (uint32)-1;

		// Link program & check link errors.
		glLinkProgram(shaderProgram);

		if (CheckShaderError(shaderProgram, GL_LINK_STATUS, true, "Error linking shader program"))
			return (uint32)-1;

		// Validate program & check validation errors.
		glValidateProgram(shaderProgram);

		if (CheckShaderError(shaderProgram, GL_VALIDATE_STATUS, true, "Invalid shader program"))
			return (uint32)-1;

		// Bind attributes for GL & add shader uniforms.
		AddAllAttributes(shaderProgram, vertexShaderText, GetVersion());
		AddShaderUniforms(shaderProgram, shaderText, programData.uniformBlockMap, programData.uniformMap, programData.samplerMap);

		// Store the program in our map & return it.
		m_ShaderProgramMap[shaderProgram] = programData;
		return shaderProgram;
	}

	uint32 GLRenderDevice::ReleaseShaderProgram(uint32 shader)
	{
		// Terminate if shader is not valid or does not exist in our map.
		if (shader == 0) return 0;
		std::map<uint32, ShaderProgram>::iterator programIt = m_ShaderProgramMap.find(shader);
		if (programIt == m_ShaderProgramMap.end()) return 0;

		// Get the program from the map.
		const ShaderProgram* shaderProgram = &programIt->second;

		// Detach & delete each shader assigned to our program.
		for (LinaArray<uint32>::const_iterator it = shaderProgram->shaders.begin(); it != shaderProgram->shaders.end(); ++it)
		{
			glDetachShader(shader, *it);
			glDeleteShader(*it);
		}

		// Delete the program, erase from our map & return.
		glDeleteProgram(shader);
		m_ShaderProgramMap.erase(programIt);
		return 0;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// FRAME BUFFER OBJECT OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateRenderTarget(uint32 texture, int32 width, int32 height, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel, bool noReadWrite, bool bindRBO, FrameBufferAttachment rboAttachment, uint32 rbo)
	{
		// Generate frame buffers & set the current object.
		uint32 fbo;
		glGenFramebuffers(1, &fbo);
		SetFBO(fbo);

		// Define attachment type & use the buffer.
		GLenum attachmentTypeGL = attachment + attachmentNumber;

		if (bindTextureMode != TextureBindMode::BINDTEXTURE_NONE)
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTypeGL, bindTextureMode, texture, mipLevel);
		else
			glFramebufferTexture(GL_FRAMEBUFFER, attachmentTypeGL, texture, mipLevel);

		// Disable read&write.
		if (noReadWrite)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		// Set the render buffer object if desired.
		if (bindRBO)
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, rboAttachment, GL_RENDERBUFFER, rbo);

		// Err check
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LINA_CORE_ERR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		// Define frame buffer object data and store it in our map.
		FBOData data;
		data.width = width;
		data.height = height;
		m_FBOMap[fbo] = data;
		SetFBO(0);
		return fbo;
	}

	void GLRenderDevice::BindTextureToRenderTarget(uint32 fbo, uint32 texture, Vector2 size, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, int mipLevel)
	{
		if ((m_FBOMap.find(fbo) == m_FBOMap.end()))
		{
			LINA_CORE_ERR("Frame buffer {0} is not generated, you can't attach a texture to non existing frame buffer!", fbo);
			return;
		}

		SetFBO(fbo);
		GLenum attachmentTypeGL = attachment + attachmentNumber;

		glBindTexture(GL_TEXTURE_2D, texture);
		if (bindTextureMode != TextureBindMode::BINDTEXTURE_NONE)
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTypeGL, bindTextureMode, texture, mipLevel);
		else
			glFramebufferTexture(GL_FRAMEBUFFER, attachmentTypeGL, texture, mipLevel);

		
		SetFBO(0);
	}

	void GLRenderDevice::MultipleDrawBuffersCommand(uint32 fbo, uint32 bufferCount, uint32* attachments)
	{
		if ((m_FBOMap.find(fbo) == m_FBOMap.end()))
		{
			LINA_CORE_ERR("Frame buffer {0} is not generated, you can't attach a texture to non existing frame buffer!", fbo);
			return;
		}
		SetFBO(fbo);

		unsigned int attachmentss[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		//glDrawBuffers(bufferCount, attachmentss);
		SetFBO(0);
	}

	uint32 GLRenderDevice::ReleaseRenderTarget(uint32 fbo)
	{
		// Terminate if fbo is not valid or does not exist in our map.
		if (fbo == 0) return 0;
		std::map<uint32, FBOData>::iterator it = m_FBOMap.find(fbo);
		if (it == m_FBOMap.end()) return 0;

		// Delete the frame buffer object, erase from our map & return.
		glDeleteFramebuffers(1, &fbo);
		m_FBOMap.erase(it);
		return 0;
	}

	uint32 GLRenderDevice::CreateRenderBufferObject(RenderBufferStorage storage, uint32 width, uint32 height, int sampleCount)
	{
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		if (sampleCount == 0)
			glRenderbufferStorage(GL_RENDERBUFFER, storage, width, height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, storage, width, height);

		return rbo;
	}

	uint32 GLRenderDevice::ReleaseRenderBufferObject(uint32 target)
	{
		glDeleteRenderbuffers(1, &target);
		return 0;
	}

	void GLRenderDevice::BlitFrameBuffers(uint32 readFBO, uint32 readWidth, uint32 readHeight, uint32 writeFBO, uint32 writeWidth, uint32 writeHeight, BufferBit mask, SamplerFilter filter)
	{
		if (m_BoundReadFBO != readFBO)
		{
			m_BoundReadFBO = readFBO;
			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
		}

		if (m_BoundWriteFBO != writeFBO)
		{
			m_BoundWriteFBO = writeFBO;
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO);
		}
		glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, writeWidth, writeHeight, mask, filter);
	}


	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// VERTEX ARRAY OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------


	void GLRenderDevice::UpdateVertexArray(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		// Terminate if fbo is not valid or does not exist in our map.
		if (vao == 0)  return;
		std::map<uint32, VertexArrayData>::iterator it = m_VAOMap.find(vao);
		if (it == m_VAOMap.end()) return;

		// Get the vertex array object data from the map.
		const VertexArrayData* vaoData = &it->second;

		// Define a usage according to the VAO data.
		BufferUsage usage;
		if (bufferIndex >= vaoData->instanceComponentsStartIndex)
			usage = BufferUsage::USAGE_DYNAMIC_DRAW;
		else
			usage = vaoData->bufferUsage;

		// Use VAO & bind its corresponding buffer.
		SetVAO(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vaoData->buffers[bufferIndex]);

		// If buffer size exceeds data size use it as subdata.
		if (vaoData->bufferSizes[bufferIndex] >= dataSize)
			glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
		else
		{
			glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
			vaoData->bufferSizes[bufferIndex] = dataSize;
		}
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// SHADER OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	void GLRenderDevice::SetShader(uint32 shader)
	{
		// Use the target shader if exists.
		if (shader == m_BoundShader) return;
		glUseProgram(shader);
		m_BoundShader = shader;
	}

	void GLRenderDevice::SetTexture(uint32 texture, uint32 sampler, uint32 unit, TextureBindMode bindTextureMode, bool setSampler)
	{

		if (m_BoundTextureUnit != unit)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			m_BoundTextureUnit = unit;
		}

		if (m_BoundTextures[unit] != texture)
		{
			glBindTexture(bindTextureMode, texture);
			m_BoundTextures[unit] = texture;
		}

		if (setSampler)
		{
			if (m_BoundSamplers[unit] != sampler)
			{
				glBindSampler(unit, sampler);
				m_BoundSamplers[unit] = sampler;
			}
		}
	}

	void GLRenderDevice::SetShaderUniformBuffer(uint32 shader, const std::string& uniformBufferName, uint32 buffer)
	{
		// Use shader first.
		SetShader(shader);

		// Update the uniform data.
		glBindBufferBase(GL_UNIFORM_BUFFER, m_ShaderProgramMap[shader].uniformBlockMap[uniformBufferName], buffer);
	}

	void GLRenderDevice::BindUniformBuffer(uint32 bufferObject, uint32 point)
	{
		// Bind the buffer object to the point.
		glBindBufferBase(GL_UNIFORM_BUFFER, point, bufferObject);
	}

	void GLRenderDevice::BindShaderBlockToBufferPoint(uint32 shader, uint32 blockPoint, std::string& blockName)
	{
		glUniformBlockBinding(shader, m_ShaderProgramMap[shader].uniformBlockMap[blockName], blockPoint);
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// UNIFORM BUFFER OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	void GLRenderDevice::UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
	{
		// Terminate if VAO is not valid or does not exist in our map.
		if (vao == 0) return;
		std::map<uint32, VertexArrayData>::iterator it = m_VAOMap.find(vao);
		if (it == m_VAOMap.end()) return;

		// Get VAO data from the map.
		const struct VertexArrayData* vaoData = &it->second;

		BufferUsage usage;

		// Check usage & enable dynamic draw if is needed to be instanced.
		if (bufferIndex >= vaoData->instanceComponentsStartIndex)
			usage = BufferUsage::USAGE_DYNAMIC_DRAW;
		else
			usage = vaoData->bufferUsage;

		SetVAO(vao);

		// Use VAO & bind buffer.
		glBindBuffer(GL_ARRAY_BUFFER, vaoData->buffers[bufferIndex]);

		// If buffer size exceeds data size use it as subdata.
		if (vaoData->bufferSizes[bufferIndex] >= dataSize)
			glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
		else
		{
			glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
			vaoData->bufferSizes[bufferIndex] = dataSize;
		}
	}

	void GLRenderDevice::UpdateUniformBuffer(uint32 buffer, const void* data, uintptr offset, uintptr dataSize)
	{
		// Get buffer & set data.
		if (m_BoundUBO != buffer)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);
			m_BoundUBO = buffer;
		}

		glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void GLRenderDevice::UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize)
	{
		void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		GenericMemory::memcpy(dest, data, dataSize);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// DRAWING OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------


	void GLRenderDevice::Draw(uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements, bool drawArrays)
	{
		// No need to draw nothin dude.
		if (!drawArrays && numInstances == 0) return;

		// Setup draw parameters.
		SetFaceCulling(drawParams.faceCulling);
		SetBlending(drawParams.sourceBlend, drawParams.destBlend);
		SetScissorTest(drawParams.useScissorTest, drawParams.scissorStartX, drawParams.scissorStartY, drawParams.scissorWidth, drawParams.scissorHeight);

		if (drawParams.useDepthTest)
		{
			SetDepthTestEnable(true);
			SetDepthTest(drawParams.shouldWriteDepth, drawParams.depthFunc);
		}
		else
			SetDepthTestEnable(false);

		SetStencilTest(drawParams.useStencilTest, drawParams.stencilFunc, drawParams.stencilTestMask, drawParams.stencilWriteMask, drawParams.stencilComparisonVal, drawParams.stencilFail, drawParams.stencilPassButDepthFail, drawParams.stencilPass);

		// Set vao & draw
		SetVAO(vao);

		if (drawArrays)
			glDrawArrays(GL_TRIANGLES, 0, numElements);
		else
		{
			// 1 object or instanced draw calls?
			if (numInstances == 1)
				glDrawElements(drawParams.primitiveType, (GLsizei)numElements, GL_UNSIGNED_INT, 0);
			else
				glDrawElementsInstanced(drawParams.primitiveType, (GLsizei)numElements, GL_UNSIGNED_INT, 0, numInstances);
		}


	}

	void GLRenderDevice::Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const Color& color, uint32 stencil)
	{
		// Make sure frame buffer objects are used.
		uint32 flags = 0;

		// Set flags according to options.
		if (shouldClearColor)
		{
			flags |= GL_COLOR_BUFFER_BIT;

			if (color != m_CurrentClearColor)
			{
				glClearColor((GLfloat)color.r, (GLfloat)color.g, (GLfloat)color.b, (GLfloat)color.a);
				m_CurrentClearColor = color;
			}
		}
		if (shouldClearDepth)
			flags |= GL_DEPTH_BUFFER_BIT;
		if (shouldClearStencil)
		{
			flags |= GL_STENCIL_BUFFER_BIT;
			SetStencilWriteMask(stencil);
		}

		// Clear the desired flags.
		glClear(flags);
	}

	void GLRenderDevice::OnWindowResized(float width, float height)
	{
		for (uint32 i = 0; i < m_FBOMap.size(); i++)
		{
			m_FBOMap[i].width = (int32)width;
			m_FBOMap[i].height = (int32)height;
		}
		glViewport(0, 0, width, height);
	}


	void GLRenderDevice::UpdateShaderUniformFloat(uint32 shader, const std::string& uniform, const float f)
	{
		glUniform1f(m_ShaderProgramMap[shader].uniformMap[uniform], (GLfloat)f);
	}

	void GLRenderDevice::UpdateShaderUniformInt(uint32 shader, const std::string& uniform, const int f)
	{
		glUniform1i(m_ShaderProgramMap[shader].uniformMap[uniform], (GLint)f);
	}

	void GLRenderDevice::UpdateShaderUniformColor(uint32 shader, const std::string& uniform, const Color& color)
	{
		glUniform3f(m_ShaderProgramMap[shader].uniformMap[uniform], (GLfloat)color.r, (GLfloat)color.g, (GLfloat)color.b);
	}

	void GLRenderDevice::UpdateShaderUniformVector2(uint32 shader, const std::string& uniform, const Vector2& m)
	{
		glUniform2f(m_ShaderProgramMap[shader].uniformMap[uniform], (GLfloat)m.x, (GLfloat)m.y);
	}

	void GLRenderDevice::UpdateShaderUniformVector3(uint32 shader, const std::string& uniform, const Vector3& m)
	{
		glUniform3f(m_ShaderProgramMap[shader].uniformMap[uniform], (GLfloat)m.x, (GLfloat)m.y, (GLfloat)m.z);
	}

	void GLRenderDevice::UpdateShaderUniformVector4F(uint32 shader, const std::string& uniform, const Vector4& m)
	{
		glUniform4f(m_ShaderProgramMap[shader].uniformMap[uniform], (GLfloat)m.x, (GLfloat)m.y, (GLfloat)m.z, (GLfloat)m.w);

	}

	void GLRenderDevice::UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, void* data)
	{
		float* matrixData = ((float*)data);
		glUniformMatrix4fv(m_ShaderProgramMap[shader].uniformMap[uniform], 1, GL_FALSE, matrixData);
	}

	void GLRenderDevice::UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, const Matrix& m)
	{
		glUniformMatrix4fv(m_ShaderProgramMap[shader].uniformMap[uniform], 1, GL_FALSE, &m[0][0]);
	}


	void GLRenderDevice::SetVAO(uint32 vao)
	{
		// Use VAO if exists.
		if (vao == m_BoundVAO) 	return;
		glBindVertexArray(vao);
		m_BoundVAO = vao;
	}

	void GLRenderDevice::SetFBO(uint32 fbo)
	{
		// Use FBO if exists.
		if (fbo == m_BoundFBO) return;
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		m_BoundFBO = m_BoundReadFBO = m_BoundWriteFBO = fbo;
		SetViewport(fbo);
	}


	void GLRenderDevice::SetRBO(uint32 rbo)
	{
		if (rbo == m_BoundRBO) return;
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		m_BoundRBO = rbo;
	}

	void GLRenderDevice::SetViewport(uint32 fbo)
	{
		// Update viewport according to the render targets if exist.
		if (fbo == m_ViewportFBO) return;
		m_ViewportFBO = fbo;

		uint32 w = m_FBOMap[fbo].width;
		uint32 h = m_FBOMap[fbo].height;

		if (w != m_BoundViewportSize.x || h != m_BoundViewportSize.y)
		{
			m_BoundViewportSize.x = w;
			m_BoundViewportSize.y = h;
			glViewport(0, 0, m_FBOMap[fbo].width, m_FBOMap[fbo].height);
		}

	}

	void GLRenderDevice::SetFaceCulling(FaceCulling faceCulling)
	{
		// If target is enabled, then disable face culling.
		// If current is disabled, then enable faceculling.
		// Else switch cull state.
		if (faceCulling == FACE_CULL_NONE)
		{
			glDisable(GL_CULL_FACE);
			m_UsedFaceCulling = FACE_CULL_NONE;
			return;
		}
		else if (m_UsedFaceCulling == FACE_CULL_NONE)// Face culling is disabled but needs to be enabled
			glEnable(GL_CULL_FACE);

		if (m_UsedFaceCulling != faceCulling)
		{
			m_UsedFaceCulling = faceCulling;
			glCullFace(faceCulling);
		}
	}

	void GLRenderDevice::SetDepthTest(bool shouldWrite, DrawFunc depthFunc)
	{

		// Toggle dept writing.
		if (shouldWrite != m_ShouldWriteDepth)
		{
			glDepthMask(shouldWrite ? GL_TRUE : GL_FALSE);
			m_ShouldWriteDepth = shouldWrite;
		}

		// Update if change is needed.
		if (depthFunc == m_UsedDepthFunction)	return;

		glDepthFunc(depthFunc);
		m_UsedDepthFunction = depthFunc;
	}

	void GLRenderDevice::SetDepthTestEnable(bool enable)
	{
		if (m_IsDepthTestEnabled != enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			m_IsDepthTestEnabled = enable;
		}
	}
	void GLRenderDevice::SetBlending(BlendFunc sourceBlend, BlendFunc destBlend)
	{
		// If no change is needed return.
		if (sourceBlend == m_UsedSourceBlending && destBlend == m_UsedDestinationBlending) return;
		else if (sourceBlend == BLEND_FUNC_NONE || destBlend == BLEND_FUNC_NONE)
			glDisable(GL_BLEND);
		else if (m_UsedSourceBlending == BLEND_FUNC_NONE || m_UsedDestinationBlending == BLEND_FUNC_NONE)
		{
			glEnable(GL_BLEND);
			glBlendFunc(sourceBlend, destBlend);
		}
		else
			glBlendFunc(sourceBlend, destBlend);


		m_UsedSourceBlending = sourceBlend;
		m_UsedDestinationBlending = destBlend;
	}

	void GLRenderDevice::SetStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass)
	{
		// If change is needed toggle enabled state & enable/disable stencil test.
		if (enable != m_IsStencilTestEnabled)
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			m_IsStencilTestEnabled = enable;
		}

		// Set stencil params.
		if (stencilFunc != m_UsedStencilFunction || stencilTestMask != m_UsedStencilTestMask || stencilComparisonVal != m_UsedStencilComparisonValue)
		{
			glStencilFunc(stencilFunc, stencilComparisonVal, stencilTestMask);
			m_UsedStencilComparisonValue = stencilComparisonVal;
			m_UsedStencilTestMask = stencilTestMask;
			m_UsedStencilFunction = stencilFunc;
		}

		if (stencilFail != m_usedStencilFail || stencilPass != m_UsedStencilPass || stencilPassButDepthFail != m_UsedStencilPassButDepthFail)
		{
			glStencilOp(stencilFail, stencilPassButDepthFail, stencilPass);
			m_usedStencilFail = stencilFail;
			m_UsedStencilPass = stencilPass;
			m_UsedStencilPassButDepthFail = stencilPassButDepthFail;
		}

		SetStencilWriteMask(stencilWriteMask);
	}

	void GLRenderDevice::SetStencilWriteMask(uint32 mask)
	{
		// Set write mask if a change is needed.
		if (m_UsedStencilWriteMask == mask) return;
		glStencilMask(mask);
		m_UsedStencilWriteMask = mask;

	}

	void GLRenderDevice::SetScissorTest(bool enable, uint32 startX, uint32 startY, uint32 width, uint32 height)
	{
		// Disable if enabled.
		if (!enable)
		{
			if (!m_IsScissorsTestEnabled) return;
			else
			{
				glDisable(GL_SCISSOR_TEST);
				m_IsScissorsTestEnabled = false;
				return;
			}
		}

		// Enable if disabled, then bind it.
		if (!m_IsScissorsTestEnabled) glEnable(GL_SCISSOR_TEST);
		glScissor(startX, startY, width, height);
		m_IsScissorsTestEnabled = true;
	}

	std::string GLRenderDevice::GetShaderVersion()
	{
		// Return if not valid.
		if (!m_ShaderVersion.empty()) return m_ShaderVersion;

		// Check & set version according to data.
		uint32 version = GetVersion();

		if (version >= 330)
			m_ShaderVersion = LinaEngine::Internal::ToString(version);
		else if (version >= 320)
			m_ShaderVersion = "150";
		else if (version >= 310)
			m_ShaderVersion = "140";
		else if (version >= 300)
			m_ShaderVersion = "130";
		else if (version >= 210)
			m_ShaderVersion = "120";
		else if (version >= 200)
			m_ShaderVersion = "110";
		else
		{
			int32 majorVersion = version / 100;
			int32 minorVersion = (version / 10) % 10;
			LINA_CORE_ERR("Error: OpenGL Version {0}.{1} does not support shaders.\n", majorVersion, minorVersion);
			return "";
		}

		return m_ShaderVersion;
	}

	uint32 GLRenderDevice::GetVersion()
	{
		// Get version data from GL.
		if (m_GLVersion != 0) return m_GLVersion;
		int32 majorVersion;
		int32 minorVersion;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		m_GLVersion = (uint32)(majorVersion * 100 + minorVersion * 10);

		return m_GLVersion;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// GLOBALS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	static GLint GetOpenGLFormat(PixelFormat format)
	{
		// Get target Open GL Pixel Format
		switch (format)
		{

		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return GL_RG;
		case PixelFormat::FORMAT_RGB: return GL_RGB;
		case PixelFormat::FORMAT_RGBA: return GL_RGBA;
		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return 0; // GL_DEPTH_STENCIL;
		case PixelFormat::FORMAT_SRGB: return GL_RGB;
		case PixelFormat::FORMAT_SRGBA: return GL_RGBA;
		case PixelFormat::FORMAT_RGBA16F: return GL_RGBA;
		case PixelFormat::FORMAT_RGB16F: return GL_RGBA;
		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}

	static GLint GetOpenGLInternalFormat(PixelFormat format, bool compress)
	{
		// Get target OpenGL Internal Pixel Format
		switch (format)
		{

		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return  GL_RG;
		case PixelFormat::FORMAT_RGB:
			if (compress) return GL_COMPRESSED_RGB_S3TC_DXT1_EXT; // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			else return GL_RGB;
		case PixelFormat::FORMAT_RGBA:
			if (compress)  return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			else  return GL_RGBA;
		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
		case PixelFormat::FORMAT_SRGB: return GL_SRGB;
		case PixelFormat::FORMAT_SRGBA: return GL_SRGB_ALPHA;
		case PixelFormat::FORMAT_RGBA16F: return GL_RGBA16F;
		case PixelFormat::FORMAT_RGB16F: return GL_RGB16F;
		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}


	static bool AddShader(GLuint shaderProgram, const std::string& text, GLenum type, LinaArray<GLuint>* shaders)
	{
		// Create shader object.
		GLuint shader = glCreateShader(type);

		if (shader == 0)
		{
			LINA_CORE_ERR("Error creating shader type {0}", type);
			return false;
		}

		// Set appropriate length data.
		const GLchar* p[1];
		p[0] = text.c_str();
		GLint lengths[1];
		lengths[0] = (GLint)text.length();

		// Add the source & compile.
		glShaderSource(shader, 1, p, lengths);
		glCompileShader(shader);

		// Check error & report if exists.
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar InfoLog[1024];
			glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
			LINA_CORE_ERR("Error compiling shader type {0}: '{1}'\n", shader, InfoLog);
			return false;
		}

		// Attach the shader to program, store it & return.
		glAttachShader(shaderProgram, shader);
		shaders->push_back(shader);
		return true;
	}

	static bool CheckShaderError(GLuint shader, int flag, bool isProgram, const std::string& errorMessage)
	{
		// Check shader errors from OpenGl.
		GLint success = 0;
		GLchar error[1024] = { 0 };

		if (isProgram)
			glGetProgramiv(shader, flag, &success);
		else
			glGetShaderiv(shader, flag, &success);

		// Get error info if exists.
		if (!success)
		{
			if (isProgram)
				glGetProgramInfoLog(shader, sizeof(error), NULL, error);
			else
				glGetShaderInfoLog(shader, sizeof(error), NULL, error);

			LINA_CORE_ERR("{0}: '{1}'\n", errorMessage.c_str(), error);
			return true;
		}

		return false;
	}

	static void AddAllAttributes(GLuint program, const std::string& vertexShaderText, uint32 version)
	{
		// Terminate if attribute layout feature is enabled.
		if (version >= 320) return;


		// FIXME: This code assumes attributes are listed in order, which isn't
		// true for all compilers. It's safe to ignore for now because OpenGL versions
		// requiring this aren't being used.
		GLint numActiveAttribs = 0;
		GLint maxAttribNameLength = 0;

		// Load attributes.
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);

		// Iterate through the attributes.
		LinaArray<GLchar> nameData(maxAttribNameLength);
		for (GLint attrib = 0; attrib < numActiveAttribs; ++attrib)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;

			// Use appropriate attribute location.
			glGetActiveAttrib(program, attrib, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
			glBindAttribLocation(program, attrib, (char*)&nameData[0]);

		}
	}

	static void AddShaderUniforms(GLuint shaderProgram, const std::string& shaderText, std::map<std::string, GLint>& uniformBlockMap, std::map<std::string, GLint>& uniformMap, std::map<std::string, GLint>& samplerMap)
	{
		// Load uniform sets.
		GLint numBlocks;
		glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);


		// Iterate through sets.
		for (int32 block = 0; block < numBlocks; ++block)
		{
			// Get uniform set data to store it in our map.
			GLint nameLen;
			glGetActiveUniformBlockiv(shaderProgram, block, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
			LinaArray<GLchar> name(nameLen);
			glGetActiveUniformBlockName(shaderProgram, block, nameLen, NULL, &name[0]);
			std::string uniformBlockName((char*)&name[0], nameLen - 1);
			uniformBlockMap[uniformBlockName] = glGetUniformBlockIndex(shaderProgram, &name[0]);
		}

		// Load uniforms.
		GLint numUniforms = 0;
		glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

		// Iterate through uniforms.
		LinaArray<GLchar> uniformName(256);
		for (int32 uniform = 0; uniform < numUniforms; ++uniform)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;

			// Get sampler uniform data & store it on our sampler map.
			glGetActiveUniform(shaderProgram, uniform, uniformName.size(), &actualLength, &arraySize, &type, &uniformName[0]);

			/*if (type != GL_SAMPLER_2D)
			{
				LINA_CORE_ERR("Non-sampler2d uniforms currently unsupported!");
				continue;
			}*/


			std::string name((char*)&uniformName[0], actualLength - 1);
			samplerMap[name] = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
			GLint loc = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
			uniformMap[&uniformName[0]] = loc;

			for (int i = 1; i < arraySize; i++)
			{
				std::string name((char*)&uniformName[0], actualLength - 2);
				name = name + std::to_string(i) + "]";

				std::string newName(name.c_str(), actualLength - 1);
				samplerMap[newName] = glGetUniformLocation(shaderProgram, name.c_str());
				GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
				uniformMap[name] = loc;
			}
		
		
		}
	}
}

