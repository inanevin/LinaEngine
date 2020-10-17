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

/*
Class: TextureDrawer

Responsible for drawing texture settings.

Timestamp: 10/16/2020 6:27:20 PM
*/

#pragma once

#ifndef TextureDrawer_HPP
#define TextureDrawer_HPP

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
	
		void Setup(LinaEngine::Graphics::RenderEngine* renderEngine) { m_renderEngine = renderEngine; }
		void SetSelectedTexture(LinaEngine::Graphics::Texture* texture);
		void DrawSelectedTexture();
		int GetSamplerFilterID(LinaEngine::Graphics::SamplerFilter filter);
		int GetWrapModeID(LinaEngine::Graphics::SamplerWrapMode wrapMode);
		LinaEngine::Graphics::SamplerFilter GetSamplerFilterFromID(int id);
		LinaEngine::Graphics::SamplerWrapMode GetWrapModeFromID(int id);

	private:

		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Graphics::Texture* m_selectedTexture = nullptr;
		int m_currentInternalPixelFormat = 0;
		int m_currentPixelFormat = 0;
		int m_currentMinFilter = 0;
		int m_currentMagFilter = 0;
		int m_currentWrapS = 0;
		int m_currentWrapR = 0;
		int m_currentWrapT = 0;
		bool m_currentGenerateMips = 0;
		int m_currentAnisotropy = 0;
	
	};
}

#endif
