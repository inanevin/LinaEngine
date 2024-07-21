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

#include "Editor/Widgets/Layout/ItemLayout.hpp"
#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Effects/Dropshadow.hpp"
#include "Core/GUI/Widgets/Primitives/Selectable.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"

namespace Lina::Editor
{
	void ItemLayout::Construct()
	{
		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		m_scroll					 = scroll;
		AddChild(scroll);
	}

	void ItemLayout::Initialize()
	{
		if (m_layout == nullptr)
		{
			CreateLayout();
			m_scroll->AddChild(m_layout);
		}
	}

	void ItemLayout::PreTick()
	{
	}

	void ItemLayout::CreateLayout()
	{
		if (m_useGridLayout)
		{
			GridLayout* grid = m_manager->Allocate<GridLayout>("Grid");
			grid->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			grid->SetAlignedPos(Vector2::Zero);
			grid->SetAlignedSize(Vector2::One);
			grid->GetProps().clipChildren = true;
			grid->SetChildPadding(Theme::GetDef().baseIndentInner);
			grid->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
			m_layout				= grid;
		}
		else
		{
			DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
			layout->GetProps().direction = DirectionOrientation::Vertical;
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			layout->SetAlignedPos(Vector2::Zero);
			layout->SetAlignedSize(Vector2::One);
			layout->GetProps().clipChildren = true;
			m_layout						= layout;
		}
	}

	void ItemLayout::ProcessItem(const ItemDefinition& def, Widget* parent, float margin)
	{
		Widget* it = CreateItem(def, margin, parent != nullptr);

		if (parent == nullptr)
			m_layout->AddChild(it);
		else
			parent->AddChild(it);

		if (m_props.itemsCanHaveChildren)
		{
			FoldLayout* fold = static_cast<FoldLayout*>(it);

			for (const ItemDefinition& d : def.children)
				ProcessItem(d, fold, margin + Theme::GetDef().baseIndent);
		}
	}
	void ItemLayout::RefreshItems()
	{
		m_gridItems.clear();
		m_listItems.clear();
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		Vector<ItemDefinition> items;
		if (m_props.onGatherItems)
			m_props.onGatherItems(items);

		for (const ItemDefinition& def : items)
			ProcessItem(def, nullptr, Theme::GetDef().baseIndent);
	}

	void ItemLayout::SetUseGridLayout(bool useGridLayout)
	{
		if (m_useGridLayout == useGridLayout)
			return;

		m_useGridLayout = useGridLayout;

		if (m_layout != nullptr)
		{
			m_manager->Deallocate(m_layout);
			m_scroll->RemoveAllChildren();
		}

		CreateLayout();
		m_scroll->AddChild(m_layout);
	}

	void ItemLayout::SetGridItemSize(const Vector2& size)
	{
		m_gridItemSize = size;
		for (Widget* w : m_gridItems)
			w->SetFixedSize(m_gridItemSize);
	}

	void ItemLayout::MakeVisibleRecursively(Widget* w)
	{
		Widget* parent = w->GetParent();
		if (parent == m_layout)
			return;

		FoldLayout* fold = static_cast<FoldLayout*>(parent);
		fold->SetIsUnfolded(true);

		parent->SetVisible(true);
		MakeVisibleRecursively(parent);
	}

	Widget* ItemLayout::CreateItem(const ItemDefinition& def, float margin, bool hasParent)
	{
		if (m_useGridLayout)
			return CreateGridItem(def);
		else
			return CreateListItem(def, margin, hasParent);
	}

	Widget* ItemLayout::CreateGridItem(const ItemDefinition& def)
	{
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		layout->SetFixedSize(m_gridItemSize);
		m_gridItems.push_back(layout);

		ShapeRect* bgShape = m_manager->Allocate<ShapeRect>("BG");
		bgShape->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_Y_COPY_X);
		bgShape->SetAlignedPosX(0.0f);
		bgShape->SetAlignedSizeX(1.0f);
		bgShape->GetProps().rounding   = Theme::GetDef().baseRounding;
		bgShape->GetProps().colorStart = Theme::GetDef().background2;
		bgShape->GetProps().colorEnd   = Theme::GetDef().background2;
		bgShape->GetProps().onlyRoundCorners.push_back(0);
		bgShape->GetProps().onlyRoundCorners.push_back(1);
		layout->AddChild(bgShape);

		if (def.useOutlineInGrid)
		{
			ShapeRect* bgShapeOutline = m_manager->Allocate<ShapeRect>("Outline");
			bgShapeOutline->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			bgShapeOutline->SetAlignedPos(Vector2::Zero);
			bgShapeOutline->SetAlignedSize(Vector2::One);
			bgShapeOutline->GetProps().rounding			= Theme::GetDef().baseRounding;
			bgShapeOutline->GetProps().colorStart.w		= 0.0f;
			bgShapeOutline->GetProps().colorEnd.w		= 0.0f;
			bgShapeOutline->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness * 1.5f;
			bgShapeOutline->GetProps().colorOutline		= Theme::GetDef().black;
			bgShapeOutline->GetProps().onlyRoundCorners.push_back(0);
			bgShapeOutline->GetProps().onlyRoundCorners.push_back(1);
			bgShape->AddChild(bgShapeOutline);
		}

		if (!def.icon.empty())
		{
			Icon* icon			  = m_manager->Allocate<Icon>("Folder");
			icon->GetProps().icon = def.icon;
			icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			icon->SetAlignedPos(Vector2(0.5f, 0.5f));
			icon->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.8f;
			bgShape->GetProps().colorStart.w = bgShape->GetProps().colorEnd.w = 0.0f;

			bgShape->AddChild(icon);
		}
		else
		{
			bgShape->GetProps().imageTextureAtlas = def.texture;
			bgShape->GetProps().fitImage		  = true;

			if (def.texture == nullptr)
			{
				LinaLoading* loading = m_manager->Allocate<LinaLoading>();
				loading->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				loading->SetAlignedPos(Vector2(0.5f, 0.5f));
				loading->SetAlignedSize(Vector2(0.25f, 0.25f));
				loading->SetPosAlignmentSourceX(PosAlignmentSource::Center);
				loading->SetPosAlignmentSourceY(PosAlignmentSource::Center);
				bgShape->AddChild(loading);
			}
			else
			{
				bgShape->GetProps().colorStart = Color(1, 1, 1, 1);
				bgShape->GetProps().colorEnd   = Color(1, 1, 1, 1);
			}
		}

		Selectable* selectable = m_manager->Allocate<Selectable>("Selectable");
		selectable->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_ALIGN_Y);
		selectable->SetAlignedPosX(0.0f);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetAlignedSizeY(0.0f);
		selectable->GetProps().rounding	  = Theme::GetDef().baseRounding;
		selectable->GetProps().colorStart = Theme::GetDef().background3;
		selectable->GetProps().colorEnd	  = Theme::GetDef().background3;
		selectable->SetDrawOrderIncrement(2);
		selectable->SetLocalControlsManager(this);
		selectable->Initialize();
		layout->AddChild(selectable);
		bgShape->GetProps().onClicked = [selectable]() { selectable->Select(); };

		Dropshadow* ds = m_manager->Allocate<Dropshadow>("Dropshadow");
		ds->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		ds->SetAlignedPos(Vector2::Zero);
		ds->SetAlignedSize(Vector2::One);
		ds->GetProps().direction = Direction::Bottom;
		ds->GetProps().color	 = Theme::GetDef().black;
		ds->GetProps().color.w	 = 0.75f;
		ds->GetProps().steps	 = 6;
		ds->SetDrawOrderIncrement(-1);
		selectable->AddChild(ds);

		Text* title = WidgetUtility::BuildEditableText(this, false, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		title->SetAlignedPos(Vector2(0.5f, 0.5f));
		title->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text				  = def.name;
		title->GetProps().fetchWrapFromParent = true;
		title->GetProps().wordWrap			  = false;
		selectable->AddChild(title);

		return layout;
	}

	Widget* ItemLayout::CreateListItem(const ItemDefinition& def, float margin, bool hasParent)
	{
		FoldLayout* fold	   = nullptr;
		Selectable* selectable = m_manager->Allocate<Selectable>("Selectable");
		selectable->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetUserData(def.userData);

		selectable->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		selectable->SetLocalControlsManager(this);
		selectable->GetProps().colorOutline = Theme::GetDef().accentPrimary0;

		if (m_props.itemsCanHaveChildren)
		{
			fold						   = m_manager->Allocate<FoldLayout>("Fold");
			fold->GetProps().useTween	   = true;
			fold->GetProps().tweenDuration = 0.25f;
			fold->GetProps().tweenPower	   = Theme::GetDef().baseIndentInner;
			fold->GetFlags().Set(WF_SIZE_ALIGN_X);

			if (!hasParent)
			{
				fold->GetFlags().Set(WF_POS_ALIGN_X);
				fold->SetAlignedPosX(0.0f);
			}
			fold->SetAlignedSizeX(1.0f);
			fold->SetIsUnfolded(m_areItemsUnfolded[def.userData]);
			fold->SetDebugName(def.name);

			selectable->GetProps().onSelectionChanged = [this](Selectable* s, bool selected) {
				if (selected && m_props.onItemSelected)
				{
					m_props.onItemSelected(s->GetUserData());
					m_lastSelectedItem = s->GetUserData();
				}
			};

			selectable->GetProps().onInteracted = [fold, this](Selectable* s) {
				fold->SetIsUnfolded(!fold->GetIsUnfolded());
				if (m_props.onItemInteracted)
					m_props.onItemInteracted(s->GetUserData());
			};

			fold->AddChild(selectable);

			fold->GetProps().onFoldChanged = [fold, def, this](bool unfolded) {
				Icon* icon			  = fold->GetWidgetOfType<Icon>(fold);
				icon->GetProps().icon = unfolded ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
				icon->CalculateIconSize();
				m_areItemsUnfolded[def.userData] = unfolded;
			};
		}
		else
		{
			selectable->SetDebugName(def.name);
			selectable->GetFlags().Set(WF_POS_ALIGN_X);
			selectable->SetAlignedPosX(0.0f);
		}

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
		layout->GetChildMargins().left = margin;
		selectable->AddChild(layout);

		if (m_props.itemsCanHaveChildren)
		{
			Icon* chevron			 = m_manager->Allocate<Icon>("Folder");
			chevron->GetProps().icon = m_areItemsUnfolded[def.userData] ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
			chevron->GetFlags().Set(WF_POS_ALIGN_Y);
			chevron->SetAlignedPosY(0.5f);
			chevron->GetProps().textScale = 0.4f;
			chevron->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			// chevron->GetProps().onClicked = [fold]() { fold->SetIsUnfolded(!fold->GetIsUnfolded()); };
			layout->AddChild(chevron);

			if (def.children.empty())
				chevron->SetVisible(false);
			else
			{
				selectable->GetProps().onClicked = [fold, chevron](Selectable* s) {
					if (chevron->GetIsHovered())
						fold->SetIsUnfolded(!fold->GetIsUnfolded());
				};
			}
		}

		if (!def.icon.empty())
		{
			Icon* icon			  = m_manager->Allocate<Icon>("Folder");
			icon->GetProps().icon = def.icon;
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(icon);
		}
		else
		{
			ShapeRect* img = m_manager->Allocate<ShapeRect>("Shape");
			img->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			img->SetAlignedPosY(0.5f);
			img->SetAlignedSizeY(0.8f);
			img->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			img->GetProps().imageTextureAtlas = def.texture;
			img->GetProps().fitImage		  = true;
			img->GetProps().colorStart		  = Color(1, 1, 1, 1);
			img->GetProps().colorEnd		  = Color(1, 1, 1, 1);
			layout->AddChild(img);

			if (def.texture == nullptr)
			{
				LinaLoading* loading = m_manager->Allocate<LinaLoading>();
				loading->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				loading->SetAlignedPos(Vector2(0.5f, 0.5f));
				loading->SetAlignedSize(Vector2(1.0f, 1.0f));
				loading->SetPosAlignmentSourceX(PosAlignmentSource::Center);
				loading->SetPosAlignmentSourceY(PosAlignmentSource::Center);
				img->GetProps().colorStart = Color(0, 0, 0, 0);
				img->GetProps().colorEnd   = Color(0, 0, 0, 0);
				img->AddChild(loading);
			}
		}

		Text* title = WidgetUtility::BuildEditableText(this, true, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text = def.name;
		layout->AddChild(title);

		Widget* listItem = m_props.itemsCanHaveChildren ? static_cast<Widget*>(fold) : selectable;
		m_listItems.push_back(listItem);
		return listItem;
	}

} // namespace Lina::Editor
