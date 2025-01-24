/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#include "Common/Data/Vector.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Rect.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Graphics/Data/RenderData.hpp"

namespace LinaVG
{
	struct DrawBuffer;
	class Atlas;
} // namespace LinaVG

namespace Lina
{
	class ResourceManagerV2;
	class Texture;
	class TextureSampler;
	class Buffer;
	class ResourceUploadQueue;

	class GUIBackend
	{
	public:
	private:
		struct FontTexture
		{
			Texture* texture = nullptr;
			uint32	 width	 = 0;
			uint32	 height	 = 0;
		};

	public:
		GUIBackend()  = default;
		~GUIBackend() = default;

		void Initialize(ResourceManagerV2* resourceManager);
		void Shutdown();
		void FontAtlasNeedsUpdate(LinaVG::Atlas* atlas);
		void ReloadAtlases(ResourceUploadQueue& queue);

		inline const FontTexture& GetFontTexture(LinaVG::Atlas* atlas) const
		{
			return m_fontAtlases.at(atlas);
		}

		inline LinaVG::Text& GetLVGText()
		{
			return m_lvgText;
		}

	private:
		LinaVG::Text						 m_lvgText;
		StringID							 m_boundFontTexture	 = 0;
		ResourceManagerV2*					 m_resourceManagerV2 = nullptr;
		HashMap<LinaVG::Atlas*, FontTexture> m_fontAtlases;
	};
} // namespace Lina
