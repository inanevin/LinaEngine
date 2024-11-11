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

#include "Editor/Widgets/Panel/PanelTextureViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"

#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Meta/ProjectData.hpp"

namespace Lina::Editor
{
	void PanelTextureViewer::Construct()
	{
		PanelResourceViewer::Construct();
		Widget* texture = m_manager->Allocate<Widget>("Texture");
		texture->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		texture->SetAlignedPos(Vector2::One * 0.5f);
		texture->SetAlignedSize(Vector2::One);
		texture->SetAnchorX(Anchor::Center);
		texture->SetAnchorY(Anchor::Center);
		m_resourceBG->AddChild(texture);
		m_texturePanel = texture;
	}

	void PanelTextureViewer::Initialize()
	{
		PanelResourceViewer::Initialize();

		if (!m_resource)
			return;

		m_texturePanel->GetWidgetProps().drawBackground	  = true;
		m_texturePanel->GetWidgetProps().fitTexture		  = true;
		m_texturePanel->GetWidgetProps().rawTexture		  = static_cast<Texture*>(m_resource);
		m_texturePanel->GetWidgetProps().colorBackground  = Color::White;
		m_texturePanel->GetWidgetProps().outlineThickness = 0.0f;
		m_texturePanel->GetWidgetProps().rounding		  = 0.0f;
		m_texturePanel->GetWidgetProps().lvgUserData	  = &m_guiUserData;

		StoreEditorActionBuffer();
		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelTextureViewer::StoreEditorActionBuffer()
	{
		Texture* txt = static_cast<Texture*>(m_resource);
		m_prevMeta	 = txt->GetMeta();
	}

	void PanelTextureViewer::UpdateResourceProperties()
	{
		Texture* txt		= static_cast<Texture*>(m_resource);
		m_textureName		= txt->GetName();
		m_textureDimensions = TO_STRING(txt->GetSize().x) + " x " + TO_STRING(txt->GetSize().y);
		m_totalSizeKb		= UtilStr::FloatToString(static_cast<float>(txt->GetTotalSize()) / 1000.0f, 1) + " KB";
		m_textureFormat		= ReflectionSystem::Get().Resolve<LinaGX::Format>().GetProperty<String>(static_cast<StringID>(txt->GetMeta().format));
	}

	void PanelTextureViewer::RebuildContents()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		Texture* txt = static_cast<Texture*>(m_resource);

		CommonWidgets::BuildClassReflection(m_inspector, this, ReflectionSystem::Get().Resolve<PanelTextureViewer>(), [this](const MetaType& meta, FieldBase* field) {
			m_guiUserData.displayChannels = m_displayChannels;
			m_guiUserData.mipLevel		  = m_mipLevel;
		});

		CommonWidgets::BuildClassReflection(
			m_inspector, &txt->GetMeta(), ReflectionSystem::Get().Resolve<Texture::Metadata>(), [this, txt](const MetaType& meta, FieldBase* field) { EditorActionResourceTexture::Create(m_editor, txt->GetID(), m_prevMeta, txt->GetMeta()); });

		Widget*		mipLevelField	  = m_inspector->FindChildWithDebugName("Mip Level");
		InputField* mipLevel		  = static_cast<InputField*>(Widget::GetWidgetOfType<InputField>(mipLevelField));
		mipLevel->GetProps().valueMax = static_cast<float>(txt->GetAllLevels().size() - 1);
	}

} // namespace Lina::Editor
