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

#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	TextureAtlas::TextureAtlas(StringID uniqueID, ResourceManagerV2* rm, const Vector2ui& sz, uint32 bytesPerPixel, LinaGX::Format format) : m_size(sz), m_bytesPerPixel(bytesPerPixel), m_textureFormat(format), m_resourceManagerV2(rm)
	{
		const size_t dataSize = static_cast<size_t>(static_cast<size_t>(sz.x * sz.y * bytesPerPixel));
		uint8*		 data	  = new uint8[dataSize];
		m_data				  = {data, dataSize};
		MEMSET(m_data.data(), 0, dataSize);
		m_rawTexture = m_resourceManagerV2->CreateResource<Texture>("TextureAtlasRawTexture", uniqueID);
		m_isDirty	 = true;
	}

	TextureAtlas::~TextureAtlas()
	{
		for (auto* r : m_rects)
			delete r;
		m_rects.clear();

		m_resourceManagerV2->DestroyResource(m_rawTexture);
		m_rawTexture = nullptr;
	}

	TextureAtlasImage* TextureAtlas::AddImage(uint8* data, const Vector2ui& size, StringID sid)
	{
		if (size.x > m_size.x || size.y > m_size.y)
		{
			return nullptr;
		}

		for (uint32 y = 0; y <= m_size.y - size.y; ++y)
		{
			for (uint32 x = 0; x <= m_size.x - size.x; ++x)
			{

				bool isFree = true;
				for (TextureAtlasImage* rect : m_rects)
				{
					const Rectui& rectCoords = rect->rectCoords;

					if (!(x + size.x <= rectCoords.pos.x || x >= rectCoords.pos.x + rectCoords.size.x || y + size.y <= rectCoords.pos.y || y >= rectCoords.pos.y + rectCoords.size.y))
					{
						isFree = false;
						break;
					}
				}

				if (isFree)
				{
					TextureAtlasImage* newRect = new TextureAtlasImage();
					newRect->byteOffset		   = (y * m_size.x + x) * m_bytesPerPixel;
					newRect->rectCoords.pos	   = {x, y};
					newRect->rectCoords.size   = size;
					newRect->rectUV.pos		   = {static_cast<float>(x) / static_cast<float>(m_size.x), static_cast<float>(y) / static_cast<float>(m_size.y)};
					newRect->rectUV.size	   = {static_cast<float>(size.x) / static_cast<float>(m_size.x), static_cast<float>(size.y) / static_cast<float>(m_size.y)};
					newRect->atlas			   = this;
					newRect->sid			   = sid;

					for (uint32 row = 0; row < size.y; ++row)
					{
						uint8*		 dest = m_data.data() + newRect->byteOffset + row * m_size.x * m_bytesPerPixel;
						const uint8* src  = data + row * size.x * m_bytesPerPixel;
						MEMCPY(dest, src, size.x * m_bytesPerPixel);
					}

					m_isDirty = true;
					m_rects.push_back(newRect);
					return newRect;
				}
			}
		}

		return nullptr;
	}

	TextureAtlasImage* TextureAtlas::GetImage(StringID sid)
	{
		auto it = linatl::find_if(m_rects.begin(), m_rects.end(), [sid](TextureAtlasImage* r) -> bool { return r->sid == sid; });

		if (it != m_rects.end())
			return *it;

		return nullptr;
	}

	bool TextureAtlas::RemoveImage(TextureAtlasImage* rect)
	{
		auto it = linatl::find_if(m_rects.begin(), m_rects.end(), [rect](TextureAtlasImage* r) -> bool { return r == rect; });

		if (it != m_rects.end())
		{
			delete rect;
			m_rects.erase(it);
		}
	}

	void TextureAtlas::RefreshGPU(ResourceUploadQueue& queue)
	{
		if (!m_isDirty)
			return;

		m_isDirty = false;
		m_rawTexture->LoadFromBuffer(m_data.data(), m_size.x, m_size.y, m_bytesPerPixel, m_bytesPerPixel == 1 ? LinaGX::ImageChannelMask::G : LinaGX::ImageChannelMask::RGBA, m_textureFormat, true);

		if (!m_rawTexture->IsGPUValid())
			m_rawTexture->GenerateHW();

		m_rawTexture->AddToUploadQueue(queue);
	}
} // namespace Lina
