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
#include "Common/Math/Math.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{
	void Table::Construct()
	{
		m_scroll = m_manager->Allocate<ScrollArea>("TableScroll");
		m_scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_scroll->SetAlignedPos(Vector2::Zero);
		m_scroll->SetAlignedSize(Vector2::One);
		m_scroll->GetProps().direction = DirectionOrientation::Vertical;
		m_scroll->SetTarget(nullptr);
		AddChild(m_scroll);

		m_layout = m_manager->Allocate<DirectionalLayout>("Layout");
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);
		m_layout->GetProps().direction				= DirectionOrientation::Horizontal;
		m_layout->GetProps().mode					= DirectionalLayout::Mode::Bordered;
		m_layout->GetProps().colorBorder			= Theme::GetDef().background0;
		m_layout->GetProps().colorBorderHovered		= Theme::GetDef().foreground1;
		m_layout->GetWidgetProps().drawBackground	= true;
		m_layout->GetWidgetProps().colorBackground	= Theme::GetDef().background1;
		m_layout->GetProps().borderThickness		= Theme::GetDef().baseSeparatorThickness;
		m_layout->GetProps().borderExpandForMouse	= 4;
		m_layout->GetWidgetProps().outlineThickness = m_layout->GetWidgetProps().rounding = 0.0f;
		AddChild(m_layout);
	}

	void Table::BuildHeaders(const Vector<HeaderDefinition>& headerDefs)
	{
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();
		m_columns.clear();
		m_scroll->SetTarget(nullptr);
		m_scroll->SetDisplayTarget(nullptr);
		m_scroll->ClearOffsetTargets();

		if (headerDefs.empty())
			return;

		const float childCount	   = static_cast<float>(headerDefs.size());
		const float alignmentEqual = 1.0f / childCount;
		float		posX		   = 0.0f;

		for (const HeaderDefinition& def : headerDefs)
		{
			// add layout

			DirectionalLayout* vertical = m_manager->Allocate<DirectionalLayout>("Vertical");
			vertical->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
			vertical->SetAlignedPosY(0.0f);
			vertical->SetAlignedSizeY(1.0f);
			vertical->SetAlignedPosX(posX);
			vertical->SetAlignedSizeX(alignmentEqual);
			vertical->GetProps().direction = DirectionOrientation::Vertical;
			m_layout->AddChild(vertical);

			posX += alignmentEqual;

			Widget* header = nullptr;
			Text*	text   = nullptr;

			if (m_props.displayHeaders)
			{
				// add header

				const uint32 i = static_cast<uint32>(m_columns.size());

				if (def.clickable)
				{
					Button* but				  = m_manager->Allocate<Button>("Header");
					but->GetProps().onClicked = [this, i]() {
						if (m_props.onColumnClicked)
							m_props.onColumnClicked(i);
					};
					header								  = but;
					header->GetWidgetProps().colorHovered = Theme::GetDef().background4;
					header->GetWidgetProps().colorPressed = Theme::GetDef().background2;
				}
				else
					header = m_manager->Allocate<Widget>("Wrapper");

				header->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
				header->SetAlignedPosX(0.0f);
				header->SetAlignedSizeX(1.0f);
				header->SetFixedSizeY(Theme::GetDef().baseItemHeight);
				header->GetWidgetProps().drawBackground	  = true;
				header->GetWidgetProps().outlineThickness = header->GetWidgetProps().rounding = 0.0f;
				header->GetWidgetProps().colorBackground									  = Theme::GetDef().background3;
				vertical->AddChild(header);

				text = m_manager->Allocate<Text>("Text");
				text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
				text->SetAlignedPos(Vector2(0.5f, 0.5f));
				text->SetAnchorX(Anchor::Center);
				text->SetAnchorY(Anchor::Center);
				text->UpdateTextAndCalcSize(def.text);
				header->AddChild(text);
			}

			DirectionalLayout* contents = m_manager->Allocate<DirectionalLayout>("Contents");
			contents->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			contents->SetAlignedPosX(0.0f);
			contents->SetAlignedSize(Vector2(1.0f, 0.0f));
			contents->GetProps().direction			= DirectionOrientation::Vertical;
			contents->GetWidgetProps().clipChildren = true;
			vertical->AddChild(contents);

			m_columns.push_back({
				.columnText		= text,
				.verticalLayout = vertical,
				.contents		= contents,
			});

			if (m_columns.size() > 1)
				m_scroll->AddOffsetTarget(m_columns.back().contents);
		}

		m_scroll->SetTarget(m_columns[0].contents);
		m_scroll->SetDisplayTarget(m_columns.back().contents);
	}

	void Table::ClearRows()
	{
		for (const Column& column : m_columns)
		{
			column.contents->DeallocAllChildren();
			column.contents->RemoveAllChildren();
		}
	}

	void Table::AddRow(const Vector<Widget*>& columns)
	{
		LINA_ASSERT(columns.size() == m_columns.size(), "");

		const size_t sz = columns.size();
		for (size_t i = 0; i < sz; i++)
		{
			DirectionalLayout* vertical = m_columns[i].contents;
			Widget*			   w		= columns[i];
			w->GetWidgetProps().drawOrderIncrement++;

			if (m_props.useRowWrapper)
			{
				Widget* wrap = m_manager->Allocate<Widget>("Wrap");
				wrap->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
				wrap->SetAlignedPosX(0.0f);
				wrap->SetAlignedSizeX(1.0f);
				wrap->SetFixedSizeY(Theme::GetDef().baseItemHeight);
				wrap->GetWidgetProps().childMargins.left  = Theme::GetDef().baseIndent;
				wrap->GetWidgetProps().drawBackground	  = true;
				wrap->GetWidgetProps().outlineThickness	  = 0.0f;
				wrap->GetWidgetProps().rounding			  = 0.0f;
				wrap->GetWidgetProps().drawOrderIncrement = 1;
				wrap->GetWidgetProps().colorBackground	  = vertical->GetChildren().size() % 2 == 0 ? m_props.colorRow : m_props.colorRowAlt;
				wrap->AddChild(w);
				vertical->AddChild(wrap);
			}
			else
				vertical->AddChild(w);
		}
	}

} // namespace Lina::Editor
