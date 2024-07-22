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
#include "Editor/EditorLocale.hpp"
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
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void ItemLayout::Construct()
	{
		GetFlags().Set(WF_CONTROLLABLE);
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
		if (m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
		{
			if (m_isHovered && !m_isFocused)
				m_isFocused = true;
			else if (m_isFocused && !m_isHovered)
				m_isFocused = false;
		}

		const Color dead = m_useGridLayout ? Theme::GetDef().background3 : Color(0.0f, 0.0f, 0.0f, 0.0f);

		for (ShapeRect* item : m_selectionItems)
		{
			const bool selected = IsItemSelected(item);

			if (selected)
			{
				item->GetProps().colorStart = m_isFocused ? Theme::GetDef().accentPrimary0 : Theme::GetDef().silent0;
				item->GetProps().colorEnd	= m_isFocused ? Theme::GetDef().accentPrimary1 : Theme::GetDef().silent1;
			}
			else
			{
				item->GetProps().colorStart = item->GetProps().colorEnd = dead;
			}
		}
	}

	void ItemLayout::OnGrabbedControls(bool isForward, Widget* prevControls)
	{
		if (prevControls == this)
			return;

		if (!m_selectedItems.empty())
		{
			m_isFocused = true;
			return;
		}

		if (!m_selectionItems.empty())
		{
			ShapeRect* item = nullptr;

			if (isForward)
			{
				for (ShapeRect* r : m_selectionItems)
				{
					if (r->GetIsVisible() && !r->GetIsDisabled())
					{
						item = r;
						break;
					}
				}
			}
			else
			{
				for (Vector<ShapeRect*>::iterator it = m_selectionItems.end() - 1; it >= m_selectionItems.begin(); it--)
				{
					ShapeRect* r = *it;
					if (r->GetIsVisible() && !r->GetIsDisabled())
					{
						item = r;
						break;
					}
				}
			}

			if (item)
			{
				SelectItem(item);
				m_isFocused = true;
			}
		}
	}

	Widget* ItemLayout::GetNextControls()
	{
		if (m_selectedItems.empty())
			return nullptr;

		Widget* currentSelected = m_selectedItems.back();

		const int32 sz	  = static_cast<int32>(m_selectionItems.size());
		bool		found = false;

		for (int32 i = 0; i < sz; i++)
		{
			ShapeRect* item = m_selectionItems[i];

			if (item == currentSelected)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (item->GetIsVisible() && !item->GetIsDisabled())
				{
					SelectItem(item);
					return this;
				}
			}
		}

		m_isFocused = false;
		return nullptr;
	}

	Widget* ItemLayout::GetPrevControls()
	{
		if (m_selectedItems.empty())
			return nullptr;

		Widget* currentSelected = m_selectedItems.front();

		const int32 sz	  = static_cast<int32>(m_selectionItems.size());
		bool		found = false;

		for (int32 i = sz - 1; i >= 0; i--)
		{
			ShapeRect* item = m_selectionItems[i];

			if (item == currentSelected)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (item->GetIsVisible() && !item->GetIsDisabled())
				{
					SelectItem(item);
					return this;
				}
			}
		}

		m_isFocused = false;
		return nullptr;
	}

	bool ItemLayout::OnKey(uint32 key, int32 scancode, LinaGX::InputAction act)
	{
		if (m_selectedItems.empty() || !m_isFocused)
			return false;

		ShapeRect* item = m_selectedItems.back();

		if (!m_useGridLayout && act == LinaGX::InputAction::Pressed)
		{
			if (key == LINAGX_KEY_RETURN)
			{
				if (m_props.itemsCanHaveChildren)
				{
					FoldLayout* fold = static_cast<FoldLayout*>(item->GetParent());
					fold->SetIsUnfolded(!fold->GetIsUnfolded());
				}
				else
				{
					if (m_props.onItemInteracted)
						m_props.onItemInteracted(item->GetUserData());
				}

				return true;
			}
			else if (key == LINAGX_KEY_RIGHT && m_props.itemsCanHaveChildren)
			{
				FoldLayout* fold = static_cast<FoldLayout*>(item->GetParent());
				fold->SetIsUnfolded(true);
				return true;
			}
			else if (key == LINAGX_KEY_LEFT && m_props.itemsCanHaveChildren)
			{
				FoldLayout* fold = static_cast<FoldLayout*>(item->GetParent());
				fold->SetIsUnfolded(false);
				return true;
			}
			else if (key == LINAGX_KEY_UP)
			{
				if (GetPrevControls() != nullptr)
					m_manager->MoveControlsToPrev();
				return true;
			}
			else if (key == LINAGX_KEY_DOWN)
			{
				if (GetNextControls() != nullptr)
					m_manager->MoveControlsToNext();
				return true;
			}
			else if (key == LINAGX_KEY_DELETE)
			{
				if (m_props.onItemDelete)
					m_props.onItemDelete(item->GetUserData());

				return true;
			}
		}
		else if (m_useGridLayout && act == LinaGX::InputAction::Pressed)
		{
			if (key == LINAGX_KEY_RETURN)
			{
				if (m_props.onItemInteracted)
					m_props.onItemInteracted(item->GetUserData());

				return true;
			}
			else if (key == LINAGX_KEY_RIGHT)
			{
				if (GetNextControls() != nullptr)
					m_manager->MoveControlsToNext();
				return true;
			}
			else if (key == LINAGX_KEY_LEFT)
			{
				if (GetPrevControls() != nullptr)
					m_manager->MoveControlsToPrev();
				return true;
			}
			else if (key == LINAGX_KEY_DELETE)
			{
				if (m_props.onItemDelete)
					m_props.onItemDelete(item->GetUserData());
				return true;
			}
		}

		return false;
	}

	bool ItemLayout::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button == LINAGX_MOUSE_0 && m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			for (ShapeRect* it : m_selectionItems)
			{
				if (it->GetIsHovered())
				{
					// if (!m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
					// 	m_selectedItems.clear();
					// else if (IsItemSelected(it))
					// {
					// 	RemoveItemFromSelected(it);
					// 	return true;
					// }

					SelectItem(it);

					if (!m_useGridLayout && m_props.itemsCanHaveChildren)
					{
						Icon* chevron = it->GetWidgetOfType<Icon>(it);
						if (chevron != nullptr)
						{
							if (chevron->GetIsHovered())
							{
								FoldLayout* fold = static_cast<FoldLayout*>(it->GetParent());
								fold->SetIsUnfolded(!fold->GetIsUnfolded());
							}
						}
					}

					return true;
				}
			}

			if (m_props.onItemSelected)
				m_props.onItemSelected(nullptr);
			m_selectedItems.clear();
		}

		if (button == LINAGX_MOUSE_0 && m_isHovered && act == LinaGX::InputAction::Repeated)
		{
			for (ShapeRect* it : m_selectionItems)
			{
				if (it->GetIsHovered())
				{
					if (!m_useGridLayout && m_props.itemsCanHaveChildren)
					{
						FoldLayout* fold = static_cast<FoldLayout*>(it->GetParent());
						fold->SetIsUnfolded(!fold->GetIsUnfolded());
						return true;
					}
					else if (!m_useGridLayout && !m_props.itemsCanHaveChildren)
					{
						if (m_props.onItemInteracted)
							m_props.onItemInteracted(it->GetUserData());
						return true;
					}
				}
			}
		}

		return false;
	}

	bool ItemLayout::IsItemSelected(ShapeRect* rect)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [rect](ShapeRect* r) -> bool { return rect == r; });
		return it != m_selectedItems.end();
	}

	void ItemLayout::RemoveItemFromSelected(ShapeRect* rect)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [rect](ShapeRect* r) -> bool { return rect == r; });
		m_selectedItems.erase(it);

		if (!m_useGridLayout && m_props.itemsCanHaveChildren)
		{
			Icon* chevron = rect->GetWidgetOfType<Icon>(rect);
			if (chevron != nullptr)
			{
				chevron->GetProps().icon = ICON_CHEVRON_DOWN;
				chevron->CalculateIconSize();
			}
		}
	}

	void ItemLayout::SelectItem(ShapeRect* r)
	{
		UnfoldRecursively(r->GetParent());
		m_scroll->ScrollToChild(r);
		m_selectedItems.clear();

		m_manager->GrabControls(this);

		if (m_props.onItemSelected)
			m_props.onItemSelected(r->GetUserData());

		m_selectedItems.push_back(r);
	}

	void ItemLayout::UnfoldRecursively(Widget* w)
	{
		Widget* parent = w->GetParent();

		if (parent != nullptr && parent->GetTID() == GetTypeID<FoldLayout>())
		{
			FoldLayout* f = static_cast<FoldLayout*>(parent);
			f->SetIsUnfolded(true);
			UnfoldRecursively(f);
		}
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

	void ItemLayout::SetSelectedItem(void* userData)
	{
		for (ShapeRect* it : m_selectionItems)
		{
			if (it->GetUserData() == userData)
			{
				m_isFocused = true;
				SelectItem(it);
				break;
			}
		}
	}

	void ItemLayout::RefreshItems()
	{
		m_selectedItems.clear();
		m_selectionItems.clear();
		m_gridItems.clear();
		m_listItems.clear();
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		Vector<ItemDefinition> items;
		if (m_props.onGatherItems)
			m_props.onGatherItems(items);

		for (const ItemDefinition& def : items)
			ProcessItem(def, nullptr, Theme::GetDef().baseIndent);

		if (items.empty())
		{
			Text* nothingToSee			  = m_manager->Allocate<Text>("NothingToSee");
			nothingToSee->GetProps().text = Locale::GetStr(LocaleStr::NothingInDirectory);
			nothingToSee->GetProps().font = BIG_FONT_SID;
			nothingToSee->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			nothingToSee->SetAlignedPos(Vector2(0.5f, 0.5f));
			nothingToSee->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			nothingToSee->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			nothingToSee->GetProps().color = Theme::GetDef().silent2;
			m_layout->AddChild(nothingToSee);
		}
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
			icon->GetProps().colorStart = icon->GetProps().colorEnd = Theme::GetDef().foreground1;
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

		ShapeRect* shape = m_manager->Allocate<ShapeRect>("Shape");
		shape->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_ALIGN_Y);
		shape->SetAlignedPosX(0.0f);
		shape->SetAlignedSizeX(1.0f);
		shape->SetAlignedSizeY(0.0f);
		shape->GetProps().rounding				= Theme::GetDef().baseRounding;
		shape->GetProps().interpolateColor		= true;
		shape->GetProps().colorInterpolateSpeed = 20.0f;
		shape->SetDrawOrderIncrement(2);
		shape->SetLocalControlsManager(this);
		shape->SetUserData(def.userData);
		shape->GetProps().colorStart = shape->GetProps().colorEnd = Theme::GetDef().background3;
		shape->Initialize();

		bgShape->GetProps().onClicked		= [shape, this]() { SelectItem(shape); };
		bgShape->GetProps().onDoubleClicked = [this, shape]() {
			if (m_props.onItemInteracted)
				m_props.onItemInteracted(shape->GetUserData());
		};
		shape->GetProps().onDoubleClicked = [this, shape]() {
			if (m_props.onItemInteracted)
				m_props.onItemInteracted(shape->GetUserData());
		};
		layout->AddChild(shape);
		m_selectionItems.push_back(shape);

		Dropshadow* ds = m_manager->Allocate<Dropshadow>("Dropshadow");
		ds->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		ds->SetAlignedPos(Vector2::Zero);
		ds->SetAlignedSize(Vector2::One);
		ds->GetProps().direction = Direction::Bottom;
		ds->GetProps().color	 = Theme::GetDef().black;
		ds->GetProps().color.w	 = 0.75f;
		ds->GetProps().steps	 = 6;
		ds->SetDrawOrderIncrement(-1);
		shape->AddChild(ds);

		Text* title = WidgetUtility::BuildEditableText(this, false, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		title->SetAlignedPos(Vector2(0.5f, 0.5f));
		title->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text				  = def.name;
		title->GetProps().fetchWrapFromParent = true;
		title->GetProps().wordWrap			  = false;
		shape->AddChild(title);

		return layout;
	}

	Widget* ItemLayout::CreateListItem(const ItemDefinition& def, float margin, bool hasParent)
	{
		FoldLayout* fold = nullptr;

		if (def.unfoldOverride)
			m_areItemsUnfolded[def.userData] = true;

		ShapeRect* shape = m_manager->Allocate<ShapeRect>("Shape");
		shape->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		shape->SetAlignedSizeX(1.0f);
		shape->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		shape->GetProps().interpolateColor		= true;
		shape->GetProps().colorInterpolateSpeed = 20.0f;
		shape->GetProps().colorStart			= Color(0.0f, 0.0f, 0.0f, 0.0f);
		shape->GetProps().colorEnd				= Color(0.0f, 0.0f, 0.0f, 0.0f);
		shape->SetUserData(def.userData);
		shape->SetDebugName(def.name);

		// Selectable* selectable = m_manager->Allocate<Selectable>("Selectable");
		// selectable->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		// selectable->SetAlignedSizeX(1.0f);
		// selectable->SetUserData(def.userData);
		//
		// selectable->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		// selectable->SetLocalControlsManager(this);
		// selectable->GetProps().colorOutline = Theme::GetDef().accentPrimary0;

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

			// selectable->GetProps().onSelectionChanged = [this](Selectable* s, bool selected) {
			// 	if (selected && m_props.onItemSelected)
			// 	{
			// 		m_props.onItemSelected(s->GetUserData());
			// 		m_lastSelectedItem = s->GetUserData();
			// 	}
			// };
			//
			// selectable->GetProps().onInteracted = [fold, this](Selectable* s) {
			// 	fold->SetIsUnfolded(!fold->GetIsUnfolded());
			// 	if (m_props.onItemInteracted)
			// 		m_props.onItemInteracted(s->GetUserData());
			// };

			// fold->AddChild(selectable);
			fold->AddChild(shape);

			fold->GetProps().onFoldChanged = [fold, def, this](bool unfolded) {
				Icon* icon			  = fold->GetWidgetOfType<Icon>(fold);
				icon->GetProps().icon = unfolded ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
				icon->CalculateIconSize();
				m_areItemsUnfolded[def.userData] = unfolded;
			};
		}
		else
		{
			shape->SetDebugName(def.name);
			shape->GetFlags().Set(WF_POS_ALIGN_X);
			shape->SetAlignedPosX(0.0f);

			// selectable->SetDebugName(def.name);
			// selectable->GetFlags().Set(WF_POS_ALIGN_X);
			// selectable->SetAlignedPosX(0.0f);
		}

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
		layout->GetChildMargins().left = margin;
		shape->AddChild(layout);
		// selectable->AddChild(layout);

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
				// selectable->GetProps().onClicked = [fold, chevron](Selectable* s) {
				// 	if (chevron->GetIsHovered())
				// 		fold->SetIsUnfolded(!fold->GetIsUnfolded());
				// };
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

		Widget* listItem = m_props.itemsCanHaveChildren ? static_cast<Widget*>(fold) : shape;
		m_listItems.push_back(listItem);
		m_selectionItems.push_back(shape);
		return listItem;
	}

} // namespace Lina::Editor
