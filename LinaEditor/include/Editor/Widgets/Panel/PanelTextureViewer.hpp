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

#include "Editor/Widgets/Panel/PanelResourceViewer.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"

namespace Lina
{
	class Texture;
	class DirectionalLayout;
	class Button;
	class Dropdown;
	class Text;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class PanelTextureViewer : public PanelResourceViewer
	{
	public:
		PanelTextureViewer() : PanelResourceViewer(PanelType::TextureViewer, GetTypeID<Texture>(), GetTypeID<PanelTextureViewer>()){};
		virtual ~PanelTextureViewer() = default;

		virtual void Construct() override;
		virtual void Initialize() override;
		void		 UpdateTextureProps();
		virtual void RebuildContents() override;

	private:
		LINA_REFLECTION_ACCESS(PanelTextureViewer);

		String			m_textureFormat		= "";
		String			m_textureName		= "";
		String			m_textureDimensions = "";
		String			m_totalSizeKb		= "";
		uint32			m_mipLevel			= 0;
		DisplayChannels m_displayChannels	= DisplayChannels::RGBA;

		GUIUserData m_guiUserData;

		Dropdown* m_formatDropdown = nullptr;
		Widget*	  m_texturePanel   = nullptr;

		Text* m_txtName	  = nullptr;
		Text* m_txtSize	  = nullptr;
		Text* m_txtSizeKb = nullptr;

		Texture::Metadata m_prevMeta = {};
	};

	LINA_WIDGET_BEGIN(PanelTextureViewer, Hidden)
	LINA_FIELD(PanelTextureViewer, m_textureName, "Name", FieldType::StringFixed, 0)
	LINA_FIELD(PanelTextureViewer, m_textureFormat, "Format", FieldType::StringFixed, 0)
	LINA_FIELD(PanelTextureViewer, m_textureDimensions, "Dimensions", FieldType::StringFixed, 0)
	LINA_FIELD(PanelTextureViewer, m_totalSizeKb, "Size (All Mips)", FieldType::StringFixed, 0)
	LINA_FIELD(PanelTextureViewer, m_mipLevel, "Mip Level", FieldType::UInt32, 0)
	LINA_FIELD(PanelTextureViewer, m_displayChannels, "Display Channels", FieldType::Enum, GetTypeID<DisplayChannels>())
	LINA_FIELD_LIMITS(PanelTextureViewer, m_mipLevel, 0, 1, 1)
	LINA_CLASS_END(PanelTextureViewer)

} // namespace Lina::Editor
