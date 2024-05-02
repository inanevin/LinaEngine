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
}

namespace Lina
{
	class GfxManager;
	class ResourceManager;
	class Texture;
	class TextureSampler;
	class Buffer;

	class GUIBackend
	{
	public:
		struct Buffers
		{
			Buffer* vertexBuffer  = nullptr;
			Buffer* indexBuffer	  = nullptr;
			uint32	indexCounter  = 0;
			uint32	vertexCounter = 0;
		};

	private:
		struct DrawRequest
		{
			uint8		   requestType	= 0;
			uint32		   firstIndex	= 0;
			uint32		   vertexOffset = 0;
			uint32		   indexCount	= 0;
			Rectui		   clip			= {};
			GPUMaterialGUI materialData;
			bool		   hasTextureBind = false;
			uint32		   textureHandle  = 0;
			uint32		   samplerHandle  = 0;
		};

		struct DrawData
		{
			Vector<DrawRequest> drawRequests;
		};

		struct FontTexture
		{
			Texture* texture = nullptr;
			uint8*	 pixels	 = nullptr;
			int		 width	 = 0;
			int		 height	 = 0;
		};

	public:
		GUIBackend()  = default;
		~GUIBackend() = default;

		void				  Initialize(GfxManager* gfxMan);
		void				  Shutdown();
		void				  BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data);
		void				  BufferEnded();
		void				  BindFontTexture(LinaVG::BackendHandle texture);
		LinaVG::BackendHandle CreateFontTexture(int width, int height);

	private:
		StringID			m_boundFontTexture = 0;
		ResourceManager*	m_resourceManager  = nullptr;
		Vector<FontTexture> m_fontTextures;
	};
} // namespace Lina
