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

#include "Editor/Widgets/Panel/PanelFontViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"

namespace Lina::Editor
{

	void PanelFontViewer::Construct()
	{
		PanelResourceViewer::Construct();
		m_fontDisplay = m_manager->Allocate<Text>("Display");
		m_fontDisplay->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_fontDisplay->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_fontDisplay->SetAnchorX(Anchor::Center);
		m_fontDisplay->SetAnchorY(Anchor::Center);
		m_resourceBG->AddChild(m_fontDisplay);
	}

	void PanelFontViewer::Initialize()
	{
		PanelResourceViewer::Initialize();

		if (!m_resource)
			return;

		Font* font					   = static_cast<Font*>(m_resource);
		m_fontDisplay->GetProps().font = font->GetID();
		m_fontDisplay->UpdateTextAndCalcSize(m_displayString);
		m_fontDisplay->GetProps().valuePtr			  = &m_displayString;
		m_fontDisplay->GetProps().fetchWrapFromParent = true;
		m_fontDisplay->GetProps().isDynamic			  = true;

		StoreEditorActionBuffer();
		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelFontViewer::StoreEditorActionBuffer()
	{
		Font* font	 = static_cast<Font*>(m_resource);
		m_storedMeta = font->GetMeta();
	}

	void PanelFontViewer::UpdateResourceProperties()
	{
		Font* font = static_cast<Font*>(m_resource);
		m_fontName = font->GetName();
	}

	void PanelFontViewer::RebuildContents()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		Font* font = static_cast<Font*>(m_resource);

		DirectionalLayout* panelItems = m_manager->Allocate<DirectionalLayout>();
		panelItems->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		panelItems->SetAlignedSize(Vector2::One);
		panelItems->SetAlignedPosX(0.0f);
		panelItems->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		panelItems->GetProps().direction		  = DirectionOrientation::Vertical;
		panelItems->GetCallbacks().onEditEnded	  = [this]() { m_fontDisplay->GetProps().textScale = m_scale; };
		m_inspector->AddChild(panelItems);
		CommonWidgets::BuildClassReflection(panelItems, this, ReflectionSystem::Get().Resolve<PanelFontViewer>());

		CommonWidgets::BuildSeperator(m_inspector);

		DirectionalLayout* fontItems = m_manager->Allocate<DirectionalLayout>();
		fontItems->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		fontItems->SetAlignedSize(Vector2::One);
		fontItems->SetAlignedPosX(0.0f);
		fontItems->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		fontItems->GetProps().direction			 = DirectionOrientation::Vertical;
		fontItems->GetCallbacks().onEditEnded	 = [this, font]() { EditorActionResourceFont::Create(m_editor, font->GetID(), m_storedMeta, font->GetMeta()); };
		m_inspector->AddChild(fontItems);
		CommonWidgets::BuildClassReflection(fontItems, &font->GetMeta(), ReflectionSystem::Get().Resolve<Font::Metadata>());

		if (m_previewOnly)
			DisableRecursively(m_inspector);
	}

} // namespace Lina::Editor
