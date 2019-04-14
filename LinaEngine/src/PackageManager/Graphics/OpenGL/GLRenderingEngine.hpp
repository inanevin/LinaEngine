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

		enum PixelFormat
		{
			FORMAT_R,
			FORMAT_RG,
			FORMAT_RGB,
			FORMAT_RGBA,
			FORMAT_DEPTH,
			FORMAT_DEPTH_AND_STENCIL,
		};

		GLRenderingEngine();
		~GLRenderingEngine();
		
		void Initialize() override;
		void Start() override;
		void OnUpdate() override;
		void OnEvent(class Event& e)  override;
		void OnWindowEvent(class Event& e) override;
		void SetMousePosition(const Vector2F& v) override { GetMainWindow().SetMousePosition(v); }
		void SetWireframeMode(bool activation) override;

		virtual uint32 CreateTexture2D(int32 width, int32 height, const void* data, int pixelDataFormat, int internalPixelFormat, bool generateMipMaps, bool compress) override;

		
;
	private:

		int keyData = -1;
		void Test();

	
	};
}


#endif
#endif