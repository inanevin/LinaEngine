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

	}

	uint32 GLRenderEngine::CreateTexture2D_Impl(int32 width, int32 height, const void * data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
	{
		return uint32();
	}

	uint32 GLRenderEngine::CreateDDSTexture2D_Impl(uint32 width, uint32 height, const unsigned char * buffer, uint32 fourCC, uint32 mipMapCount)
	{
		return uint32();
	}

	uint32 GLRenderEngine::ReleaseTexture2D_Impl(uint32 texture2D)
	{
		return uint32();
	}

	void GLRenderEngine::SetShader_Impl(uint32 shader)
	{

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

