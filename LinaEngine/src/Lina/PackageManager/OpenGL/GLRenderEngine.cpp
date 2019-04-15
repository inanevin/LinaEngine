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
#include "GLRenderEngine.hpp"  
#include "glad/glad.h"

namespace LinaEngine
{
	GLint GetOpenGLFormat(PixelFormat dataFormat);
	GLint GetOpenGLInternalFormat(PixelFormat internalFormat, bool compress);

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
		
	}

	void GLRenderEngine::Tick_Impl()
	{
		m_MainWindow->Tick();
		glClearColor(0.5f, 0.5f, 0.3f, 1.0f);

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

		if (generateMipMaps) {
			glGenerateMipmap(textureTarget);
		}
		else {
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

	void GLRenderEngine::SetShader_Impl(uint32 shader)
	{
		if (shader == m_BoundShader)
			return;

		glUseProgram(shader);
		m_BoundShader = shader;
	}

	void GLRenderEngine::SetShaderSampler_Impl(uint32 shader, const LinaString & samplerName, uint32 texture, uint32 sampler, uint32 unit)
	{

	}

	static GLint GetOpenGLFormat(PixelFormat format)
	{
		switch (format) {
		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return 0; // GL_RG;
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
		switch (format) {
		case PixelFormat::FORMAT_R: return GL_RED;
		case PixelFormat::FORMAT_RG: return 0;// GL_RG;
		case PixelFormat::FORMAT_RGB:
			if (compress) {
				return 0; // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			}
			else {
				return GL_RGB;
				//return GL_SRGB;
			}
		case PixelFormat::FORMAT_RGBA:
			if (compress) {
				// TODO: Decide on the default RGBA compression scheme
	//			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				return 0; // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			}
			else {
				return 0; // GL_SRGB_ALPHA;
			}
		case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
		case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return 0; // GL_DEPTH_STENCIL;
		default:
			LINA_CORE_ERR("PixelFormat {0} is not a valid PixelFormat.", format);
			return 0;
		};
	}

}

