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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/Graphics/Resource/Texture.hpp"

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

		UpdateTextureProps();
		m_texturePanel->GetWidgetProps().drawBackground	  = true;
		m_texturePanel->GetWidgetProps().fitTexture		  = true;
		m_texturePanel->GetWidgetProps().rawTexture		  = static_cast<Texture*>(m_resource);
		m_texturePanel->GetWidgetProps().colorBackground  = Color::White;
		m_texturePanel->GetWidgetProps().outlineThickness = 0.0f;
		m_texturePanel->GetWidgetProps().rounding		  = 0.0f;
		m_texturePanel->GetWidgetProps().textureUserData  = &m_displayUserData;
	}

	void PanelTextureViewer::OnGeneralMetaChanged(const MetaType& meta, FieldBase* field)
	{
		m_displayUserData.displayChannels = m_displayChannels;
		m_displayUserData.mipLevel		  = m_mipLevel;
	}

	void PanelTextureViewer::OnResourceMetaChanged(const MetaType& meta, FieldBase* field)
	{
		ReimportResource();
	}

	void PanelTextureViewer::RegenGPU()
	{
		UpdateTextureProps();
		Texture* txt = static_cast<Texture*>(m_resource);
		txt->DestroyHW();
		txt->GenerateHW();
		txt->AddToUploadQueue(m_editor->GetEditorRenderer().GetUploadQueue(), false);
		m_editor->GetEditorRenderer().MarkBindlessDirty();
	}

	void PanelTextureViewer::UpdateTextureProps()
	{
		Texture* txt		= static_cast<Texture*>(m_resource);
		m_textureName		= txt->GetName();
		m_textureDimensions = TO_STRING(txt->GetSize().x) + " x " + TO_STRING(txt->GetSize().y);
		m_totalSizeKb		= UtilStr::FloatToString(static_cast<float>(txt->GetTotalSize()) / 1000.0f, 1) + " KB";
		m_textureFormat		= ReflectionSystem::Get().Resolve<LinaGX::Format>().GetProperty<String>(static_cast<StringID>(txt->GetMeta().format));

		Widget*		mipLevelField	  = m_foldGeneral->FindChildWithDebugName("Mip Level");
		InputField* mipLevel		  = static_cast<InputField*>(Widget::GetWidgetOfType<InputField>(mipLevelField));
		mipLevel->GetProps().valueMax = static_cast<float>(txt->GetAllLevels().size() - 1);
	}

} // namespace Lina::Editor
