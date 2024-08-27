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

#include "Editor/Widgets/Panel/PanelWidgetEditor.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/Resources/ResourceDirectory.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/LayoutBorder.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	void PanelWidgetEditor::Construct()
	{
		Widget* leftSide = m_manager->Allocate<Widget>("Left");
		leftSide->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		leftSide->SetAlignedPos(Vector2(0.0f, 0.0f));
		leftSide->SetAlignedSize(Vector2(0.25f, 1.0f));
		AddChild(leftSide);

		Widget* widgetsPanel = m_manager->Allocate<Widget>("WidgetsPanel");
		widgetsPanel->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		widgetsPanel->SetAlignedPos(Vector2(0.0f, 0.0f));
		widgetsPanel->SetAlignedSize(Vector2(1.0f, 0.5f));
		widgetsPanel->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		leftSide->AddChild(widgetsPanel);

		ItemController* itemControllerWidgets = m_manager->Allocate<ItemController>("ItemController");
		itemControllerWidgets->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemControllerWidgets->SetAlignedPos(Vector2::Zero);
		itemControllerWidgets->SetAlignedSize(Vector2::One);
		itemControllerWidgets->GetWidgetProps().drawBackground	 = true;
		itemControllerWidgets->GetWidgetProps().outlineThickness = 0.0f;
		itemControllerWidgets->GetWidgetProps().colorBackground	 = Theme::GetDef().background0;
		itemControllerWidgets->GetWidgetProps().childMargins	 = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		m_widgetsController										 = itemControllerWidgets;
		widgetsPanel->AddChild(itemControllerWidgets);

		{
			ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
			scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			scroll->SetAlignedPos(Vector2::Zero);
			scroll->SetAlignedSize(Vector2::One);
			scroll->GetProps().direction = DirectionOrientation::Vertical;
			itemControllerWidgets->AddChild(scroll);

			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			layout->SetAlignedPos(Vector2::Zero);
			layout->SetAlignedSize(Vector2::One);
			layout->GetProps().direction		  = DirectionOrientation::Vertical;
			layout->GetWidgetProps().clipChildren = true;
			scroll->AddChild(layout);
			m_widgetsLayout = layout;
		}

		DirectionalLayout* hierarchy = m_manager->Allocate<DirectionalLayout>("Hierarchy");
		hierarchy->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		hierarchy->SetAlignedPos(Vector2(0.0f, 0.5f));
		hierarchy->SetAlignedSize(Vector2(1.0f, 0.5f));
		hierarchy->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		hierarchy->GetProps().direction			 = DirectionOrientation::Vertical;
		hierarchy->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		leftSide->AddChild(hierarchy);

		Button* btnLoad = WidgetUtility::BuildIconTextButton(this, ICON_SAVE, Locale::GetStr(LocaleStr::Load));
		btnLoad->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		btnLoad->SetAlignedSizeX(0.75f);
		btnLoad->SetAlignedPosX(0.5f);
		btnLoad->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		btnLoad->SetAnchorX(Anchor::Center);
		btnLoad->GetProps().onClicked = [this]() { m_manager->AddToForeground(BuildDirectorySelector(), 0.25f); };
		hierarchy->AddChild(btnLoad);

		ItemController* itemControllerHierarchy = m_manager->Allocate<ItemController>("ItemController");
		itemControllerHierarchy->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemControllerHierarchy->SetAlignedPosX(0.0f);
		itemControllerHierarchy->SetAlignedSize(Vector2(1.0f, 0.0f));
		itemControllerHierarchy->GetWidgetProps().drawBackground   = true;
		itemControllerHierarchy->GetWidgetProps().outlineThickness = 0.0f;
		itemControllerHierarchy->GetWidgetProps().colorBackground  = Theme::GetDef().background0;
		itemControllerWidgets->GetWidgetProps().childMargins	   = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		m_hierarchyController									   = itemControllerHierarchy;
		hierarchy->AddChild(itemControllerHierarchy);

		{
			ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
			scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			scroll->SetAlignedPos(Vector2::Zero);
			scroll->SetAlignedSize(Vector2::One);
			scroll->GetProps().direction = DirectionOrientation::Vertical;
			itemControllerHierarchy->AddChild(scroll);

			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			layout->SetAlignedPos(Vector2::Zero);
			layout->SetAlignedSize(Vector2::One);
			layout->GetProps().direction		  = DirectionOrientation::Vertical;
			layout->GetWidgetProps().clipChildren = true;
			scroll->AddChild(layout);
			m_hierarchyLayout = layout;
		}

		LayoutBorder* leftBorder = m_manager->Allocate<LayoutBorder>("Border");
		leftBorder->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X);
		leftBorder->SetAlignedPos(Vector2(0.0f, 0.5f));
		leftBorder->SetAlignedSizeX(1.0f);
		leftBorder->GetProps().orientation			 = DirectionOrientation::Horizontal;
		leftBorder->GetProps().minSize				 = 0.15f;
		leftBorder->GetWidgetProps().drawBackground	 = true;
		leftBorder->GetWidgetProps().colorBackground = Theme::GetDef().background0;
		leftBorder->GetWidgetProps().colorHovered	 = Theme::GetDef().background3;
		leftSide->AddChild(leftBorder);
		leftBorder->AssignSides(widgetsPanel, hierarchy);
		m_border2 = leftBorder;

		Widget* rightSide = m_manager->Allocate<Widget>("Right");
		rightSide->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		rightSide->SetAlignedPos(Vector2(0.25f, 0.0f));
		rightSide->SetAlignedSize(Vector2(0.75f, 1.0f));
		AddChild(rightSide);

		LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
		border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		border->SetAlignedPos(Vector2(0.25f, 0.0f));
		border->SetAlignedSizeY(1.0f);
		border->GetProps().orientation			 = DirectionOrientation::Vertical;
		border->GetProps().minSize				 = 0.15f;
		border->GetWidgetProps().drawBackground	 = true;
		border->GetWidgetProps().colorBackground = Theme::GetDef().background0;
		border->GetWidgetProps().colorHovered	 = Theme::GetDef().background3;
		AddChild(border);

		border->AssignSides(leftSide, rightSide);
		m_border1 = border;
	}

	void PanelWidgetEditor::SaveLayoutToStream(OStream& stream)
	{
		m_border1->SaveToStream(stream);
		m_border2->SaveToStream(stream);
	}

	void PanelWidgetEditor::LoadLayoutFromStream(IStream& stream)
	{
		m_border1->LoadFromStream(stream);
	}

	void PanelWidgetEditor::Initialize()
	{
		Widget::Initialize();

		RefreshWidgets();
		RefreshHierarchy();
	}

	void PanelWidgetEditor::Tick(float delta)
	{
	}

	void PanelWidgetEditor::RefreshWidgets()
	{
		m_widgetsLayout->DeallocAllChildren();
		m_widgetsLayout->RemoveAllChildren();
		m_widgetsController->ClearItems();

		Vector<CategoryInfo> categories;

		const HashMap<TypeID, MetaType>& types = ReflectionSystem::Get().GetTypes();
		for (auto& [typeID, meta] : types)
		{
			if (meta.HasProperty<uint32>("WidgetIdent"_hs))
			{
				const String title	  = meta.GetProperty<String>("Title"_hs);
				const String category = meta.GetProperty<String>("Category"_hs);

				WidgetInfo widgetInfo = {
					.title = title,
					.tid   = typeID,
				};

				auto it = linatl::find_if(categories.begin(), categories.end(), [&category](const CategoryInfo& inf) -> bool { return inf.title.compare(category) == 0; });

				if (it != categories.end())
				{
					it->widgets.push_back(widgetInfo);
				}
				else
				{
					categories.push_back({});
					CategoryInfo& categoryInfo = categories.back();
					categoryInfo.title		   = category;
					categoryInfo.widgets.push_back(widgetInfo);
				}
			}
		}

		for (const CategoryInfo& category : categories)
		{
			Widget* cat = CommonWidgets::BuildDefaultFoldItem(this, nullptr, Theme::GetDef().baseIndent, "", Color::White, category.title, true, nullptr, true, true);
			m_widgetsController->AddItem(cat->GetChildren().front());
			m_widgetsLayout->AddChild(cat);

			for (const WidgetInfo& wif : category.widgets)
			{
				Widget* wi = CommonWidgets::BuildDefaultListItem(this, nullptr, Theme::GetDef().baseIndent * 2.0f, "", Color::White, wif.title, true);
				cat->AddChild(wi);
				m_widgetsController->AddItem(wi);
			}
		}
	}

	void PanelWidgetEditor::RefreshHierarchy()
	{
		m_hierarchyLayout->DeallocAllChildren();
		m_hierarchyLayout->RemoveAllChildren();
		m_hierarchyController->ClearItems();
	}

	Widget* PanelWidgetEditor::BuildDirectorySelector()
	{
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>();
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_FOREGROUND_BLOCKER);
		layout->SetAlignedPos(Vector2(0.5f, 0.5f));
		layout->SetAlignedSize(Vector2(0.4f, 0.6f));
		layout->SetAnchorX(Anchor::Center);
		layout->SetAnchorY(Anchor::Center);
		layout->GetWidgetProps().borderThickness  = TBLR::Eq(2);
		layout->GetWidgetProps().colorBorders	  = Theme::GetDef().black;
		layout->GetWidgetProps().drawBackground	  = true;
		layout->GetWidgetProps().outlineThickness = 0.0f;
		layout->GetWidgetProps().rounding		  = 0.0f;
		layout->GetWidgetProps().colorBackground  = Theme::GetDef().background1;

		WindowBar* wb = m_manager->Allocate<WindowBar>();
		wb->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		wb->SetAlignedPosX(0.0f);
		wb->SetAlignedSizeX(1.0f);
		wb->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->AddChild(wb);

		Text* txt			 = m_manager->Allocate<Text>();
		txt->GetProps().text = Locale::GetStr(LocaleStr::SelectWidget);
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		wb->AddChild(txt);

		ResourceDirectoryBrowser* bw = m_manager->Allocate<ResourceDirectoryBrowser>();
		bw->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bw->SetAlignedPosX(0.0f);
		bw->SetAlignedSize(Vector2(1.0f, 0.0f));
		bw->GetProps().itemTypeIDFilter = 0;
		layout->AddChild(bw);

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>();
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		horizontal->SetAlignedPosX(0.0f);
		horizontal->SetAlignedSizeX(1.0f);
		horizontal->SetFixedSizeY(Theme::GetDef().baseItemHeight * 1.5f);
		horizontal->GetWidgetProps().drawBackground		 = true;
		horizontal->GetWidgetProps().colorBackground	 = Theme::GetDef().background1;
		horizontal->GetWidgetProps().borderThickness.top = Theme::GetDef().baseOutlineThickness;
		horizontal->GetProps().mode						 = DirectionalLayout::Mode::EqualPositions;
		horizontal->GetWidgetProps().childPadding		 = Theme::GetDef().baseIndent;
		horizontal->GetWidgetProps().childMargins		 = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		layout->AddChild(horizontal);

		Button* select					   = m_manager->Allocate<Button>();
		select->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Select);
		select->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		select->SetAlignedSizeX(0.3f);
		select->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		select->SetAlignedPosY(0.5f);
		select->SetAnchorY(Anchor::Center);
		select->SetTickHook([bw, select](float delta) {
			Vector<ResourceDirectory*> selection = bw->GetItemController()->GetSelectedUserData<ResourceDirectory>();
			if (selection.size() == 1 && selection.front()->resourceTID == GetTypeID<GUIWidget>())
				select->SetIsDisabled(false);
			else
				select->SetIsDisabled(true);
		});

		select->GetProps().onClicked = [this, layout, bw]() {
			m_manager->SetForegroundDim(0.0f);
			m_manager->AddToKillList(layout);
			ResourceDirectory* selection = bw->GetItemController()->GetSelectedUserData<ResourceDirectory>().front();
			// do whatever
		};

		horizontal->AddChild(select);

		Button* cancel					   = m_manager->Allocate<Button>();
		cancel->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Cancel);
		cancel->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_Y_TOTAL_CHILDREN | WF_SIZE_ALIGN_X);
		cancel->SetAlignedSizeX(0.3f);
		cancel->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		cancel->SetAlignedPosY(0.5f);
		cancel->SetAnchorY(Anchor::Center);

		cancel->GetProps().onClicked = [this, layout]() {
			m_manager->SetForegroundDim(0.0f);
			m_manager->AddToKillList(layout);
		};
		horizontal->AddChild(cancel);

		layout->Initialize();
		return layout;
	}
} // namespace Lina::Editor
