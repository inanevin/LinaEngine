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

#include "Editor/Widgets/Compound/Table.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{
	void Table::Construct()
	{
		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		AddChild(layout);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPosX(0.0f);
		scroll->SetAlignedSize(Vector2(1.0f, 0.0f));
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		layout->AddChild(scroll);

		m_controller = m_manager->Allocate<ItemController>("Controller");
		m_controller->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_controller->SetAlignedPos(Vector2::Zero);
		m_controller->SetAlignedSize(Vector2::One);
		scroll->AddChild(m_controller);

		m_contents = m_manager->Allocate<DirectionalLayout>("Contents");
		m_contents->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_contents->SetAlignedPos(Vector2::Zero);
		m_contents->SetAlignedSize(Vector2::One);
		m_contents->GetProps().direction			  = DirectionOrientation::Horizontal;
		m_contents->GetProps().mode					  = DirectionalLayout::Mode::Bordered;
		m_contents->GetProps().colorBorder			  = Theme::GetDef().silent2;
		m_contents->GetProps().colorBorderHovered	  = Theme::GetDef().foreground1;
		m_contents->GetWidgetProps().drawBackground	  = true;
		m_contents->GetWidgetProps().colorBackground  = Theme::GetDef().background1;
		m_contents->GetProps().borderThickness		  = Theme::GetDef().baseSeparatorThickness;
		m_contents->GetProps().borderExpandForMouse	  = 4;
		m_contents->GetWidgetProps().outlineThickness = m_contents->GetWidgetProps().rounding = 0.0f;
		m_controller->AddChild(m_contents);

		scroll->SetTarget(m_contents);
	}

	void Table::Initialize()
	{
		Widget::Initialize();
		Refresh();
	}

	void Table::Refresh()
	{
		m_contents->DeallocAllChildren();
		m_contents->RemoveAllChildren();
		m_controller->ClearItems();

		if (m_props.columns.empty())
			return;

		const float childCount	   = static_cast<float>(m_props.columns.size());
		const float alignmentEqual = 1.0f / childCount;
		float		posX		   = 0.0f;
		for (const Column& column : m_props.columns)
		{
			DirectionalLayout* vertical = m_manager->Allocate<DirectionalLayout>("Vertical");
			vertical->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
			vertical->SetAlignedPos(Vector2(posX, 0.0f));
			vertical->SetAlignedSize(Vector2(alignmentEqual, 1.0f));
			vertical->GetProps().direction = DirectionOrientation::Vertical;
			m_contents->AddChild(vertical);

			posX += alignmentEqual;

			Widget* widget = m_manager->Allocate<Widget>("Wrapper");
			widget->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_Y);
			widget->SetAlignedPosX(0.0f);
			widget->SetAlignedSizeX(1.0f);
			widget->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			widget->GetWidgetProps().drawBackground = true;
			widget->GetWidgetProps().rounding = widget->GetWidgetProps().outlineThickness = 0.0f;
			widget->GetWidgetProps().colorBackground									  = Theme::GetDef().background3;
			vertical->AddChild(widget);

			Text* text = m_manager->Allocate<Text>("Text");
			text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			text->SetAlignedPos(Vector2(0.5f, 0.5f));
			text->SetAnchorX(Anchor::Center);
			text->SetAnchorY(Anchor::Center);
			text->UpdateTextAndCalcSize(column.title);
			widget->AddChild(text);
		}
	}

	void Table::AddRow(const Vector<Widget*>& columns)
	{
		LINA_ASSERT(columns.size() == m_props.columns.size(), "");
	}

} // namespace Lina::Editor
