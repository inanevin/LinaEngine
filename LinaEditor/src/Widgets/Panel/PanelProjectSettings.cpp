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

#include "Editor/Widgets/Panel/PanelProjectSettings.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Actions/EditorActionSettings.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"

namespace Lina::Editor
{
	void PanelProjectSettings::Construct()
	{
		m_editor = Editor::Get();

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(scroll);

		m_layout = m_manager->Allocate<DirectionalLayout>("Inspector");
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);
		m_layout->GetProps().direction				 = DirectionOrientation::Vertical;
		m_layout->GetWidgetProps().childPadding		 = Theme::GetDef().baseIndentInner;
		m_layout->GetWidgetProps().clipChildren		 = true;
		m_layout->GetWidgetProps().childMargins.left = Theme::GetDef().baseBorderThickness;
		m_layout->GetWidgetProps().childMargins.top	 = Theme::GetDef().baseIndent;
		scroll->AddChild(m_layout);

		FoldLayout* foldPackaging = CommonWidgets::BuildFoldTitle(this, Locale::GetStr(LocaleStr::Packaging), &m_foldPackaging);
		CommonWidgets::BuildClassReflection(foldPackaging, &m_settingsPackaging, ReflectionSystem::Get().Meta<PackagingSettings>());

		foldPackaging->GetCallbacks().onEditStarted = [this]() { m_oldSettingsPackaging = m_settingsPackaging; };
		foldPackaging->GetCallbacks().onEditEnded	= [this]() { EditorActionSettingsPackaging::Create(m_editor, m_oldSettingsPackaging, m_settingsPackaging); };
	}

	void PanelProjectSettings::SetSettingsPackaging(const PackagingSettings& settings)
	{
	}
} // namespace Lina::Editor
