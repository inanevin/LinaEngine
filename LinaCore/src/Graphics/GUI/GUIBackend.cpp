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

#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	void GUIBackend::Initialize(ResourceManagerV2* resMan, ResourceUploadQueue* uploadQueue)
	{
		m_resourceManagerV2						  = resMan;
		m_uploadQueue							  = uploadQueue;
		m_lvgText.GetCallbacks().atlasNeedsUpdate = std::bind(&GUIBackend::FontAtlasNeedsUpdate, this, std::placeholders::_1);
	}

	void GUIBackend::Shutdown()
	{
		for (const auto& ft : m_fontAtlases)
		{
			m_resourceManagerV2->DestroyResource<Texture>(ft.second.texture);
		}

		m_fontAtlases.clear();
	}

	void GUIBackend::FontAtlasNeedsUpdate(LinaVG::Atlas* atlas)
	{
		auto		 it		= m_fontAtlases.find(atlas);
		const uint32 width	= atlas->GetSize().x;
		const uint32 height = atlas->GetSize().y;

		if (it == m_fontAtlases.end())
		{
			const String name		 = "GUI Backend Font Texture " + TO_STRING(m_fontAtlases.size());
			FontTexture	 fontTexture = {
				 .texture = m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), name),
				 .width	  = width,
				 .height  = height,
			 };
			fontTexture.texture->GetMeta().format		   = LinaGX::Format::R8_UNORM;
			fontTexture.texture->GetMeta().generateMipmaps = true;
			m_fontAtlases[atlas]						   = fontTexture;
		}

		auto& ft = m_fontAtlases[atlas];
		ft.texture->LoadFromBuffer(atlas->GetData(), width, height, 1);
		if (!ft.texture->IsGPUValid())
			ft.texture->GenerateHW();
		ft.texture->AddToUploadQueue(*m_uploadQueue, true);
	}
	/*
		void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
		{
			auto&  ft		   = m_fontTextures[m_boundFontTexture];
			uint32 startOffset = offsetY * ft.width + offsetX;

			for (int i = 0; i < height; i++)
			{
				const uint32 size = width;
				MEMCPY(ft.pixels + startOffset, &data[width * i], size);
				startOffset += ft.width;
			}
		}

		void GUIBackend::BufferEnded()
		{
			auto& ft = m_fontTextures[m_boundFontTexture];
			ft.texture->LoadFromBuffer(ft.pixels, ft.width, ft.height, 1);

			if (!ft.texture->IsGPUValid())
				ft.texture->GenerateHW();

			ft.texture->AddToUploadQueue(*m_uploadQueue, true);
		}

		void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
		{
			m_boundFontTexture = texture;
		}

		LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
		{
			const String name		 = "GUI Backend Font Texture " + TO_STRING(m_fontTextures.size());
			FontTexture	 fontTexture = {
				 .texture = m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), name),
				 .width	  = width,
				 .height  = height,
			 };

			fontTexture.texture->GetMeta().format		   = LinaGX::Format::R8_UNORM;
			fontTexture.texture->GetMeta().generateMipmaps = true;
			fontTexture.pixels							   = new uint8[width * height];
			memset(fontTexture.pixels, 0, width * height);

			m_fontTextures.push_back(fontTexture);
			m_boundFontTexture = static_cast<LinaVG::BackendHandle>(m_fontTextures.size() - 1);
			return m_boundFontTexture;
		}
	*/
} // namespace Lina
