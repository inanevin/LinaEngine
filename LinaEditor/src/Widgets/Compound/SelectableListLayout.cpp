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

#include "Editor/Widgets/Compound/SelectableListLayout.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Selectable.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Effects/Dropshadow.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void SelectableListLayout::Construct()
	{
		FileMenu* fm = m_manager->Allocate<FileMenu>("FileMenu");
		AddChild(fm);
		m_contextMenu = fm;
	}

	void SelectableListLayout::Initialize()
	{
		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(scroll);

		if (!m_props.useGridAsLayout)
		{
			DirectionalLayout* dirLayout	= m_manager->Allocate<DirectionalLayout>("Layout");
			dirLayout->GetProps().direction = DirectionOrientation::Vertical;
			dirLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dirLayout->SetAlignedPos(Vector2::Zero);
			dirLayout->SetAlignedSize(Vector2::One);
			dirLayout->GetProps().backgroundStyle	   = DirectionalLayout::BackgroundStyle::Default;
			dirLayout->GetProps().colorBackgroundStart = dirLayout->GetProps().colorBackgroundEnd = Theme::GetDef().background1;
			dirLayout->GetProps().colorOutline													  = Theme::GetDef().accentPrimary0;
			dirLayout->GetProps().clipChildren													  = true;
			scroll->AddChild(dirLayout);
			m_dirLayout = dirLayout;
			m_layout	= dirLayout;
		}
		else
		{
			GridLayout* gridLayout = m_manager->Allocate<GridLayout>("GridLayout");
			gridLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			gridLayout->SetAlignedPos(Vector2::Zero);
			gridLayout->SetAlignedSize(Vector2::One);
			gridLayout->GetProps().colorOutline	   = Theme::GetDef().accentPrimary0;
			gridLayout->GetChildMargins()		   = TBLR::Eq(Theme::GetDef().baseIndent);
			gridLayout->GetProps().clipChildren	   = true;
			gridLayout->GetProps().background	   = GridLayout::BackgroundStyle::Default;
			gridLayout->GetProps().colorBackground = Theme::GetDef().background1;
			gridLayout->GetProps().verticalPadding = Theme::GetDef().baseIndent * 2;
			gridLayout->SetChildPadding(Theme::GetDef().baseIndent * 2);
			gridLayout->GetProps().colorOutline = Theme::GetDef().accentPrimary0;
			scroll->AddChild(gridLayout);
			m_gridLayout = gridLayout;
			m_layout	 = gridLayout;
		}

		Editor::Get()->GetWindowPanelManager().AddPayloadListener(this);
	}

	void SelectableListLayout::Destruct()
	{
		Editor::Get()->GetWindowPanelManager().RemovePayloadListener(this);
	}

	void SelectableListLayout::Tick(float delta)
	{
		if (m_gridLayout)
			m_gridLayout->GetProps().outlineThickness = m_payloadActive ? Theme::GetDef().baseOutlineThickness : 0.0f;
		else
			m_dirLayout->GetProps().outlineThickness = m_payloadActive ? Theme::GetDef().baseOutlineThickness : 0.0f;

		if (!m_payloadActive)
			return;

		for (auto* s : m_selectables)
			s->GetProps().colorStart = s->GetProps().colorEnd = s->GetIsHovered() ? Theme::GetDef().accentPrimary0 : Color(0, 0, 0, 0);
	}
	void SelectableListLayout::RefreshItems()
	{
		if (!m_listener)
			return;

		m_gridLayoutItems.clear();
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();
		m_selectables.clear();

		Vector<SelectableListItem> items;
		m_listener->OnSelectableListFillItems(this, items, nullptr);

		for (const auto& it : items)
		{
			if (m_gridLayout)
				m_layout->AddChild(CreateGridItem(it, 0));
			else
				m_layout->AddChild(CreateItem(it, 0));
		}

		if (items.empty())
		{
			Text* nothingToSee = m_manager->Allocate<Text>("NothingToSee");
			nothingToSee->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			nothingToSee->SetAlignedPos(Vector2::One * 0.5f);
			nothingToSee->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			nothingToSee->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			nothingToSee->GetProps().color = Theme::GetDef().foreground1;
			nothingToSee->GetProps().text  = Locale::GetStrUnicode(LocaleStr::NothingInDirectory);
			nothingToSee->Initialize();
			m_layout->AddChild(nothingToSee);
		}

		m_layout->Initialize();
	}

	Widget* SelectableListLayout::CreateGridItem(const SelectableListItem& item, uint8 level)
	{
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		layout->SetFixedSize(m_props.defaultGridSize);
		m_gridLayoutItems.push_back(layout);

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

		ShapeRect* bgShapeOutline = m_manager->Allocate<ShapeRect>("Outline");
		bgShapeOutline->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		bgShapeOutline->SetAlignedPos(Vector2::Zero);
		bgShapeOutline->SetAlignedSize(Vector2::One);
		bgShapeOutline->GetProps().rounding			= Theme::GetDef().baseRounding;
		bgShapeOutline->GetProps().colorStart.w		= 0.0f;
		bgShapeOutline->GetProps().colorEnd.w		= 0.0f;
		bgShapeOutline->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness * 1.5f;
		bgShapeOutline->GetProps().colorOutline		= Theme::GetDef().background0;
		bgShapeOutline->GetProps().onlyRoundCorners.push_back(0);
		bgShapeOutline->GetProps().onlyRoundCorners.push_back(1);
		bgShape->AddChild(bgShapeOutline);

		if (item.useFolderIcon)
		{
			Icon* folder			= m_manager->Allocate<Icon>("Folder");
			folder->GetProps().icon = ICON_FOLDER;
			folder->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			folder->SetAlignedPos(Vector2(0.5f, 0.5f));
			folder->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			folder->GetProps().dynamicSizeToParent = true;
			folder->GetProps().dynamicSizeScale	   = 0.8f;
			bgShape->AddChild(folder);
		}
		else if (item.useCustomTexture)
		{
			// ShapeRect* img = m_manager->Allocate<ShapeRect>("Shape");
			// img->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			// img->GetProps().onlyRoundCorners.push_back(0);
			// img->GetProps().onlyRoundCorners.push_back(1);
			// img->SetAlignedPos(Vector2(0.5f, 0.5f));
			// img->SetAlignedSize(Vector2(1.0f, 1.0f));
			// img->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			// img->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			bgShape->GetProps().imageTextureAtlas = item.customTexture;
			bgShape->GetProps().fitImage		  = true;

			if (item.customTexture == nullptr)
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

			// bgShape->GetProps().rounding = Theme::GetDef().baseRounding;
			// bgShape->AddChild(img);

			// img->SetCustomTooltipUserData(item.userData);
			// img->SetBuildCustomTooltip([this](void* userData) -> Widget* { return m_listener->OnSelectableListBuildCustomTooltip(this, userData); });
		}

		Selectable* selectable = m_manager->Allocate<Selectable>("Selectable");
		selectable->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_ALIGN_Y);
		selectable->SetAlignedPosX(0.0f);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetAlignedSizeY(0.0f);
		selectable->SetUserData(item.userData);
		selectable->GetProps().rounding	  = Theme::GetDef().baseRounding;
		selectable->GetProps().colorStart = Theme::GetDef().background3;
		selectable->GetProps().colorEnd	  = Theme::GetDef().background3;
		selectable->SetDrawOrderIncrement(2);
		selectable->SetUserData(item.userData);
		layout->AddChild(selectable);
		bgShape->GetProps().onClicked = [selectable]() { selectable->Select(); };

		SetSelectableCallbacks(item, selectable, nullptr);
		m_selectables.push_back(selectable);

		Dropshadow* ds = m_manager->Allocate<Dropshadow>("Dropshadow");
		ds->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		ds->SetAlignedPos(Vector2::Zero);
		ds->SetAlignedSize(Vector2::One);
		ds->GetProps().direction = Direction::Bottom;
		ds->GetProps().color	 = Theme::GetDef().background0;
		ds->GetProps().color.w	 = 0.75f;
		ds->GetProps().steps	 = 6;
		ds->SetDrawOrderIncrement(-1);
		selectable->AddChild(ds);

		Text* title = WidgetUtility::BuildEditableText(this, false, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		title->SetAlignedPos(Vector2(0.5f, 0.5f));
		title->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text				  = item.title;
		title->GetProps().fetchWrapFromParent = true;
		title->GetProps().wordWrap			  = false;
		selectable->AddChild(title);

		return layout;
	}

	Widget* SelectableListLayout::CreateItem(const SelectableListItem& item, uint8 level)
	{
		FoldLayout* fold = m_manager->Allocate<FoldLayout>("Fold");
		fold->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		fold->SetAlignedPosX(0.0f);
		fold->SetAlignedSizeX(1.0f);
		fold->SetUserData(item.userData);

		Selectable* selectable = m_manager->Allocate<Selectable>("Selectable");
		selectable->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		selectable->SetAlignedPosX(0.0f);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		selectable->SetLocalControlsManager(m_layout);
		selectable->SetUserData(item.userData);
		selectable->GetProps().colorOutline = Theme::GetDef().accentPrimary0;
		m_selectables.push_back(selectable);
		fold->AddChild(selectable);

		SetSelectableCallbacks(item, selectable, fold);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
		layout->GetChildMargins().left = Theme::GetDef().baseIndent + Theme::GetDef().baseIndent * level;
		selectable->AddChild(layout);

		Icon* dropdown = nullptr;

		if (item.useDropdownIcon)
		{
			dropdown					   = m_manager->Allocate<Icon>("Dropdown");
			dropdown->GetProps().icon	   = !fold->GetIsUnfolded() ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
			dropdown->GetProps().textScale = 0.3f;
			dropdown->GetFlags().Set(WF_POS_ALIGN_Y);
			dropdown->SetAlignedPosY(0.5f);
			dropdown->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			dropdown->GetProps().onClicked = [fold]() { fold->SetIsUnfolded(!fold->GetIsUnfolded()); };
			dropdown->SetVisible(item.hasChildren);
			layout->AddChild(dropdown);
		}

		if (item.useFolderIcon)
		{
			Icon* folder			= m_manager->Allocate<Icon>("Folder");
			folder->GetProps().icon = ICON_FOLDER;
			folder->GetFlags().Set(WF_POS_ALIGN_Y);
			folder->SetAlignedPosY(0.5f);
			folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(folder);
		}
		else if (item.useCustomTexture)
		{
			ShapeRect* img = m_manager->Allocate<ShapeRect>("Shape");
			img->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			img->SetAlignedPosY(0.5f);
			img->SetAlignedSizeY(0.8f);
			img->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			img->GetProps().imageTextureAtlas = item.customTexture;
			img->GetProps().fitImage		  = true;
			img->GetProps().colorStart		  = Color(1, 1, 1, 1);
			img->GetProps().colorEnd		  = Color(1, 1, 1, 1);
			layout->AddChild(img);

			if (item.customTexture == nullptr)
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

			img->SetCustomTooltipUserData(item.userData);
			img->SetBuildCustomTooltip([this](void* userData) -> Widget* { return m_listener->OnSelectableListBuildCustomTooltip(this, userData); });
		}

		Text* title = WidgetUtility::BuildEditableText(this, true, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text = item.title;
		layout->AddChild(title);

		fold->GetProps().onFoldChanged = [dropdown, fold, item, level, this](bool unfolded) {
			if (dropdown)
			{
				dropdown->GetProps().icon = !unfolded ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
				dropdown->CalculateIconSize();
			}

			m_foldStatus[item.userData] = unfolded;

			if (!unfolded)
			{
				auto* first = fold->GetChildren().front();

				for (auto* c : fold->GetChildren())
				{
					if (c == first)
						continue;
					m_manager->Deallocate(c);
				}

				fold->RemoveAllChildren();
				fold->AddChild(first);
			}
			else
			{
				Vector<SelectableListItem> subItems;
				m_listener->OnSelectableListFillItems(this, subItems, item.userData);

				for (const auto& subItem : subItems)
				{
					auto* child = CreateItem(subItem, level + 1);
					fold->AddChild(child);
					child->Initialize();
				}
			}
		};

		fold->SetIsUnfolded(m_foldStatus[item.userData]);

		return fold;
	}

	void SelectableListLayout::SetSelectableCallbacks(const SelectableListItem& item, Selectable* selectable, FoldLayout* fold)
	{
		selectable->GetProps().onRightClick = [this, item](Selectable* s) { m_contextMenu->CreateItems(0, m_lgxWindow->GetMousePosition()); };

		selectable->GetProps().onInteracted = [fold, item, this](Selectable*) {
			if (item.useCustomInteraction)
				m_listener->OnSelectableListItemInteracted(this, item.userData);
			else if (fold)
				fold->SetIsUnfolded(!fold->GetIsUnfolded());
		};

		selectable->SetOnGrabbedControls([this, item]() { m_listener->OnSelectableListItemControl(this, item.userData); });

		selectable->GetProps().onDockedOut = [this, selectable, item]() {
			DirectionalLayout* payload		= m_manager->Allocate<DirectionalLayout>();
			payload->GetChildMargins().left = Theme::GetDef().baseIndent;
			payload->SetUserData(item.userData);
			Text* text = m_manager->Allocate<Text>();
			text->GetFlags().Set(WF_POS_ALIGN_Y);
			text->SetAlignedPosY(0.5f);
			text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			text->GetProps().text = item.title;
			payload->AddChild(text);
			payload->Initialize();
			Editor::Get()->GetWindowPanelManager().CreatePayload(payload, m_listener->OnSelectableListGetPayloadType(this), Vector2ui(static_cast<uint32>(text->GetSizeX() + Theme::GetDef().baseIndent * 2), static_cast<uint32>(Theme::GetDef().baseItemHeight)));
		};
	}

	void SelectableListLayout::SetGridLayoutItemSize(const Vector2& size)
	{
		m_props.defaultGridSize = size;
		for (auto* i : m_gridLayoutItems)
			i->SetFixedSize(size);
	}

	void SelectableListLayout::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (type != m_listener->OnSelectableListGetPayloadType(this))
			return;
		m_payloadActive = true;
	}

	void SelectableListLayout::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		if (type != m_listener->OnSelectableListGetPayloadType(this))
			return;
		m_payloadActive = false;

		for (auto* s : m_selectables)
			s->GetProps().colorStart = s->GetProps().colorEnd = Color(0, 0, 0, 0);
	}

	bool SelectableListLayout::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (type != m_listener->OnSelectableListGetPayloadType(this) || !m_isHovered)
			return false;

		for (auto* s : m_selectables)
		{
			if (s->GetIsHovered())
			{
				m_listener->OnSelectableListPayloadDropped(this, payload->GetUserData(), s->GetUserData());
				m_manager->AddToKillList(payload);
				return true;
			}
		}

		m_manager->AddToKillList(payload);
		m_listener->OnSelectableListPayloadDropped(this, payload->GetUserData(), nullptr);
		return true;
	}

	bool SelectableListLayout::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_selectables.empty())
			return false;

		if (m_isHovered)
		{
			bool anyHovered = false;

			for (auto* c : GetChildren())
			{
				if (c->GetIsHovered())
				{
					anyHovered = true;
					break;
				}
			}

			for (auto* c : m_layout->GetChildren())
			{
				if (c->GetIsHovered())
				{
					anyHovered = true;
					break;
				}
			}

			if (!anyHovered)
			{
				m_listener->OnSelectableListItemControl(this, nullptr);
				return true;
			}
		}

		return false;
	}

	void SelectableListLayout::ChangeFold(void* userData, bool isUnfolded)
	{
		for (auto* s : m_selectables)
		{
			if (s->GetUserData() == userData)
			{
				static_cast<FoldLayout*>(s->GetParent())->SetIsUnfolded(isUnfolded);
				break;
			}
		}
	}

	void SelectableListLayout::GrabControls(void* userData)
	{
		for (auto* s : m_selectables)
		{
			if (s->GetUserData() == userData)
			{
				m_manager->GrabControls(s);
				break;
			}
		}
	}
} // namespace Lina::Editor
