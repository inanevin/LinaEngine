/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: RenderEngine
Timestamp: 4/15/2019 12:26:31 PM

*/

#pragma once

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "Lina/Events/Event.hpp"

namespace LinaEngine
{

	enum PixelFormat
	{
		FORMAT_R,
		FORMAT_RG,
		FORMAT_RGB,
		FORMAT_RGBA,
		FORMAT_DEPTH,
		FORMAT_DEPTH_AND_STENCIL,
	};

	struct ShaderProgram
	{
		//LinaArray<uint32> m_Shaders;
		//LinaMap<LinaString, uint32> m_UniformMap;
		//LinaMap<LinaString, uint32> m_SamplerMap;
	};

	template<class Derived>
	class RenderEngine
	{
	public:

		virtual ~RenderEngine()
		{
			LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
		}

		FORCEINLINE void* GetNativeWindow() { return m_Derived->GetNativeWindow_Impl(); }
		FORCEINLINE void Initialize() { m_Derived->Initialize_Impl(); }
		FORCEINLINE void Tick() { m_Derived->Tick_Impl(); }
		FORCEINLINE bool CreateContextWindow() { return m_Derived->CreateContextWindow_Impl(); }
		FORCEINLINE void SetMainWindowEventCallback(const std::function<void(Event&)>& callback) { m_Derived->SetMainWindowEventCallback_Impl(callback); }

		FORCEINLINE uint32 CreateTexture2D(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
		{
			m_Derived->CreateTexture2D_Impl(width, height, data, pixelDataFormat, internalPixelFormat, generateMipMaps, compress);
		}

		FORCEINLINE uint32 CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount)
		{
			m_Derived->CreateDDSTexture2D_Impl(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourceCC, uint32 mipMapCount);
		}

		FORCEINLINE uint32 ReleaseTexture2D(uint32 texture2D)
		{
			m_Derived->ReleaseTexture2D_Impl(uint32 texture2D);
		}

		FORCEINLINE void SetShader(uint32 shader)
		{
			m_Derived->SetShader_Impl(shader);
		}

		FORCEINLINE void SetShaderSampler(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit)
		{
			m_Derivd->SetShaderSampler_Impl(shader, samplerName, texture, sampler, unit);
		}

	protected:

		FORCEINLINE RenderEngine() 
		{
			LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());
			m_Derived = static_cast<Derived*>(this);
			//m_ContextWindow = std::make_unique<PAMWindow>();
			//m_ContextWindow->Initialize();
		}

	private:

		Derived* m_Derived;
		//std::unique_ptr<Window<PAMWindow>> m_ContextWindow;
	};
}


#endif