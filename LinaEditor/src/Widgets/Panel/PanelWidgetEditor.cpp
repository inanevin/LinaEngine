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
#include "Editor/Widgets/Panel/PanelWidgetEditorProperties.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Editor.hpp"
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
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	void PanelWidgetEditor::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		horizontal->SetAlignedPos(Vector2::Zero);
		horizontal->SetAlignedSize(Vector2::One);
		horizontal->GetProps().direction = DirectionOrientation::Horizontal;
		horizontal->GetProps().mode		 = DirectionalLayout::Mode::Bordered;
		AddChild(horizontal);

		DirectionalLayout* leftSide = m_manager->Allocate<DirectionalLayout>("Left");
		leftSide->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		leftSide->SetAlignedPos(Vector2(0.0f, 0.0f));
		leftSide->SetAlignedSize(Vector2(0.25f, 1.0f));
		leftSide->GetProps().mode	   = DirectionalLayout::Mode::Bordered;
		leftSide->GetProps().direction = DirectionOrientation::Vertical;
		horizontal->AddChild(leftSide);

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
		itemControllerWidgets->GetWidgetProps().drawBackground		 = true;
		itemControllerWidgets->GetWidgetProps().dropshadow.enabled	 = true;
		itemControllerWidgets->GetWidgetProps().dropshadow.isInner	 = true;
		itemControllerWidgets->GetWidgetProps().dropshadow.direction = Direction::Top;
		itemControllerWidgets->GetWidgetProps().dropshadow.color	 = Theme::GetDef().black;
		itemControllerWidgets->GetWidgetProps().dropshadow.color.w	 = 0.25f;
		itemControllerWidgets->GetWidgetProps().dropshadow.steps	 = 8;
		itemControllerWidgets->GetWidgetProps().outlineThickness	 = 0.0f;
		itemControllerWidgets->GetWidgetProps().colorBackground		 = Theme::GetDef().background0;
		itemControllerWidgets->GetWidgetProps().childMargins		 = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};

		itemControllerWidgets->GetProps().onCreatePayload = [this](void* userdata) {
			Widget*		root   = m_editor->GetWindowPanelManager().GetPayloadRoot();
			WidgetInfo* inf	   = static_cast<WidgetInfo*>(userdata);
			Text*		t	   = root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text = inf->title;
			t->Initialize();
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::WidgetEditorWidget, t->GetSize());
		};

		itemControllerWidgets->GetProps().onCheckCanCreatePayload = [](void* userdata) -> bool { return userdata != nullptr; };
		m_widgetsController										  = itemControllerWidgets;
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
		itemControllerHierarchy->GetWidgetProps().drawBackground	   = true;
		itemControllerHierarchy->GetWidgetProps().dropshadow.enabled   = true;
		itemControllerHierarchy->GetWidgetProps().dropshadow.isInner   = true;
		itemControllerHierarchy->GetWidgetProps().dropshadow.color	   = Theme::GetDef().black;
		itemControllerHierarchy->GetWidgetProps().dropshadow.color.w   = 0.25f;
		itemControllerHierarchy->GetWidgetProps().dropshadow.steps	   = 8;
		itemControllerHierarchy->GetWidgetProps().dropshadow.direction = Direction::Top;
		itemControllerHierarchy->GetWidgetProps().outlineThickness	   = 0.0f;
		itemControllerHierarchy->GetWidgetProps().colorBackground	   = Theme::GetDef().background0;
		itemControllerHierarchy->GetWidgetProps().childMargins		   = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		itemControllerHierarchy->GetContextMenu()->SetListener(this);
		itemControllerHierarchy->GetProps().payloadType = PayloadType::WidgetEditorWidget;

		itemControllerHierarchy->GetProps().onPayloadAccepted = [](void* userdata) {

		};
		m_hierarchyController = itemControllerHierarchy;
		hierarchy->AddChild(itemControllerHierarchy);

		itemControllerHierarchy->GetProps().onDelete = [itemControllerHierarchy, this]() {
			Vector<Widget*> selection = itemControllerHierarchy->GetSelectedUserData<Widget>();
			RequestDelete(selection);
		};

		itemControllerHierarchy->GetProps().onDuplicate = [itemControllerHierarchy, this]() {
			Vector<Widget*> selection = itemControllerHierarchy->GetSelectedUserData<Widget>();
			RequestDuplicate(selection);
		};

		itemControllerHierarchy->GetProps().onItemRenamed = [itemControllerHierarchy, this](void* item) { RequestRename(static_cast<Widget*>(item)); };

		itemControllerHierarchy->GetProps().onItemSelected = [this](void* userdata) {
			Widget* w = static_cast<Widget*>(userdata);
			m_propertiesArea->Refresh(w);
		};

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

		Widget* gridArea = m_manager->Allocate<Widget>("Grid");
		gridArea->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		gridArea->SetAlignedPos(Vector2(0.25f, 0.0f));
		gridArea->SetAlignedSize(Vector2(0.5f, 1.0f));
		gridArea->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent * 2);
		horizontal->AddChild(gridArea);
		m_gridParent = gridArea;

		PanelWidgetEditorProperties* propertiesArea = m_manager->Allocate<PanelWidgetEditorProperties>("Properties");
		propertiesArea->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		propertiesArea->SetAlignedPos(Vector2(0.75f, 0.0f));
		propertiesArea->SetAlignedSize(Vector2(0.25f, 1.0f));
		horizontal->AddChild(propertiesArea);
		m_propertiesArea = propertiesArea;

		m_leftSide	  = leftSide;
		m_leftSideTop = widgetsPanel;
		m_leftSideBot = hierarchy;
		m_middle	  = m_gridParent;
		m_rightSide	  = m_propertiesArea;
	}

	void PanelWidgetEditor::PreDestruct()
	{
		if (m_currentWidget == nullptr)
			return;

		CloseCurrent(false);
	}

	void PanelWidgetEditor::SaveLayoutToStream(OStream& stream)
	{
		stream << m_leftSide->GetAlignedSizeX();
		stream << m_leftSideTop->GetAlignedSizeY();
		stream << m_middle->GetAlignedSizeX();
		stream << m_lastOpenWidget;
	}

	void PanelWidgetEditor::LoadLayoutFromStream(IStream& stream)
	{
		float align0 = 0.0f, align1 = 0.0f, align2 = 0.0f;
		stream >> align0 >> align1 >> align2;

		m_leftSide->SetAlignedSizeX(align0);
		m_middle->SetAlignedPosX(align0);
		m_middle->SetAlignedSizeX(align2);
		m_rightSide->SetAlignedPosX(align0 + align2);
		m_rightSide->SetAlignedSizeX(1.0f - align0 - align2);
		m_leftSideTop->SetAlignedSizeY(align1);
		m_leftSideBot->SetAlignedPosY(align1);
		m_leftSideBot->SetAlignedSizeY(1.0f - align1);

		stream >> m_lastOpenWidget;
		OpenWidget(m_lastOpenWidget);
	}

	void PanelWidgetEditor::Initialize()
	{
		Widget::Initialize();
		RefreshWidgets();
	}

	void PanelWidgetEditor::Tick(float delta)
	{
	}

	void PanelWidgetEditor::RefreshWidgets()
	{
		m_widgetsLayout->DeallocAllChildren();
		m_widgetsLayout->RemoveAllChildren();
		m_widgetsController->ClearItems();
		m_categories.clear();

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

				auto it = linatl::find_if(m_categories.begin(), m_categories.end(), [&category](const CategoryInfo& inf) -> bool { return inf.title.compare(category) == 0; });

				if (it != m_categories.end())
				{
					it->widgets.push_back(widgetInfo);
				}
				else
				{
					m_categories.push_back({});
					CategoryInfo& categoryInfo = m_categories.back();
					categoryInfo.title		   = category;
					categoryInfo.widgets.push_back(widgetInfo);
				}
			}
		}

		for (CategoryInfo& category : m_categories)
		{
			Widget* cat = CommonWidgets::BuildDefaultFoldItem(this, nullptr, Theme::GetDef().baseIndent, "", Color::White, category.title, true, nullptr, true, true);
			m_widgetsController->AddItem(cat->GetChildren().front());
			m_widgetsLayout->AddChild(cat);

			for (WidgetInfo& wif : category.widgets)
			{
				Widget* wi = CommonWidgets::BuildDefaultListItem(this, &wif, Theme::GetDef().baseIndent * 2.0f, "", Color::White, wif.title, true);
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

		if (m_currentWidget == nullptr)
			return;

		m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_currentWidget->GetID());

		Widget* sourceWidget = &m_currentWidget->GetRoot();
		Widget* rootInEditor = CommonWidgets::BuildDefaultFoldItem(this, sourceWidget, Theme::GetDef().baseIndent, "", Color::White, sourceWidget->GetWidgetProps().debugName, true, &m_foldValues[sourceWidget], true);

		m_hierarchyController->AddItem(rootInEditor->GetChildren().front());
		m_hierarchyLayout->AddChild(rootInEditor);

		AddItemForWidget(rootInEditor, sourceWidget, Theme::GetDef().baseIndent * 2);
	}

	void PanelWidgetEditor::AddItemForWidget(Widget* rootInEditor, Widget* sourceWidget, float margin)
	{
		for (Widget* c : sourceWidget->GetChildren())
		{
			const std::type_info& typeInfo = typeid(c);
			const String		  typeName = typeInfo.name();
			Widget*				  item	   = CommonWidgets::BuildDefaultFoldItem(this, c, margin, "", Color::White, c->GetWidgetProps().debugName, !c->GetChildren().empty(), &m_foldValues[c], false);

			Text* classType			   = m_manager->Allocate<Text>();
			classType->GetProps().text = "(" + ReflectionSystem::Get().Resolve(c->GetTID()).GetProperty<String>("Title"_hs) + ")";
			classType->GetFlags().Set(WF_POS_ALIGN_Y);
			classType->SetAlignedPosY(0.5f);
			classType->SetAnchorY(Anchor::Center);
			classType->Initialize();
			item->GetChildren().front()->AddChild(classType);

			m_hierarchyController->AddItem(item->GetChildren().front());
			rootInEditor->AddChild(item);
			AddItemForWidget(item, c, margin + Theme::GetDef().baseIndent);
		}
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
		bw->GetProps().itemTypeIDFilter				   = 0;
		bw->GetItemController()->GetProps().onInteract = [this, bw, layout]() {
			ResourceDirectory* selection = bw->GetItemController()->GetSelectedUserData<ResourceDirectory>().front();

			if (!selection->isFolder && selection->resourceTID == GetTypeID<GUIWidget>())
			{
				m_manager->SetForegroundDim(0.0f);
				m_manager->AddToKillList(layout);
				CheckSaveCurrent([this, selection]() { OpenWidget(selection->resourceID); });
			}
		};
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
			CheckSaveCurrent([this, selection]() { OpenWidget(selection->resourceID); });
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

	void PanelWidgetEditor::CheckSaveCurrent(Delegate<void()>&& onAct)
	{
		if (m_currentWidget == nullptr)
		{
			onAct();
			return;
		}

		GenericPopup* p = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::SaveChanges), Locale::GetStr(LocaleStr::SaveChangesDesc), ICON_LINA_LOGO, this);
		p->AddButton(GenericPopup::ButtonProps{
			.text = Locale::GetStr(LocaleStr::Yes),
			.onClicked =
				[onAct, this, p]() {
					CloseCurrent(true);
					onAct();
					m_manager->AddToKillList(p);
				},
		});

		p->AddButton(GenericPopup::ButtonProps{
			.text = Locale::GetStr(LocaleStr::No),
			.onClicked =
				[onAct, this, p]() {
					CloseCurrent(false);
					onAct();
					m_manager->AddToKillList(p);
				},
		});

		m_manager->AddToForeground(p, 0.25f);
	}

	void PanelWidgetEditor::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		if (m_currentWidget == nullptr)
			return;

		bool deleteDisabled	   = false;
		bool duplicateDisabled = false;
		bool renameDisabled	   = false;

		Vector<Widget*> selection = m_hierarchyController->GetSelectedUserData<Widget>();

		if (selection.empty())
			return;

		if (selection.size() > 1)
		{
			renameDisabled = true;
		}

		Widget* currentRoot = &m_currentWidget->GetRoot();

		auto it = linatl::find_if(selection.begin(), selection.end(), [currentRoot](Widget* w) -> bool { return w == currentRoot; });
		if (it != selection.end())
		{
			deleteDisabled	  = true;
			duplicateDisabled = true;
		}

		if (sid == 0)
		{
			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Rename),
				.altText	 = "CTRL + R",
				.hasDropdown = false,
				.isDisabled	 = renameDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Delete),
				.altText	= "DEL",
				.isDisabled = deleteDisabled,
				.userData	= userData,
			});
			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Duplicate),
				.altText	= "CTRL + D",
				.isDisabled = duplicateDisabled,
				.userData	= userData,
			});
		}
	}

	bool PanelWidgetEditor::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		if (m_currentWidget == nullptr)
			return;

		Vector<Widget*> selection = m_hierarchyController->GetSelectedUserData<Widget>();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Rename)))
		{
			RequestRename(selection.front());
			return true;
		}

		Vector<Widget*> roots;

		for (Widget* w : selection)
		{
			auto it = linatl::find_if(selection.begin(), selection.end(), [w](Widget* selected) -> bool { return selected == w->GetParent(); });
			if (it == selection.end())
				roots.push_back(w);
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{

			RequestDelete(roots);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			RequestDuplicate(roots);
			return true;
		}

		return false;
	}

	void PanelWidgetEditor::RequestDelete(Vector<Widget*> widgets)
	{
		Vector<Widget*> selection	= m_hierarchyController->GetSelectedUserData<Widget>();
		Widget*			currentRoot = &m_currentWidget->GetRoot();

		if (selection.empty())
			return;

		auto it = linatl::find_if(selection.begin(), selection.end(), [currentRoot](Widget* w) -> bool { return w == currentRoot; });
		if (it != selection.end())
		{
			return;
		}
	}

	void PanelWidgetEditor::RequestDuplicate(Vector<Widget*> widgets)
	{
		Vector<Widget*> selection	= m_hierarchyController->GetSelectedUserData<Widget>();
		Widget*			currentRoot = &m_currentWidget->GetRoot();

		if (selection.empty())
			return;

		auto it = linatl::find_if(selection.begin(), selection.end(), [currentRoot](Widget* w) -> bool { return w == currentRoot; });
		if (it != selection.end())
		{
			return;
		}
	}

	void PanelWidgetEditor::RequestRename(Widget* w)
	{
		Widget* parent = m_hierarchyController->GetItem(w);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>();
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = text->GetProps().text;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(parent->GetSizeX());
		inp->SetPos(text->GetParent()->GetPos());
		inp->SetFixedSizeY(text->GetParent()->GetSizeY());
		inp->Initialize();

		inp->GetProps().onEditEnd = [text, inp, w, parent, this](const String& str) {
			text->GetProps().text = str;
			text->CalculateTextSize();
			w->GetWidgetProps().debugName = str;
			text->GetWidgetManager()->AddToKillList(inp);
		};

		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_hierarchyController->SetFocus(true);
		m_manager->GrabControls(inp);
	}

	void PanelWidgetEditor::OpenWidget(ResourceID id)
	{
		m_propertiesArea->Refresh(nullptr);
		m_currentWidget = m_editor->GetResourcePipeline().OpenResource<GUIWidget>(id, (void*)m_manager);
		RefreshHierarchy();
		m_lastOpenWidget = id;

		Widget* root = &m_currentWidget->GetRoot();
		m_gridParent->AddChild(root);
		root->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		root->SetAlignedPos(Vector2::Zero);
		root->SetAlignedSize(Vector2::One);
		m_propertiesArea->Refresh(root);
	}

	void PanelWidgetEditor::CloseCurrent(bool save)
	{
		if (save)
			m_editor->GetResourcePipeline().CloseAndSaveResource<GUIWidget>(m_currentWidget);
		else
			m_editor->GetResourcePipeline().CloseResource<GUIWidget>(m_currentWidget);

		m_gridParent->RemoveChild(&m_currentWidget->GetRoot());
	}
} // namespace Lina::Editor
