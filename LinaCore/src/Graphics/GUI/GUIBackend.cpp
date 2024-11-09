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
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	void GUIBackend::Initialize(ResourceManagerV2* resMan)
	{
		m_resourceManagerV2						  = resMan;
		m_lvgText.GetCallbacks().atlasNeedsUpdate = std::bind(&GUIBackend::FontAtlasNeedsUpdate, this, std::placeholders::_1);
	}

	void GUIBackend::Shutdown()
	{
		for (const auto& ft : m_fontAtlases)
		{
			if (ft.second.texture->IsHWValid())
				ft.second.texture->DestroyHW();

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
	}

	void GUIBackend::ReloadAtlases(ResourceUploadQueue& queue)
	{
		for (const Pair<LinaVG::Atlas*, FontTexture>& pair : m_fontAtlases)
		{
			const FontTexture& ft = pair.second;

			if (ft.texture->GetAllLevels().empty())
				continue;

			if (!ft.texture->IsHWValid())
				ft.texture->GenerateHW();

			ft.texture->AddToUploadQueue(queue, true);
		}
	}

} // namespace Lina
