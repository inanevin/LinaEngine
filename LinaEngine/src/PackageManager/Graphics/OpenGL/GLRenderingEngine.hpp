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

#pragma once

#ifdef LLF_GRAPHICS_OPENGL


#ifndef RenderingEngine_OpenGL_HPP
#define RenderingEngine_OpenGL_HPP

#include "Lina/Rendering/RenderingEngine.hpp"


namespace LinaEngine
{
	
	
	class GLRenderingEngine : public RenderingEngine
	{

	public:

		GLRenderingEngine();
		~GLRenderingEngine();
		
		void Initialize() override;
		void Start() override;
		void OnUpdate() override;
		void OnEvent(class Event& e)  override;
		void OnWindowEvent(class Event& e) override;
		void SetMousePosition(const Vector2F& v) override { GetMainWindow().SetMousePosition(v); }
		void SetWireframeMode(bool activation) override;

		virtual uint32 CreateTexture2D(int32 width, int32 height, const void* data, RenderingEngine::PixelFormat pixelDataFormat, RenderingEngine::PixelFormat internalPixelFormat, bool generateMipMaps, bool compress) override;
		virtual uint32 CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount) override;
		virtual uint32 ReleaseTexture2D(uint32 texture2D) override;
		virtual void SetShader(uint32 shader) override;
		virtual void SetShaderSampler(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit) override {};

	private:

		uint32 m_BoundShader = 0;

		int keyData = -1;
		void Test();

	
	};
}


#endif
#endif