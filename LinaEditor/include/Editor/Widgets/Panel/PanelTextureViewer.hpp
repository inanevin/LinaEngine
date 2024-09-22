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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	class Texture;
	class DirectionalLayout;
	class Button;
	class Dropdown;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class PanelTextureViewer : public Panel
	{
	public:
		PanelTextureViewer() : Panel(PanelType::TextureViewer, 0){};
		virtual ~PanelTextureViewer() = default;

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Destruct() override;

		virtual void SaveLayoutToStream(OStream& stream) override;
		virtual void LoadLayoutFromStream(IStream& stream) override;

	private:
		void StoreBuffer();
		void SetRuntimeDirty(bool isDirty);
		void RegenTexture(const String& path);

	private:
		LINA_REFLECTION_ACCESS(PanelTextureViewer);

		bool m_generalInfoFold = true;
		bool m_textureInfoFold = true;

		String			m_textureName	  = "";
		String			m_textureSize	  = "";
		DisplayChannels m_displayChannels = DisplayChannels::RGBA;

		GUIRendererUserData m_displayUserData;

		Dropdown*			  m_formatDropdown		   = nullptr;
		Button*				  m_saveButton			   = nullptr;
		Widget*				  m_texturePanel		   = nullptr;
		Editor*				  m_editor				   = nullptr;
		Texture*			  m_texture				   = nullptr;
		DirectionalLayout*	  m_inspector			   = nullptr;
		uint32				  m_mipLevel			   = 0;
		LinaGX::TextureBuffer m_textureBuffer		   = {};
		bool				  m_containsRuntimeChanges = false;
	};

	LINA_WIDGET_BEGIN(PanelTextureViewer, Hidden)
	LINA_FIELD(PanelTextureViewer, m_textureName, "Texture Name", FieldType::StringFixed, 0)
	LINA_FIELD(PanelTextureViewer, m_textureSize, "Texture Size", FieldType::StringFixed, 0)
	LINA_FIELD(PanelTextureViewer, m_mipLevel, "Mip Level", FieldType::UInt32, 0)
	LINA_FIELD(PanelTextureViewer, m_displayChannels, "Display Channels", FieldType::Enum, GetTypeID<DisplayChannels>())
	LINA_FIELD_LIMITS(PanelTextureViewer, m_mipLevel, 0, 1, 1)
	LINA_CLASS_END(PanelTextureViewer)

} // namespace Lina::Editor
