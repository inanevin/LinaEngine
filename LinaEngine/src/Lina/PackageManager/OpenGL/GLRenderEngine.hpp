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

#pragma once

#ifndef GLRenderEngine_HPP
#define GLRenderEngine_HPP

#include "Lina/Rendering/RenderEngine.hpp"
#include "Lina/PackageManager/PAMWindow.hpp"

namespace LinaEngine::Graphics
{

#define MAKEFOURCC(a, b, c, d)                              \
                ((uint32)(uint8)(a) | ((uint32)(uint8)(b) << 8) |       \
				((uint32)(uint8)(c) << 16) | ((uint32)(uint8)(d) << 24 ))

#define MAKEFOURCCDXT(a) MAKEFOURCC('D', 'X', 'T', a)
#define FOURCC_DXT1 MAKEFOURCCDXT('1')
#define FOURCC_DXT2 MAKEFOURCCDXT('2')
#define FOURCC_DXT3 MAKEFOURCCDXT('3')
#define FOURCC_DXT4 MAKEFOURCCDXT('4')
#define FOURCC_DXT5 MAKEFOURCCDXT('5')

	// A derived class of RenderEngine. Check RenderEngine.hpp for method details.
	class GLRenderEngine : public RenderEngine<GLRenderEngine>
	{
	public:

		GLRenderEngine();
		virtual ~GLRenderEngine();

		void Initialize_Impl() STATIC_OVERRIDE;
		void Tick_Impl() STATIC_OVERRIDE;

		FORCEINLINE bool CreateContextWindow_Impl() STATIC_OVERRIDE 
		{ 
			return m_MainWindow->Initialize(); 
		}
		FORCEINLINE void* GetNativeWindow_Impl() STATIC_OVERRIDE 
		{ 
			return m_MainWindow->GetNativeWindow();
		}

		FORCEINLINE void SetMainWindowEventCallback_Impl(const std::function<void(Event&)>& callback) STATIC_OVERRIDE 
		{ 
			m_MainWindow->SetEventCallback(callback);
		}

		uint32 CreateTexture2D_Impl(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress) STATIC_OVERRIDE;
		uint32 CreateDDSTexture2D_Impl(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount) STATIC_OVERRIDE;
		uint32 ReleaseTexture2D_Impl(uint32 texture2D) STATIC_OVERRIDE;
		void SetShader_Impl(uint32 shader) STATIC_OVERRIDE;
		void SetShaderSampler_Impl(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit) STATIC_OVERRIDE;

	private:

		// Main window reference.
		std::unique_ptr<Window<PAMWindow>> m_MainWindow;

		// Currently active shader.
		uint32 m_BoundShader = 0;

	};
}



#endif