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

#include "Common/Math/Vector.hpp"
#include "Common/Math/Rect.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class Texture;
	class ResourceManagerV2;
	class ResourceUploadQueue;

	class TextureAtlas;
	struct TextureAtlasImage
	{
		StringID	  sid		 = 0;
		TextureAtlas* atlas		 = nullptr;
		size_t		  byteOffset = 0;
		Rect		  rectUV	 = {};
		Rectui		  rectCoords = {};
	};

	class TextureAtlas
	{
	public:
		struct Grid
		{
			Grid(const Vector2ui& pos, const Vector2ui& sz)
			{
				rect.pos  = pos;
				rect.size = sz;
			};

			Rectui rect = {};
		};

		TextureAtlas(const Vector2ui& sz, LinaGX::Format format);
		~TextureAtlas();
		TextureAtlasImage* AddImage(uint8* data, const Vector2ui& size, StringID sid = 0);
		TextureAtlasImage* GetImage(StringID sid);
		bool			   RemoveImage(TextureAtlasImage* rect);
		void			   Refresh(ResourceUploadQueue& queue);
		void			   CreateRawTexture(ResourceManagerV2& rm);
		void			   DestroyRawTexture(ResourceManagerV2& rm);

		inline Texture* GetRaw() const
		{
			return m_rawTexture;
		}

		inline uint32 GetBytesPerPixel() const
		{
			return m_bytesPerPixel;
		}

		inline LinaGX::Format GetFormat() const
		{
			return m_textureFormat;
		}

	private:
		Vector<Grid*>			   m_availableGrids;
		Vector2ui				   m_size		   = Vector2ui::Zero;
		uint32					   m_bytesPerPixel = 4;
		LinaGX::Format			   m_textureFormat = LinaGX::Format::R8G8B8A8_SRGB;
		Span<uint8>				   m_data;
		Texture*				   m_rawTexture = nullptr;
		Vector<TextureAtlasImage*> m_rects;
	};

} // namespace Lina
