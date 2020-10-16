/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: TextureDrawer
Timestamp: 10/16/2020 6:27:20 PM

*/
#pragma once

#ifndef TextureDrawer_HPP
#define TextureDrawer_HPP

// Headers here.
#include "Rendering/RenderingCommon.hpp"
#include "Core/Common.hpp"
namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Mesh;
		class Texture;
		class Material;
	}
}

namespace LinaEditor
{
	class TextureDrawer
	{
		
	public:
		
		TextureDrawer() {};
		~TextureDrawer() {};
	

		// Drawing textures
		FORCEINLINE void Setup(LinaEngine::Graphics::RenderEngine* renderEngine) { m_renderEngine = renderEngine; }
		void SetSelectedTexture(LinaEngine::Graphics::Texture* texture);
		void DrawSelectedTexture();
		int GetSamplerFilterID(LinaEngine::Graphics::SamplerFilter filter);
		int GetWrapModeID(LinaEngine::Graphics::SamplerWrapMode wrapMode);
		LinaEngine::Graphics::SamplerFilter GetSamplerFilterFromID(int id);
		LinaEngine::Graphics::SamplerWrapMode GetWrapModeFromID(int id);

	private:

		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Graphics::Texture* m_selectedTexture = nullptr;
		int m_currentInternalPixelFormat;
		int m_currentPixelFormat;
		int m_currentMinFilter;
		int m_currentMagFilter;
		int m_currentWrapS;
		int m_currentWrapR;
		int m_currentWrapT;
		bool m_currentGenerateMips;
		int m_currentAnisotropy;

	
	};
}

#endif
