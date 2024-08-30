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

#include "Editor/Widgets/Panel/PanelWidgetEditorProperties.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina::Editor
{

	void PanelWidgetEditorProperties::Construct()
	{
		ScrollArea* sc = m_manager->Allocate<ScrollArea>("ScrollArea");
		sc->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		sc->SetAlignedPos(Vector2::Zero);
		sc->SetAlignedSize(Vector2::One);
		sc->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(sc);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetWidgetProps().clipChildren		 = true;
		layout->GetProps().direction				 = DirectionOrientation::Vertical;
		layout->GetWidgetProps().childMargins.top	 = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.left	 = Theme::GetDef().baseBorderThickness * 0.5f;
		layout->GetWidgetProps().childPadding		 = Theme::GetDef().baseIndentInner;
		m_layout									 = layout;
		sc->AddChild(m_layout);
	}

	void PanelWidgetEditorProperties::Refresh(Widget* w)
	{
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		if (w == nullptr)
			return;

		CommonWidgets::BuildClassReflection(m_layout, w, ReflectionSystem::Get().Resolve<Widget>());
	}

} // namespace Lina::Editor
