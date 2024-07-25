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
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
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

		FileMenu* fm = m_manager->Allocate<FileMenu>("FileMenu");
		AddChild(fm);
		m_contextMenu = fm;
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
			if (m_isFocused && !m_isHovered)
				SetFocus(false);
		}
		else
			m_isPressed = false;

		if (m_isPressed && m_dragTarget && !m_dragTarget->GetIsHovered())
		{
			if (m_props.onCreatePayload)
				m_props.onCreatePayload(m_dragTarget->GetUserData());

			m_dragTarget = nullptr;
		}

		if (m_isPressed && !m_isFocused)
			SetFocus(true);

		if (!m_isPressed)
			m_dragTarget = nullptr;

		const Color dead = m_useGridLayout ? Theme::GetDef().background3 : Color(0.0f, 0.0f, 0.0f, 0.0f);

		for (Widget* item : m_selectionItems)
		{
			const bool selected = IsItemSelected(item);

			if (selected)
			{
				item->GetWidgetProps().colorBackground.start = m_isFocused ? Theme::GetDef().accentPrimary0 : Theme::GetDef().silent0;
				item->GetWidgetProps().colorBackground.end	 = m_isFocused ? Theme::GetDef().accentPrimary1 : Theme::GetDef().silent1;
			}
			else
			{
				item->GetWidgetProps().colorBackground = dead;
			}
		}
	}

	void ItemLayout::Draw()
	{
		Widget::Draw();
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
			Widget* item = nullptr;

			if (isForward)
			{
				for (Widget* r : m_selectionItems)
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
				for (Vector<Widget*>::iterator it = m_selectionItems.end() - 1; it >= m_selectionItems.begin(); it--)
				{
					Widget* r = *it;
					if (r->GetIsVisible() && !r->GetIsDisabled())
					{
						item = r;
						break;
					}
				}
			}

			if (item)
			{
				SelectItem(item, true);
				m_isFocused = true;
			}
		}
	}

	Widget* ItemLayout::GetNextControls()
	{
		if (m_selectedItems.empty())
		{
			if (m_selectionItems.empty())
				return nullptr;
			SelectItem(m_selectionItems[0], true);
			return this;
		}

		Widget* currentSelected = m_selectedItems.back();

		const int32 sz	  = static_cast<int32>(m_selectionItems.size());
		bool		found = false;

		for (int32 i = 0; i < sz; i++)
		{
			Widget* item = m_selectionItems[i];

			if (item == currentSelected)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (item->GetIsVisible() && !item->GetIsDisabled())
				{
					SelectItem(item, true);
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
			Widget* item = m_selectionItems[i];

			if (item == currentSelected)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (item->GetIsVisible() && !item->GetIsDisabled())
				{
					SelectItem(item, true);
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

		Widget* item = m_selectedItems.back();

		if (key == LINAGX_KEY_D && act == LinaGX::InputAction::Pressed)
		{
			if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
			{
				if (m_props.onDuplicate)
					m_props.onDuplicate(item->GetUserData());
				return true;
			}
		}

		if (key == LINAGX_KEY_DELETE && act == LinaGX::InputAction::Pressed)
		{
			if (m_props.onDelete)
				m_props.onDelete(item->GetUserData());
			return true;
		}
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
				GetPrevControls();
				m_isFocused = true;
				return true;
			}
			else if (key == LINAGX_KEY_DOWN)
			{
				GetNextControls();
				m_isFocused = true;
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
				GetNextControls();
				m_isFocused = true;
				return true;
			}
			else if (key == LINAGX_KEY_LEFT)
			{
				GetPrevControls();
				m_isFocused = true;
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

		if (m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			if (button == LINAGX_MOUSE_0)
				m_isPressed = true;

			if (m_useGridLayout)
			{
				for (Widget* it : m_gridItems)
				{
					if (it->GetIsHovered())
					{
						m_dragTarget = it;
						break;
					}
				}
			}
			else
			{
				for (Widget* it : m_listItems)
				{
					if (it->GetIsHovered())
					{
						m_dragTarget = it;
						break;
					}
				}
			}

			if (!m_dragTarget && button == LINAGX_MOUSE_1)
			{
				SetFocus(true);
				m_contextMenu->CreateItems(0, m_lgxWindow->GetMousePosition(), nullptr);
				return true;
			}

			return false;
		}

		return false;
	}

	bool ItemLayout::IsItemSelected(Widget* rect)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [rect](Widget* r) -> bool { return rect == r; });
		return it != m_selectedItems.end();
	}

	void ItemLayout::RemoveItemFromSelected(Widget* rect)
	{
		auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [rect](Widget* r) -> bool { return rect == r; });
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

	void ItemLayout::SelectItem(Widget* r, bool clearSelected, bool callEvent)
	{
		UnfoldRecursively(r->GetParent());
		m_scroll->ScrollToChild(r);

		if (clearSelected)
			m_selectedItems.clear();

		m_manager->GrabControls(this);

		m_selectedItems.push_back(r);

		if (callEvent && m_props.onItemSelected)
			m_props.onItemSelected(r->GetUserData());
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
			grid->GetWidgetProps().clipChildren = true;
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
			layout->GetWidgetProps().clipChildren = true;
			m_layout							  = layout;
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

	void ItemLayout::SetOutlineThicknessAndColor(float outlineThickness, const Color& outlineColor)
	{
		if (m_useGridLayout)
		{
			GridLayout* grid						= static_cast<GridLayout*>(m_layout);
			grid->GetWidgetProps().outlineThickness = outlineThickness;
			grid->GetWidgetProps().colorOutline		= outlineColor;
		}
		else
		{
			DirectionalLayout* layout				  = static_cast<DirectionalLayout*>(m_layout);
			layout->GetWidgetProps().outlineThickness = outlineThickness;
			layout->GetWidgetProps().colorOutline	  = outlineColor;
		}
	}

	void ItemLayout::SetSelectedItem(void* userData)
	{
		for (Widget* it : m_selectionItems)
		{
			if (it->GetUserData() == userData)
			{
				m_isFocused = true;
				SelectItem(it, true, false);
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

	void ItemLayout::SetFocus(bool isFocused)
	{
		m_isFocused = isFocused;
		if (m_isFocused)
			m_manager->GrabControls(this);
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
		layout->SetUserData(def.userData);
		layout->GetProps().receiveInput = true;

		m_gridItems.push_back(layout);

		Widget* bgShape = m_manager->Allocate<Widget>("BG");
		bgShape->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_Y_COPY_X);
		bgShape->SetAlignedPosX(0.0f);
		bgShape->SetAlignedSizeX(1.0f);
		bgShape->GetWidgetProps().drawBackground  = true;
		bgShape->GetWidgetProps().rounding		  = Theme::GetDef().baseRounding;
		bgShape->GetWidgetProps().colorBackground = Theme::GetDef().background2;
		bgShape->GetWidgetProps().onlyRound.push_back(0);
		bgShape->GetWidgetProps().onlyRound.push_back(1);
		bgShape->SetUserData(def.userData);
		layout->AddChild(bgShape);

		if (def.useOutlineInGrid)
		{
			Widget* bgShapeOutline = m_manager->Allocate<Widget>("Outline");
			bgShapeOutline->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			bgShapeOutline->SetAlignedPos(Vector2::Zero);
			bgShapeOutline->SetAlignedSize(Vector2::One);
			bgShapeOutline->GetWidgetProps().drawBackground			 = true;
			bgShapeOutline->GetWidgetProps().rounding				 = Theme::GetDef().baseRounding;
			bgShapeOutline->GetWidgetProps().colorBackground.start.w = 0.0f;
			bgShapeOutline->GetWidgetProps().colorBackground.end.w	 = 0.0f;
			bgShapeOutline->GetWidgetProps().outlineThickness		 = Theme::GetDef().baseOutlineThickness * 1.5f;
			bgShapeOutline->GetWidgetProps().colorOutline			 = Theme::GetDef().black;
			bgShapeOutline->GetWidgetProps().onlyRound.push_back(0);
			bgShapeOutline->GetWidgetProps().onlyRound.push_back(1);
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
			bgShape->GetWidgetProps().colorBackground.start.w = bgShape->GetWidgetProps().colorBackground.end.w = 0.0f;

			bgShape->AddChild(icon);
		}
		else
		{
			bgShape->GetWidgetProps().textureAtlas = def.texture;
			bgShape->GetWidgetProps().fitTexture   = true;

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
				bgShape->GetWidgetProps().colorBackground = Color(1, 1, 1, 1);
			}
		}

		Widget* shape = m_manager->Allocate<Widget>("Shape");
		shape->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_ALIGN_Y);
		shape->SetAlignedPosX(0.0f);
		shape->SetAlignedSizeX(1.0f);
		shape->SetAlignedSizeY(0.0f);
		shape->GetWidgetProps().rounding			  = Theme::GetDef().baseRounding;
		shape->GetWidgetProps().interpolateColor	  = true;
		shape->GetWidgetProps().colorInterpolateSpeed = 20.0f;
		shape->SetDrawOrderIncrement(2);
		shape->SetLocalControlsManager(this);
		shape->SetUserData(def.userData);
		shape->GetWidgetProps().colorBackground = Theme::GetDef().background3;
		shape->Initialize();
		shape->SetDebugName(def.name);

		layout->AddChild(shape);
		m_selectionItems.push_back(shape);

		layout->GetProps().onRightClicked = [this, shape, layout]() {
			SetFocus(true);

			auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [shape](Widget* sr) -> bool { return sr == shape; });
			if (it == m_selectedItems.end())
				SelectItem(shape, true);
			m_contextMenu->CreateItems(0, m_lgxWindow->GetMousePosition(), layout->GetUserData());
		};

		layout->GetProps().onDoubleClicked = [this, shape]() {
			SetFocus(true);
			if (m_props.onItemInteracted)
				m_props.onItemInteracted(shape->GetUserData());
		};

		Text* title = m_manager->Allocate<Text>("Title");
		title->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		title->SetAlignedPos(Vector2(0.5f, 0.5f));
		title->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text				  = def.name;
		title->GetProps().fetchWrapFromParent = true;
		title->GetProps().wordWrap			  = false;
		title->SetUserData(def.userData);
		shape->AddChild(title);

		layout->GetProps().onPressed = [this, shape, layout, title, def]() {
			SetFocus(true);

			const bool wasSelected = IsItemSelected(shape);

			if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
			{
				if (IsItemSelected(shape))
					RemoveItemFromSelected(shape);
				else
					SelectItem(shape, false);
			}
			else
			{
				SelectItem(shape, true);

				if (def.nameEditable && wasSelected && title->GetIsHovered())
					RenameTitle(title);
			}
		};

		return layout;
	}

	Widget* ItemLayout::CreateListItem(const ItemDefinition& def, float margin, bool hasParent)
	{
		FoldLayout* fold = nullptr;

		if (def.unfoldOverride)
			m_areItemsUnfolded[def.userData] = true;

		Widget* shape = m_manager->Allocate<Widget>("Shape");
		shape->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		shape->SetAlignedSizeX(1.0f);
		shape->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		shape->GetWidgetProps().interpolateColor	  = true;
		shape->GetWidgetProps().colorInterpolateSpeed = 20.0f;
		shape->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		shape->SetUserData(def.userData);
		shape->SetDebugName(def.name);

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
		}

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
		layout->GetChildMargins().left	= margin;
		layout->GetProps().receiveInput = true;

		shape->AddChild(layout);

		Icon* chevron = nullptr;
		if (m_props.itemsCanHaveChildren)
		{
			chevron						   = m_manager->Allocate<Icon>("Folder");
			chevron->GetProps().icon	   = m_areItemsUnfolded[def.userData] ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
			chevron->GetProps().colorStart = chevron->GetProps().colorEnd = def.color;
			chevron->GetFlags().Set(WF_POS_ALIGN_Y);
			chevron->SetAlignedPosY(0.5f);
			chevron->GetProps().textScale = 0.4f;
			chevron->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(chevron);

			if (def.children.empty())
				chevron->SetVisible(false);
		}

		layout->GetProps().onDoubleClicked = [shape, this, chevron, fold]() {
			if (chevron)
			{
				fold->SetIsUnfolded(!fold->GetIsUnfolded());
			}
			else
			{
				if (m_props.onItemInteracted)
					m_props.onItemInteracted(shape->GetUserData());
			}

			SetFocus(true);
			SelectItem(shape, true);
		};
		layout->GetProps().onRightClicked = [shape, this]() {
			SetFocus(true);

			auto it = linatl::find_if(m_selectedItems.begin(), m_selectedItems.end(), [shape](Widget* sr) -> bool { return sr == shape; });
			if (it == m_selectedItems.end())
				SelectItem(shape, true);
			m_contextMenu->CreateItems(0, m_lgxWindow->GetMousePosition(), shape->GetUserData());
		};

		if (!def.icon.empty())
		{
			Icon* icon					= m_manager->Allocate<Icon>("Folder");
			icon->GetProps().icon		= def.icon;
			icon->GetProps().colorStart = icon->GetProps().colorEnd = def.color;
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(icon);
		}
		else
		{
			Widget* img = m_manager->Allocate<Widget>("Shape");
			img->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			img->SetAlignedPosY(0.5f);
			img->SetAlignedSizeY(0.8f);
			img->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			img->GetWidgetProps().textureAtlas	  = def.texture;
			img->GetWidgetProps().fitTexture	  = true;
			img->GetWidgetProps().colorBackground = Color(1, 1, 1, 1);
			layout->AddChild(img);

			if (def.texture == nullptr)
			{
				LinaLoading* loading = m_manager->Allocate<LinaLoading>();
				loading->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				loading->SetAlignedPos(Vector2(0.5f, 0.5f));
				loading->SetAlignedSize(Vector2(1.0f, 1.0f));
				loading->SetPosAlignmentSourceX(PosAlignmentSource::Center);
				loading->SetPosAlignmentSourceY(PosAlignmentSource::Center);
				img->GetWidgetProps().colorBackground = Color(0, 0, 0, 0);
				img->AddChild(loading);
			}
		}

		Text* title = WidgetUtility::BuildEditableText(this, true, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text = def.name;
		title->SetUserData(def.userData);
		layout->AddChild(title);

		layout->GetProps().onPressed = [shape, this, chevron, fold, title, def]() {
			if (chevron && chevron->GetIsHovered())
				fold->SetIsUnfolded(!fold->GetIsUnfolded());

			SetFocus(true);

			const bool wasSelected = IsItemSelected(shape);

			if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
			{
				if (IsItemSelected(shape))
					RemoveItemFromSelected(shape);
				else
					SelectItem(shape, false);
			}
			else
			{
				SelectItem(shape, true);

				if (def.nameEditable && wasSelected && title->GetIsHovered())
					RenameTitle(title);
			}
		};

		Widget* listItem = m_props.itemsCanHaveChildren ? static_cast<Widget*>(fold) : shape;
		m_listItems.push_back(layout);
		shape->Initialize();
		m_selectionItems.push_back(shape);
		return listItem;
	}

	void ItemLayout::RenameTitle(Text* txt)
	{
		InputField* inp = m_manager->Allocate<InputField>();
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = txt->GetProps().text;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(txt->GetSizeX() * 2);
		inp->Initialize();

		if (!m_useGridLayout)
		{
			inp->SetPosX(txt->GetPosX());
			inp->SetPosY(txt->GetParent()->GetPosY());
		}
		else
		{
			inp->SetPosX(txt->GetParent()->GetRect().GetCenter().x - inp->GetHalfSizeX());
			inp->SetPosY(txt->GetPosY());
		}

		inp->GetProps().onEditEnd = [inp, txt, this](const String& str) {
			if (str.empty())
			{
				inp->GetText()->GetProps().text = txt->GetProps().text;
				inp->GetText()->CalculateTextSize();
				return;
			}
			txt->GetProps().text = str;
			txt->CalculateTextSize();
			m_manager->AddToKillList(inp);
			if (m_props.onItemRenamed)
				m_props.onItemRenamed(txt->GetUserData(), str);
		};

		m_manager->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();
		m_manager->GrabControls(inp);
	}

} // namespace Lina::Editor
