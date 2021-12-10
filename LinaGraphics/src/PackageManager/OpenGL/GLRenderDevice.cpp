/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "PackageManager/OpenGL/GLRenderDevice.hpp"  
#include "Math/Color.hpp"
#include "Memory/Memory.hpp"
#include "glad/glad.h"

namespace Lina::Graphics
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

	float lineVertices[]
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f
	};

	struct btLine {
		GLfloat vertices[6];
		btLine(const Vector3& f, const Vector3& t) {
			vertices[0] = f.x;
			vertices[1] = f.y;
			vertices[2] = f.z;

			vertices[3] = t.x;
			vertices[4] = t.y;
			vertices[5] = t.z;
		}
	};

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

	float hdriCubemapVertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};


	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// GLOBALS DECLARATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	GLint GetOpenGLFormat(PixelFormat dataFormat);
	GLint GetOpenGLInternalFormat(PixelFormat internalFormat, bool compress);
	static bool AddShader(GLuint shaderProgram, const std::string& text, GLenum type, std::vector<GLuint>* shaders);
	static void AddAllAttributes(GLuint program, const std::string& vertexShaderText, uint32 version);
	static bool CheckShaderError(GLuint shader, int flag, bool isProgram, const std::string& errorMessage);
	static void AddShaderUniforms(GLuint shaderProgram, const std::string& shaderText, std::map<std::string, GLint>& uniformBlockMap, std::map<std::string, GLint>& uniformMap, std::map<std::string, GLint>& samplerMap);

	GLRenderDevice::GLRenderDevice()
	{
		LINA_TRACE("[Constructor] -> GLRenderDevice ({0})", typeid(*this).name());
		m_GLVersion = m_boundFBO = m_boundVAO = m_boundShader = m_viewportFBO = 0;
	}

	GLRenderDevice::~GLRenderDevice()
	{

	}


	void GLRenderDevice::Initialize(int width, int height, DrawParams& defaultParams)
	{
		const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
		const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
		LINA_TRACE("Graphics Information: {0}, {1}", vendor, renderer);

		m_isStencilTestEnabled = defaultParams.useStencilTest;
		m_isDepthTestEnabled = defaultParams.useDepthTest;
		m_isBlendingEnabled = (defaultParams.sourceBlend != BlendFunc::BLEND_FUNC_NONE || defaultParams.destBlend != BlendFunc::BLEND_FUNC_NONE);
		m_isScissorsTestEnabled = defaultParams.useScissorTest;
		m_usedDepthFunction = defaultParams.depthFunc;
		m_shouldWriteDepth = defaultParams.shouldWriteDepth;
		m_usedStencilFail = defaultParams.stencilFail;
		m_usedStencilPassButDepthFail = defaultParams.stencilPassButDepthFail;
		m_usedStencilPass = defaultParams.stencilPass;
		m_usedStencilFunction = defaultParams.stencilFunc;
		m_usedStencilTestMask = defaultParams.stencilTestMask;
		m_usedStencilWriteMask = defaultParams.stencilWriteMask;
		m_usedStencilComparisonValue = defaultParams.stencilComparisonVal;
		m_usedFaceCulling = defaultParams.faceCulling;
		m_usedSourceBlending = defaultParams.sourceBlend;
		m_usedDestinationBlending = defaultParams.destBlend;

		// Default GL settings.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_MULTISAMPLE);

		glDepthFunc(GL_LESS);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glFrontFace(GL_CW);


		if (m_isBlendingEnabled)
			glBlendFunc(m_usedSourceBlending, m_usedDestinationBlending);

	}



	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// TEXTURE OPERATIONS
	// ---------------------------------------------------------------------


	uint32 GLRenderDevice::CreateTexture2D(Vector2 size, const void* data, SamplerParameters samplerParams, bool compress, bool useBorder, Color borderColor)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.m_textureParams.m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.m_textureParams.m_internalPixelFormat, compress);
		GLenum textureTarget = GL_TEXTURE_2D;
		GLuint textureHandle;

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);

		glTexImage2D(textureTarget, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, data);

		// OpenGL texture params.
		SetupTextureParameters(textureTarget, samplerParams, useBorder, &borderColor.r);

		// Enable mipmaps if needed.
		if (samplerParams.m_textureParams.m_generateMipMaps)
			glGenerateMipmap(textureTarget);
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(textureTarget, 0);

		return textureHandle;
	}

	uint32 GLRenderDevice::CreateTextureHDRI(Vector2 size, float* data, SamplerParameters samplerParams)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.m_textureParams.m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.m_textureParams.m_internalPixelFormat, false);
		GLenum textureTarget = GL_TEXTURE_2D;
		GLuint textureHandle;

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);


		glTexImage2D(textureTarget, 0, internalFormat, size.x, size.y, 0, format, GL_FLOAT, data);

		// OpenGL texture params.
		SetupTextureParameters(textureTarget, samplerParams);

		// Enable mipmaps if needed.
		if (samplerParams.m_textureParams.m_generateMipMaps)
			glGenerateMipmap(textureTarget);
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(textureTarget, 0);

		return textureHandle;
	}

	uint32 GLRenderDevice::CreateCubemapTexture(Vector2 size, SamplerParameters samplerParams, const std::vector<unsigned char*>& data, uint32 dataSize)
	{
		GLuint textureHandle;
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.m_textureParams.m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.m_textureParams.m_internalPixelFormat, false);

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

		if (samplerParams.m_textureParams.m_generateMipMaps)
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
		GLint format = GetOpenGLFormat(samplerParams.m_textureParams.m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.m_textureParams.m_internalPixelFormat, false);

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

		// Loop through each face to gen. image.
		for (GLuint i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size.x, size.y, 0, format, GL_FLOAT, NULL);
		}

		// Specify wrapping & filtering
		//SetupTextureParameters(GL_TEXTURE_CUBE_MAP, samplerParams);

		if (samplerParams.m_textureParams.m_generateMipMaps)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		else
		{
			//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
			//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return textureHandle;
	}

	uint32 GLRenderDevice::CreateTexture2DMSAA(Vector2 size, SamplerParameters samplerParams, int sampleCount)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.m_textureParams.m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.m_textureParams.m_internalPixelFormat, false);
		GLenum textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
		GLuint textureHandle;

		// Generate texture & bind to program.
		glGenTextures(1, &textureHandle);
		glBindTexture(textureTarget, textureHandle);

		// Build texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, internalFormat, size.x, size.y, GL_TRUE);

		// Setup texture params
		//SetupTextureParameters(textureTarget, samplerParams);

		// Enable mipmaps if needed.
		//if (samplerParams.m_textureParams.m_generateMipMaps)
		//	glGenerateMipmap(textureTarget);
		//else
		//{
		//	glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
		//	glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		//}

		glBindTexture(textureTarget, 0);
		return textureHandle;
	}

	uint32 GLRenderDevice::CreateTexture2DEmpty(Vector2 size, SamplerParameters samplerParams)
	{
		// Declare formats, target & handle for the texture.
		GLint format = GetOpenGLFormat(samplerParams.m_textureParams.m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(samplerParams.m_textureParams.m_internalPixelFormat, false);
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
		if (samplerParams.m_textureParams.m_generateMipMaps)
			glGenerateMipmap(textureTarget);
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(textureTarget, 0);
		return textureHandle;
	}

	void GLRenderDevice::SetupTextureParameters(uint32 textureTarget, SamplerParameters samplerParams, bool useBorder, float* borderColor)
	{
		// OpenGL texture params.
		glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, samplerParams.m_textureParams.m_minFilter);
		glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, samplerParams.m_textureParams.m_magFilter);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, samplerParams.m_textureParams.m_wrapS);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, samplerParams.m_textureParams.m_wrapT);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, samplerParams.m_textureParams.m_wrapR);

		if (useBorder)
		{
			float borderColor[] = { borderColor[0], borderColor[1], borderColor[2], borderColor[3] };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
	}

	void GLRenderDevice::UpdateTextureParameters(uint32 bindMode, uint32 id, SamplerParameters samplerParams)
	{
		glBindTexture(bindMode, id);
		glTexParameterf(bindMode, GL_TEXTURE_MIN_FILTER, samplerParams.m_textureParams.m_minFilter);
		glTexParameterf(bindMode, GL_TEXTURE_MAG_FILTER, samplerParams.m_textureParams.m_magFilter);
		glTexParameteri(bindMode, GL_TEXTURE_WRAP_S, samplerParams.m_textureParams.m_wrapS);
		glTexParameteri(bindMode, GL_TEXTURE_WRAP_T, samplerParams.m_textureParams.m_wrapT);
		glTexParameteri(bindMode, GL_TEXTURE_WRAP_R, samplerParams.m_textureParams.m_wrapR);

		// Enable mipmaps if needed.
		if (samplerParams.m_textureParams.m_generateMipMaps)
			glGenerateMipmap(bindMode);
		else
		{
			glTexParameteri(bindMode, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(bindMode, GL_TEXTURE_MAX_LEVEL, 0);
		}

		glBindTexture(bindMode, 0);
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

	uint32 GLRenderDevice::CreateVertexArray(const std::vector<BufferData>& data, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage)
	{
		// Define vertex array object, buffers, buffer count & their sizes.
		unsigned int numBuffers = numVertexComponents + numInstanceComponents + 1;
		GLuint VAO;
		GLuint* buffers = new GLuint[numBuffers];
		uintptr* bufferSizes = new uintptr[numBuffers];

		// We generate a buffer for each allocated element in the indexed model.
		glGenVertexArrays(1, &VAO);
		SetVAO(VAO);
		glGenBuffers(numBuffers, buffers);

		// We create the attributes for each buffer.
		for (uint32 i = 0; i < numBuffers - 1; i++)
		{
			bool isFloatBuffer = data[i].m_isFloat;

			// Check vertex component count and switch to dynamic draw if current attribute exceeds.
			// This is for the Allocated Elements after StartElementIndex in indexed model, e.g. attributes that'll be dynamically changed.
			BufferUsage attribUsage = bufferUsage;
			bool inInstancedMode = false;
			if (i >= numVertexComponents)
			{
				attribUsage = BufferUsage::USAGE_DYNAMIC_DRAW;
				inInstancedMode = true;
			}

			// Get each Allocated Element as buffer (each one is a std::vector<float>, size of elementSize
			uint32 elementCount = data[i].m_elementSize;
			auto typeSize = isFloatBuffer ? sizeof(float) : sizeof(int);
			uintptr dataSize = inInstancedMode ? elementCount * typeSize : elementCount * typeSize * numVertices;

			// Bind the current array buffer & set the data.
			glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);

			if(inInstancedMode)
			glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, attribUsage);
			else
			{
				if (isFloatBuffer)
				{
					const void* bufferData = &data[i].m_floatElements[0];
					glBufferData(GL_ARRAY_BUFFER, dataSize, bufferData, attribUsage);
				}
				else
				{
					const void* bufferData = &data[i].m_intElements[0];
					glBufferData(GL_ARRAY_BUFFER, dataSize, bufferData, attribUsage);
				}
			}
			bufferSizes[i] = dataSize;

			// Define element sizes to pass the required part of the array to the attrib pointer call.
			uint32 elementSizeDiv = elementCount / 4;
			uint32 elementSizeRem = elementCount % 4;

			// This is for elements that has size of 4 or more. 
			// E.g for matrices, we are going to be buffering them 4 by 4.
			for (uint32 j = 0; j < elementSizeDiv; j++)
			{
				uint32 actualAttrib = data[i].m_attrib + j;
				glEnableVertexAttribArray(actualAttrib);

				if (isFloatBuffer)
					glVertexAttribPointer(actualAttrib, 4, GL_FLOAT, GL_FALSE, elementCount * sizeof(GLfloat), (const GLvoid*)(sizeof(GLfloat) * j * 4));
				else
					glVertexAttribIPointer(actualAttrib, 4, GL_INT, elementCount * sizeof(GLint), (const GLvoid*)(sizeof(GLint) * j * 4));

				if (inInstancedMode)
					glVertexAttribDivisor(actualAttrib, 1);

			}


			// This is for the elements that have sizes < 4
			if (elementSizeRem != 0)
			{
				uint32 actualAttrib = data[i].m_attrib;
				glEnableVertexAttribArray(actualAttrib);

				if (isFloatBuffer)
					glVertexAttribPointer(actualAttrib, elementCount, GL_FLOAT, GL_FALSE, elementCount * sizeof(GLfloat), (const GLvoid*)(sizeof(GLfloat) * elementSizeDiv * 4));
				else
					glVertexAttribIPointer(actualAttrib, elementCount, GL_INT, elementCount * sizeof(GLint), (const GLvoid*)(sizeof(GLint) * elementSizeDiv * 4));

				if (inInstancedMode)
					glVertexAttribDivisor(actualAttrib, 1);

			}
		}

		// Finally bind the element array buffer.
		uintptr indicesSize = numIndices * sizeof(uint32);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, bufferUsage);
		bufferSizes[numBuffers - 1] = indicesSize;

		// Build vertex array based on our calculated data.
		struct VertexArrayData vaoData;
		vaoData.buffers = buffers;
		vaoData.bufferSizes = bufferSizes;
		vaoData.numBuffers = numBuffers;
		vaoData.numElements = numIndices;
		vaoData.bufferUsage = bufferUsage;
		vaoData.instanceComponentsStartIndex = numVertexComponents;

		// Store the array in our map & return the modified vertex array object.
		m_vaoMap[VAO] = vaoData;
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
		std::map<uint32, VertexArrayData>::iterator it = m_vaoMap.find(vao);
		if (it == m_vaoMap.end()) return 0;

		// Get the vertex array object data from the map.
		const struct VertexArrayData* vaoData = &it->second;

		// Delete the VA & buffers, then data.
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(vaoData->numBuffers, vaoData->buffers);
		delete[] vaoData->buffers;
		delete[] vaoData->bufferSizes;

		// Remove from the map.
		m_vaoMap.erase(it);
		return 0;
	}

	uint32 GLRenderDevice::CreateSkyboxVertexArray()
	{
		unsigned int skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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

	uint32 GLRenderDevice::CreateLineVertexArray()
	{
		unsigned int lineVAO, lineVBO;
		glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);
		SetVAO(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), &lineVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		return lineVAO;
	}


	uint32 GLRenderDevice::CreateHDRICubeVertexArray()
	{
		uint32 cubeVAO, cubeVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(hdriCubemapVertices), hdriCubemapVertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return cubeVAO;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// TEXTURE SAMPLER OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateSampler(SamplerParameters samplerParams, bool isCubemap)
	{
		// OpenGL Texture Sampler parameters.
		uint32 result = 0;
		glGenSamplers(1, &result);

		glSamplerParameteri(result, GL_TEXTURE_WRAP_S, samplerParams.m_textureParams.m_wrapS);
		glSamplerParameteri(result, GL_TEXTURE_WRAP_T, samplerParams.m_textureParams.m_wrapT);
		glSamplerParameteri(result, GL_TEXTURE_MAG_FILTER, samplerParams.m_textureParams.m_magFilter);
		glSamplerParameteri(result, GL_TEXTURE_MIN_FILTER, samplerParams.m_textureParams.m_minFilter);

		if (isCubemap)
			glSamplerParameteri(result, GL_TEXTURE_WRAP_R, samplerParams.m_textureParams.m_wrapR);
		// Set m_anisotropy if applicable.
		if (samplerParams.m_anisotropy != 0.0f && samplerParams.m_textureParams.m_minFilter != FILTER_NEAREST && samplerParams.m_textureParams.m_minFilter != FILTER_LINEAR)
			glSamplerParameterf(result, GL_TEXTURE_MAX_ANISOTROPY, samplerParams.m_anisotropy);

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

	uint32 GLRenderDevice::CreateShaderProgram(const std::string& shaderText, ShaderUniformData* data, bool usesGeometryShader)
	{
		// Shader program instance.
		GLuint shaderProgram = glCreateProgram();

		if (shaderProgram == 0)
		{
			LINA_ERR("Error creating shader program!");
			return (uint32)-1;
		}


		// Modify the shader text to include the version data.
		std::string version = GetShaderVersion();
		std::string vertexShaderText = "#version " + version + " core" + "\n#define VS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;
		std::string fragmentShaderText = "#version " + version + " core" + "\n#define FS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;

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

		// Bind attributes for GL & add shader uniforms.
		AddAllAttributes(shaderProgram, vertexShaderText, GetVersion());
		AddShaderUniforms(shaderProgram, shaderText, programData.uniformBlockMap, programData.uniformMap, programData.samplerMap);
		// Store the program in our map & return it.
		m_shaderProgramMap[shaderProgram] = programData;
		*data = ScanShaderUniforms(shaderProgram);

		return shaderProgram;
	}

	bool GLRenderDevice::ValidateShaderProgram(uint32 shader)
	{
		// Validate program & check validation errors.
		glValidateProgram(shader);
		return CheckShaderError(shader, GL_VALIDATE_STATUS, true, "Invalid shader program");
	}

	uint32 GLRenderDevice::ReleaseShaderProgram(uint32 shader)
	{
		// Terminate if shader is not valid or does not exist in our map.
		if (shader == 0) return 0;
		std::map<uint32, ShaderProgram>::iterator programIt = m_shaderProgramMap.find(shader);
		if (programIt == m_shaderProgramMap.end()) return 0;

		// Get the program from the map.
		const ShaderProgram* shaderProgram = &programIt->second;

		// Detach & delete each shader assigned to our program.
		for (std::vector<uint32>::const_iterator it = shaderProgram->shaders.begin(); it != shaderProgram->shaders.end(); ++it)
		{
			glDetachShader(shader, *it);
			glDeleteShader(*it);
		}

		// Delete the program, erase from our map & return.
		glDeleteProgram(shader);
		m_shaderProgramMap.erase(programIt);
		return 0;
	}

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// FRAME BUFFER OBJECT OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	uint32 GLRenderDevice::CreateRenderTarget(uint32 texture, int32 width, int32 height, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel, bool noReadWrite, bool bindRBO, FrameBufferAttachment rboAttachment, uint32 rbo, bool errorCheck)
	{
		// Generate frame buffers & set the current object.
		uint32 fbo;
		glGenFramebuffers(1, &fbo);
		SetFBO(fbo);

		// Define attachment type & use the buffer.
		GLenum attachmentTypeGL = attachment + attachmentNumber;

		if (bindTextureMode != TextureBindMode::BINDTEXTURE_NONE)
		{
			if (bindTextureMode != TextureBindMode::BINDTEXTURE_TEXTURE)
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTypeGL, bindTextureMode, texture, mipLevel);
			else
				glFramebufferTexture(GL_FRAMEBUFFER, attachmentTypeGL, texture, mipLevel);
		}

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
		if (errorCheck && glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LINA_ERR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		SetFBO(0);
		return fbo;
	}

	void GLRenderDevice::BindTextureToRenderTarget(uint32 fbo, uint32 texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 textureAttachmentNumber, int mipLevel, bool bindTexture, bool setDefaultFBO)
	{
		SetFBO(fbo);
		GLenum attachmentTypeGL = attachment + attachmentNumber;
		GLenum textureAttachment = bindTextureMode + textureAttachmentNumber;

		if (bindTexture)
			glBindTexture(GL_TEXTURE_2D, texture);

		if (bindTextureMode != TextureBindMode::BINDTEXTURE_NONE)
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTypeGL, textureAttachment, texture, mipLevel);
		else
			glFramebufferTexture(GL_FRAMEBUFFER, attachmentTypeGL, texture, mipLevel);

		if (setDefaultFBO)
			SetFBO(0);
	}

	void GLRenderDevice::MultipleDrawBuffersCommand(uint32 fbo, uint32 bufferCount, uint32* attachments)
	{
		SetFBO(fbo);
		glDrawBuffers(bufferCount, attachments);
		SetFBO(0);
	}

	void GLRenderDevice::ResizeRTTexture(uint32 texture, Vector2 newSize, PixelFormat m_internalPixelFormat, PixelFormat m_pixelFormat, TextureBindMode bindMode, bool compress)
	{
		glBindTexture(bindMode, texture);
		GLint format = GetOpenGLFormat(m_pixelFormat);
		GLint internalFormat = GetOpenGLInternalFormat(m_internalPixelFormat, compress);
		glTexImage2D(bindMode, 0, internalFormat, (uint32)newSize.x, (uint32)newSize.y, 0, format, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(bindMode, 0);
	}

	void GLRenderDevice::ResizeRenderBuffer(uint32 fbo, uint32 rbo, Vector2 newSize, RenderBufferStorage storage)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, storage, (uint32)newSize.x, (uint32)newSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	uint32 GLRenderDevice::ReleaseRenderTarget(uint32 fbo)
	{
		// Terminate if fbo is not valid or does not exist in our map.
		if (fbo == 0) return 0;

		// Delete the frame buffer object, erase from our map & return.
		glDeleteFramebuffers(1, &fbo);
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

	void GLRenderDevice::UpdateSamplerParameters(uint32 sampler, SamplerParameters samplerParams)
	{
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, samplerParams.m_textureParams.m_wrapS);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, samplerParams.m_textureParams.m_wrapT);
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, samplerParams.m_textureParams.m_magFilter);
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, samplerParams.m_textureParams.m_minFilter);

		// Set m_anisotropy if applicable.
		if (samplerParams.m_anisotropy != 0.0f && samplerParams.m_textureParams.m_minFilter != FILTER_NEAREST && samplerParams.m_textureParams.m_minFilter != FILTER_LINEAR)
			glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, samplerParams.m_anisotropy);

	}

	void GLRenderDevice::GenerateTextureMipmaps(uint32 texture, TextureBindMode bindMode)
	{
		glBindTexture(bindMode, texture);
		glGenerateMipmap(bindMode);
	}

	void GLRenderDevice::BlitFrameBuffers(uint32 readFBO, uint32 readWidth, uint32 readHeight, uint32 writeFBO, uint32 writeWidth, uint32 writeHeight, BufferBit mask, SamplerFilter filter, FrameBufferAttachment att, uint32 attCount)
	{
		if (m_boundReadFBO != readFBO)
		{
			m_boundReadFBO = readFBO;

		}
		glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
		glReadBuffer(att + attCount);
		if (m_boundWriteFBO != writeFBO)
		{
			m_boundWriteFBO = writeFBO;

		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO);
		glDrawBuffer(att + attCount);
		glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, writeWidth, writeHeight, mask, filter);
	}

	bool GLRenderDevice::IsRenderTargetComplete(uint32 fbo)
	{
		SetFBO(fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LINA_ERR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
			SetFBO(0);
			return false;
		}
		SetFBO(0);
		return true;
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
		std::map<uint32, VertexArrayData>::iterator it = m_vaoMap.find(vao);
		if (it == m_vaoMap.end()) return;

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
		if (shader == m_boundShader) return;
		glUseProgram(shader);
		m_boundShader = shader;
	}

	void GLRenderDevice::SetTexture(uint32 texture, uint32 sampler, uint32 unit, TextureBindMode bindTextureMode, bool setSampler)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(bindTextureMode, texture);

		if (setSampler)
			glBindSampler(unit, sampler);
	}

	void GLRenderDevice::SetShaderUniformBuffer(uint32 shader, const std::string& uniformBufferName, uint32 buffer)
	{
		// Use shader first.
		SetShader(shader);

		// Update the uniform data.
		glBindBufferBase(GL_UNIFORM_BUFFER, m_shaderProgramMap[shader].uniformBlockMap[uniformBufferName], buffer);
	}

	void GLRenderDevice::BindUniformBuffer(uint32 bufferObject, uint32 point)
	{
		// Bind the buffer object to the point.
		glBindBufferBase(GL_UNIFORM_BUFFER, point, bufferObject);
	}

	void GLRenderDevice::BindShaderBlockToBufferPoint(uint32 shader, uint32 blockPoint, std::string& blockName)
	{
		glUniformBlockBinding(shader, m_shaderProgramMap[shader].uniformBlockMap[blockName], blockPoint);
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
		std::map<uint32, VertexArrayData>::iterator it = m_vaoMap.find(vao);
		if (it == m_vaoMap.end()) return;

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
		if (m_boundUBO != buffer)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);
			m_boundUBO = buffer;
		}

		glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
	}

	void GLRenderDevice::UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize)
	{
		void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		Memory::memcpy(dest, data, dataSize);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	ShaderUniformData GLRenderDevice::ScanShaderUniforms(uint32 shader)
	{


		GLint count;
		glGetProgramiv(shader, GL_ACTIVE_UNIFORMS, &count);
		ShaderUniformData data;
		uint32 samplerUnit = 0;

		std::vector<GLchar> uniformName(256);
		for (int32 uniform = 0; uniform < count; ++uniform)
		{

			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;
			// Get sampler uniform data & store it on our sampler map.
			glGetActiveUniform(shader, uniform, uniformName.size(), &actualLength, &arraySize, &type, &uniformName[0]);

			std::string nameStr = &uniformName[0];
			//	for (int j = 0; j < uniformName.size(); j++)
				//	nameStr += uniformName[j];


			if (nameStr.find("material.") != std::string::npos || nameStr.find("uf_") != std::string::npos)
			{
				if (nameStr.find(".texture") != std::string::npos)
				{
					size_t lastindex = nameStr.find_last_of(".");
					std::string samplerName = nameStr.substr(0, lastindex);

					if (type == GL_SAMPLER_2D)
					{
						data.m_sampler2Ds[samplerName] = { samplerUnit,  TextureBindMode::BINDTEXTURE_TEXTURE2D };
						samplerUnit++;
					}
					else if (type == GL_SAMPLER_CUBE)
					{
						data.m_sampler2Ds[samplerName] = { samplerUnit,  TextureBindMode::BINDTEXTURE_CUBEMAP };
						samplerUnit++;
					}
				}
				else if (nameStr.find(".isActive") != std::string::npos)
					continue;

				if (type == GL_FLOAT)
					data.m_floats[&uniformName[0]] = 0.0f;
				else if (type == GL_INT)
					data.m_ints[&uniformName[0]] = 0;
				else if (type == GL_FLOAT_VEC2)
					data.m_vector2s[&uniformName[0]] = Vector2::One;
				else if (type == GL_FLOAT_VEC3)
				{
					if (nameStr.find("color") != std::string::npos || nameStr.find("Color") != std::string::npos)
						data.m_colors[&uniformName[0]] = Color::White;
					else
						data.m_vector3s[&uniformName[0]] = Vector3::One;
				}
				else if (type == GL_FLOAT_VEC4)
					data.m_vector4s[&uniformName[0]] = Vector4::One;
				else if (type == GL_BOOL)
					data.m_bools[&uniformName[0]] = false;
				else if (type == GL_FLOAT_MAT4)
					data.m_matrices[&uniformName[0]] = Matrix::Identity();
			}
		}

		return data;
	}


	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// DRAWING OPERATIONS
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	void GLRenderDevice::SetDrawParameters(const DrawParams& drawParams)
	{
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

	}


	void GLRenderDevice::Draw(uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements, bool drawArrays)
	{
		// No need to draw nothin dude.
		if (!drawArrays && numInstances == 0) return;

		// Set parameters.
		if (!drawParams.skipParameters)
			SetDrawParameters(drawParams);

		// Set vao & draw
		SetVAO(vao);

		if (drawArrays)
		{
			glDrawArrays(drawParams.primitiveType, 0, numElements);
		}
		else
		{
			// 1 object or instanced draw calls?
			if (numInstances == 1)
				glDrawElements(drawParams.primitiveType, (GLsizei)numElements, GL_UNSIGNED_INT, 0);
			else
				glDrawElementsInstanced(drawParams.primitiveType, (GLsizei)numElements, GL_UNSIGNED_INT, 0, numInstances);
		}


	}

	void GLRenderDevice::DrawLine(float width)
	{
		// This function requires you to set model matrix in the debuglines shader.
		glLineWidth(width);
		glDrawArrays(GL_LINES, 0, 2);
	}

	void GLRenderDevice::DrawLine(uint32 shader, const Matrix& model, const Vector3& from, const Vector3& to, float width)
	{
		// Set line width.
		glLineWidth(width);

		// Lines
		btLine lines(from, to);
		Vector3 result = to - from;
		GLuint vao, vbo;

		// Buffers
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);

		//UPLOADING VERTEX
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * 6, lines.vertices, GL_STATIC_DRAW);

		// Enable position.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(GLfloat) * 3, (GLvoid*)0);
		glBindVertexArray(0);

		// Draw
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

		//delete buffers
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void GLRenderDevice::Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const Color& color, uint32 stencil)
	{
		// Make sure frame buffer objects are used.
		uint32 flags = 0;

		// Set flags according to options.
		if (shouldClearColor)
		{
			flags |= GL_COLOR_BUFFER_BIT;

			if (color != m_currentClearColor)
			{
				glClearColor((GLfloat)color.r, (GLfloat)color.g, (GLfloat)color.b, (GLfloat)color.a);
				m_currentClearColor = color;
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



	void GLRenderDevice::UpdateShaderUniformFloat(uint32 shader, const std::string& uniform, const float f)
	{
		glUniform1f(m_shaderProgramMap[shader].uniformMap[uniform], (GLfloat)f);
	}

	void GLRenderDevice::UpdateShaderUniformInt(uint32 shader, const std::string& uniform, const int f)
	{
		glUniform1i(m_shaderProgramMap[shader].uniformMap[uniform], (GLint)f);
	}

	void GLRenderDevice::UpdateShaderUniformColor(uint32 shader, const std::string& uniform, const Color& color)
	{
		glUniform3f(m_shaderProgramMap[shader].uniformMap[uniform], (GLfloat)color.r, (GLfloat)color.g, (GLfloat)color.b);
	}

	void GLRenderDevice::UpdateShaderUniformVector2(uint32 shader, const std::string& uniform, const Vector2& m)
	{
		glUniform2f(m_shaderProgramMap[shader].uniformMap[uniform], (GLfloat)m.x, (GLfloat)m.y);
	}

	void GLRenderDevice::UpdateShaderUniformVector3(uint32 shader, const std::string& uniform, const Vector3& m)
	{
		glUniform3f(m_shaderProgramMap[shader].uniformMap[uniform], (GLfloat)m.x, (GLfloat)m.y, (GLfloat)m.z);
	}

	void GLRenderDevice::UpdateShaderUniformVector4F(uint32 shader, const std::string& uniform, const Vector4& m)
	{
		glUniform4f(m_shaderProgramMap[shader].uniformMap[uniform], (GLfloat)m.x, (GLfloat)m.y, (GLfloat)m.z, (GLfloat)m.w);

	}

	void GLRenderDevice::UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, void* data)
	{
		float* matrixData = ((float*)data);
		glUniformMatrix4fv(m_shaderProgramMap[shader].uniformMap[uniform], 1, GL_FALSE, matrixData);
	}

	void GLRenderDevice::UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, const Matrix& m)
	{
		glUniformMatrix4fv(m_shaderProgramMap[shader].uniformMap[uniform], 1, GL_FALSE, &m[0][0]);
	}


	void GLRenderDevice::SetVAO(uint32 vao)
	{
		// Use VAO if exists.
		if (vao == m_boundVAO) 	return;
		glBindVertexArray(vao);
		m_boundVAO = vao;
	}

	void GLRenderDevice::CaptureHDRILightingData(Matrix& view, Matrix& projection, Vector2 captureSize, uint32 cubeMapTexture, uint32 hdrTexture, uint32 fbo, uint32 rbo, uint32 shader)
	{
		uint32 captureFBO;
		glGenFramebuffers(1, &captureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		SetShader(shader);
		UpdateShaderUniformInt(shader, "equirectangularMap", 0);
		UpdateShaderUniformMatrix(shader, "projection", projection);
		SetTexture(hdrTexture, 0, 0);

	}

	void GLRenderDevice::SetFBO(uint32 fbo)
	{
		if (fbo == m_boundFBO) return;
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		m_boundFBO = m_boundReadFBO = m_boundWriteFBO = fbo;
	}


	void GLRenderDevice::SetRBO(uint32 rbo)
	{
		if (rbo == m_boundRBO) return;
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		m_boundRBO = rbo;
	}

	void GLRenderDevice::SetViewport(Vector2 pos, Vector2 size)
	{
		// Update viewport according to the render targets if exist.
		// if (fbo == m_ViewportFBO) return;
		// m_ViewportFBO = fbo;

		if (pos == m_boundViewportPos && size == m_boundViewportSize) return;
		glViewport((uint32)pos.x, (uint32)pos.y, (uint32)size.x, (uint32)size.y);
		m_boundViewportSize = size;
		m_boundViewportPos = pos;

	}

	void GLRenderDevice::SetFaceCulling(FaceCulling faceCulling)
	{
		// If target is enabled, then disable face culling.
		// If current is disabled, then enable faceculling.
		// Else switch cull state.
		if (faceCulling == FACE_CULL_NONE)
		{
			glDisable(GL_CULL_FACE);
			m_usedFaceCulling = FACE_CULL_NONE;
			return;
		}
		else if (m_usedFaceCulling == FACE_CULL_NONE)// Face culling is disabled but needs to be enabled
			glEnable(GL_CULL_FACE);

		if (m_usedFaceCulling != faceCulling)
		{
			m_usedFaceCulling = faceCulling;
			glCullFace(faceCulling);
		}
	}

	void GLRenderDevice::SetDepthTest(bool shouldWrite, DrawFunc depthFunc)
	{

		// Toggle dept writing.
		if (shouldWrite != m_shouldWriteDepth)
		{
			glDepthMask(shouldWrite ? GL_TRUE : GL_FALSE);
			m_shouldWriteDepth = shouldWrite;
		}

		// Update if change is needed.
		if (depthFunc == m_usedDepthFunction)	return;

		glDepthFunc(depthFunc);
		m_usedDepthFunction = depthFunc;
	}

	void GLRenderDevice::SetDepthTestEnable(bool enable)
	{
		if (m_isDepthTestEnabled != enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			m_isDepthTestEnabled = enable;
		}
	}
	void GLRenderDevice::SetBlending(BlendFunc sourceBlend, BlendFunc destBlend)
	{
		// If no change is needed return.
		if (sourceBlend == m_usedSourceBlending && destBlend == m_usedDestinationBlending) return;
		else if (sourceBlend == BLEND_FUNC_NONE || destBlend == BLEND_FUNC_NONE)
			glDisable(GL_BLEND);
		else if (m_usedSourceBlending == BLEND_FUNC_NONE || m_usedDestinationBlending == BLEND_FUNC_NONE)
		{
			glEnable(GL_BLEND);
			glBlendFunc(sourceBlend, destBlend);
		}
		else
			glBlendFunc(sourceBlend, destBlend);


		m_usedSourceBlending = sourceBlend;
		m_usedDestinationBlending = destBlend;
	}

	void GLRenderDevice::SetStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass)
	{
		// If change is needed toggle enabled state & enable/disable stencil test.
		if (enable != m_isStencilTestEnabled)
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			m_isStencilTestEnabled = enable;
		}

		// Set stencil params.
		if (stencilFunc != m_usedStencilFunction || stencilTestMask != m_usedStencilTestMask || stencilComparisonVal != m_usedStencilComparisonValue)
		{
			glStencilFunc(stencilFunc, stencilComparisonVal, stencilTestMask);
			m_usedStencilComparisonValue = stencilComparisonVal;
			m_usedStencilTestMask = stencilTestMask;
			m_usedStencilFunction = stencilFunc;
		}

		if (stencilFail != m_usedStencilFail || stencilPass != m_usedStencilPass || stencilPassButDepthFail != m_usedStencilPassButDepthFail)
		{
			glStencilOp(stencilFail, stencilPassButDepthFail, stencilPass);
			m_usedStencilFail = stencilFail;
			m_usedStencilPass = stencilPass;
			m_usedStencilPassButDepthFail = stencilPassButDepthFail;
		}

		SetStencilWriteMask(stencilWriteMask);
	}

	void GLRenderDevice::SetStencilWriteMask(uint32 mask)
	{
		// Set write mask if a change is needed.
		if (m_usedStencilWriteMask == mask) return;
		glStencilMask(mask);
		m_usedStencilWriteMask = mask;

	}

	void GLRenderDevice::SetScissorTest(bool enable, uint32 startX, uint32 startY, uint32 width, uint32 height)
	{
		// Disable if enabled.
		if (!enable)
		{
			if (!m_isScissorsTestEnabled) return;
			else
			{
				glDisable(GL_SCISSOR_TEST);
				m_isScissorsTestEnabled = false;
				return;
			}
		}

		// Enable if disabled, then bind it.
		if (!m_isScissorsTestEnabled) glEnable(GL_SCISSOR_TEST);
		glScissor(startX, startY, width, height);
		m_isScissorsTestEnabled = true;
	}

	std::string GLRenderDevice::GetShaderVersion()
	{
		// Return if not valid.
		if (!m_shaderVersion.empty()) return m_shaderVersion;

		// Check & set version according to data.
		uint32 version = GetVersion();

		if (version >= 330)
			m_shaderVersion = std::to_string(version);
		else if (version >= 320)
			m_shaderVersion = "150";
		else if (version >= 310)
			m_shaderVersion = "140";
		else if (version >= 300)
			m_shaderVersion = "130";
		else if (version >= 210)
			m_shaderVersion = "120";
		else if (version >= 200)
			m_shaderVersion = "110";
		else
		{
			int32 majorVersion = version / 100;
			int32 minorVersion = (version / 10) % 10;
			LINA_ERR("Error: OpenGL Version {0}.{1} does not support shaders.\n", majorVersion, minorVersion);
			return "";
		}

		return m_shaderVersion;
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
		case PixelFormat::FORMAT_DEPTH16: return GL_DEPTH_COMPONENT16;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return 0; // GL_DEPTH_STENCIL;
		case PixelFormat::FORMAT_SRGB: return GL_RGB;
		case PixelFormat::FORMAT_SRGBA: return GL_RGBA;
		case PixelFormat::FORMAT_RGBA16F: return GL_RGBA;
		case PixelFormat::FORMAT_RGB16F: return GL_RGBA;
		default:
			LINA_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
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
		case PixelFormat::FORMAT_DEPTH16: return GL_DEPTH_COMPONENT16;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
		case PixelFormat::FORMAT_SRGB: return GL_SRGB;
		case PixelFormat::FORMAT_SRGBA: return GL_SRGB_ALPHA;
		case PixelFormat::FORMAT_RGBA16F: return GL_RGBA16F;
		case PixelFormat::FORMAT_RGB16F: return GL_RGB16F;
		default:
			LINA_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}


	static bool AddShader(GLuint shaderProgram, const std::string& text, GLenum type, std::vector<GLuint>* shaders)
	{
		// Build shader object.
		GLuint shader = glCreateShader(type);

		if (shader == 0)
		{
			LINA_ERR("Error creating shader type {0}", type);
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
			LINA_ERR("Error compiling shader type {0}: '{1}'\n", shader, InfoLog);
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

			LINA_ERR("{0}: '{1}'\n", errorMessage.c_str(), error);
			return true;
		}

		return false;
	}

	static void AddAllAttributes(GLuint program, const std::string& vertexShaderText, uint32 version)
	{
		// Terminate if attribute layout feature is enabled.
		if (version >= 320) return;


		// FIXME: This code assumes attributes are listed in order, which isn't
		// true for all compilers. It'nameStr safe to ignore for now because OpenGL versions
		// requiring this aren't being used.
		GLint numActiveAttribs = 0;
		GLint maxAttribNameLength = 0;

		// Load attributes.
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);

		// Iterate through the attributes.
		std::vector<GLchar> nameData(maxAttribNameLength);
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
			std::vector<GLchar> name(nameLen);
			glGetActiveUniformBlockName(shaderProgram, block, nameLen, NULL, &name[0]);
			std::string uniformBlockName((char*)&name[0], nameLen - 1);
			uniformBlockMap[uniformBlockName] = glGetUniformBlockIndex(shaderProgram, &name[0]);
		}

		// Load uniforms.
		GLint numUniforms = 0;
		glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

		// Iterate through uniforms.
		std::vector<GLchar> uniformName(256);
		for (int32 uniform = 0; uniform < numUniforms; ++uniform)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;

			// Get sampler uniform data & store it on our sampler map.
			glGetActiveUniform(shaderProgram, uniform, uniformName.size(), &actualLength, &arraySize, &type, &uniformName[0]);

			/*if (type != GL_SAMPLER_2D)
			{
				LINA_ERR("Non-sampler2d uniforms currently unsupported!");
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

