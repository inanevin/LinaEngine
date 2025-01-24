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
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	TextureAtlas::TextureAtlas(const Vector2ui& sz, LinaGX::Format format) : m_size(sz), m_textureFormat(format)
	{
		m_bytesPerPixel		  = GetBytesPerPixelFromFormat(format);
		const size_t dataSize = static_cast<size_t>(static_cast<size_t>(sz.x * sz.y * m_bytesPerPixel));
		uint8*		 data	  = new uint8[dataSize];
		m_data				  = {data, dataSize};
		MEMSET(m_data.data(), 0, dataSize);
		m_availableGrids.push_back(new Grid(Vector2ui::Zero, sz));
	}

	TextureAtlas::~TextureAtlas()
	{
		for (auto* r : m_rects)
			delete r;

		for (Grid* g : m_availableGrids)
			delete g;

		m_availableGrids.clear();
		m_rects.clear();

		m_rawTexture = nullptr;
		delete[] m_data.data();
	}

	void TextureAtlas::CreateRawTexture(ResourceManagerV2& rm)
	{
		LINA_ASSERT(m_rawTexture == nullptr, "");
		m_rawTexture							= rm.CreateResource<Texture>(rm.ConsumeResourceID(), "TextureAtlasRawTexture");
		m_rawTexture->GetMeta().format			= m_textureFormat;
		m_rawTexture->GetMeta().generateMipmaps = true;
	}

	void TextureAtlas::DestroyRawTexture(ResourceManagerV2& rm)
	{
		LINA_ASSERT(m_rawTexture != nullptr, "");
		m_rawTexture->DestroyHW();
		rm.DestroyResource(m_rawTexture);
		m_rawTexture = nullptr;
	}

	void TextureAtlas::Refresh(ResourceUploadQueue& queue)
	{
		m_rawTexture->LoadFromBuffer(m_data.data(), m_size.x, m_size.y, m_bytesPerPixel);

		if (!m_rawTexture->IsHWValid())
			m_rawTexture->GenerateHW();

		m_rawTexture->AddToUploadQueue(queue, false);
	}

	TextureAtlasImage* TextureAtlas::AddImage(uint8* data, const Vector2ui& size, StringID sid)
	{
		if (size.x > m_size.x || size.y > m_size.y)
		{
			return nullptr;
		}

		for (Grid* grid : m_availableGrids)
		{
			if (grid->rect.size.x < size.x || grid->rect.size.y < size.y)
				continue;

			TextureAtlasImage* img = new TextureAtlasImage();
			img->rectCoords.pos	   = grid->rect.pos;
			img->rectCoords.size   = size;
			img->rectUV.pos		   = {static_cast<float>(img->rectCoords.pos.x) / static_cast<float>(m_size.x), static_cast<float>(img->rectCoords.pos.y) / static_cast<float>(m_size.y)};
			img->rectUV.size	   = {static_cast<float>(size.x) / static_cast<float>(m_size.x), static_cast<float>(size.y) / static_cast<float>(m_size.y)};
			img->atlas			   = this;
			img->sid			   = sid;
			img->byteOffset		   = (img->rectCoords.pos.y * m_size.x + img->rectCoords.pos.x) * m_bytesPerPixel;

			for (uint32 row = 0; row < size.y; ++row)
			{
				uint8*		 dest = m_data.data() + img->byteOffset + row * m_size.x * m_bytesPerPixel;
				const uint8* src  = data + row * size.x * m_bytesPerPixel;
				MEMCPY(dest, src, size.x * m_bytesPerPixel);
			}

			const Vector2ui newGridRightPos	 = Vector2ui(grid->rect.pos.x + size.x, grid->rect.pos.y);
			const Vector2ui newGridRightSize = Vector2ui(grid->rect.pos.x + grid->rect.size.x - newGridRightPos.x, size.y);
			const Vector2ui newGridDownPos	 = Vector2ui(grid->rect.pos.x, grid->rect.pos.y + size.y);
			const Vector2ui newGridDownSize	 = Vector2ui(grid->rect.size.x, grid->rect.pos.y + grid->rect.size.y - newGridDownPos.y);

			Grid* newGridRight = new Grid(newGridRightPos, newGridRightSize);
			Grid* newGridDown  = new Grid(newGridDownPos, newGridDownSize);

			auto it = linatl::find_if(m_availableGrids.begin(), m_availableGrids.end(), [grid](Grid* g) -> bool { return g == grid; });
			m_availableGrids.erase(it);
			delete grid;

			m_availableGrids.push_back(newGridRight);
			m_availableGrids.push_back(newGridDown);
			m_rects.push_back(img);
			return img;
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
			Grid* newGrid = new Grid(rect->rectCoords.pos, rect->rectCoords.size);
			m_availableGrids.push_back(newGrid);

			uint32 startPos = (newGrid->rect.pos.y * m_size.x + newGrid->rect.pos.x) * m_bytesPerPixel;
			for (uint32 row = 0; row < newGrid->rect.size.y; row++)
			{
				MEMSET(m_data.data() + startPos, 0, newGrid->rect.size.x * m_bytesPerPixel);
				startPos += m_size.x * m_bytesPerPixel;
			}

			delete rect;
			m_rects.erase(it);
			return true;
		}

		return false;
	}

} // namespace Lina
