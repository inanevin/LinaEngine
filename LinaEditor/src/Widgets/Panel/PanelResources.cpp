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

#include "Editor/Widgets/Panel/PanelResources.hpp"
#include "Editor/Editor.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/IO/FileManager.hpp"
#include "Editor/IO/ExtensionSupport.hpp"
#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void PanelResources::Construct()
	{
		m_editor = Editor::Get();

		Widget* folderBrowser = BuildFolderBrowser();
		Widget* fileBrowser	  = BuildFileBrowser();

		// LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
		// border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		// border->SetAlignedPos(Vector2(0.25f, 0.0f));
		// border->SetAlignedSizeY(1.0f);
		// border->GetProps().orientation			 = DirectionOrientation::Vertical;
		// border->GetWidgetProps().colorBackground = Theme::GetDef().background2;
		// border->GetWidgetProps().colorHovered	 = Theme::GetDef().background3;
		//
		// AddChild(border);

		// border->AssignSides(folderBrowser, fileBrowser);

		m_editor->GetFileManager().AddListener(this);
	}

	void PanelResources::Destruct()
	{
		Panel::Destruct();
		m_editor->GetFileManager().RemoveListener(this);
	}

	void PanelResources::Initialize()
	{
		Widget::Initialize();
		RefreshFolderBrowser();
	}

	void PanelResources::LoadLayoutFromStream(IStream& stream)
	{
		float border = 0.0f;
		stream >> border;
		stream >> m_contentsSize;

		uint32 favDirectoriesSize = 0;
		stream >> favDirectoriesSize;

		for (uint32 i = 0; i < favDirectoriesSize; i++)
		{
			String str = "";
			StringSerialization::LoadFromStream(stream, str);
			if (m_editor->GetFileManager().FindItemFromRelativePath(str, m_editor->GetFileManager().GetRoot()) != nullptr)
				m_favouriteDirectories.push_back(str);
		}

		const bool showAsGrid = m_contentsSize > LIST_CONTENTS_LIMIT;
		RefreshFolderBrowser();
		SwitchFileBrowserContents(showAsGrid);
	}

	void PanelResources::SaveLayoutToStream(OStream& stream)
	{
		float border = 0.25f;
		stream << border;
		stream << m_contentsSize;
		stream << static_cast<uint32>(m_favouriteDirectories.size());
		for (const String& str : m_favouriteDirectories)
			StringSerialization::SaveToStream(stream, str);
	}

	void PanelResources::SaveLayoutDefaults(OStream& stream)
	{
		stream << 0.25f;
		stream << MAX_CONTENTS_SIZE;
		stream << 0;
	}

	void PanelResources::SwitchFileBrowserContents(bool showAsGrid)
	{
		if (m_fileBrowserContentsAsGrid == showAsGrid)
			return;

		m_fileBrowserContentsAsGrid						  = showAsGrid;
		m_fileBrowserController->GetProps().colorDeadItem = m_fileBrowserContentsAsGrid ? Theme::GetDef().background1 : Color(0.0f, 0.0f, 0.0f, 0.0f);

		ScrollArea* scroll = static_cast<ScrollArea*>(m_fileBrowserController->GetChildren()[1]);
		scroll->RemoveAllChildren();

		m_manager->Deallocate(m_fileBrowserLayout);

		if (m_fileBrowserContentsAsGrid)
		{
			GridLayout* grid = m_manager->Allocate<GridLayout>("GridLayout");
			grid->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			grid->SetAlignedPos(Vector2::Zero);
			grid->SetAlignedSize(Vector2::One);
			grid->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
			grid->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
			grid->GetProps().verticalPadding	= Theme::GetDef().baseIndent;
			grid->GetWidgetProps().clipChildren = true;
			scroll->AddChild(grid);
			m_fileBrowserLayout = grid;
		}
		else
		{
			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			layout->SetAlignedPos(Vector2::Zero);
			layout->SetAlignedSize(Vector2::One);
			layout->GetProps().direction		  = DirectionOrientation::Vertical;
			layout->GetWidgetProps().clipChildren = true;
			scroll->AddChild(layout);
			m_fileBrowserLayout = layout;
		}

		scroll->SetTarget(m_fileBrowserLayout);
		m_fileBrowserController->GetProps().hoverAcceptItemParents = m_fileBrowserContentsAsGrid;

		RefreshFileBrowser();
	}

	Widget* PanelResources::BuildFileBrowserTop()
	{
		DirectionalLayout* topContents = m_manager->Allocate<DirectionalLayout>("TopContents");
		topContents->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		topContents->SetAlignedPos(0.0f);
		topContents->SetAlignedSizeX(1.0f);
		topContents->SetFixedSizeY(Theme::GetDef().baseItemHeight * 2);
		topContents->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		topContents->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);

		Button* sort = m_manager->Allocate<Button>("Sort");
		sort->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
		sort->SetAlignedPosY(0.5f);
		sort->SetAnchorY(Anchor::Center);
		sort->SetAlignedSizeY(1.0f);
		sort->GetProps().onClicked = [this, sort]() {
			Popup* popup				   = m_manager->Allocate<Popup>("Popup");
			popup->GetProps().selectedIcon = ICON_CIRCLE_FILLED;
			popup->SetPos(sort->GetPos());
			popup->AddTitleItem(Locale::GetStr(LocaleStr::ContentSorting));
			popup->AddToggleItem(Locale::GetStr(LocaleStr::Alphabetical), m_contentSorting == ContentSorting::Alphabetical);
			popup->AddToggleItem(Locale::GetStr(LocaleStr::TypeBased), m_contentSorting == ContentSorting::TypeBased);
			popup->GetProps().onSelectedItem = [this](uint32 idx, void* ud) {
				m_contentSorting = static_cast<ContentSorting>(idx);

				if (m_viewDirectory)
				{
					m_editor->GetFileManager().ScanItem(m_viewDirectory);
					RefreshFileBrowser();
				}
			};
			popup->Initialize();
			popup->ScrollToItem(static_cast<int32>(m_contentSorting));
			m_manager->AddToForeground(popup);
			m_manager->GrabControls(popup);
		};
		topContents->AddChild(sort);

		Icon* sortIcon			  = m_manager->Allocate<Icon>("SortIcon");
		sortIcon->GetProps().icon = ICON_SORT_AZ;
		sortIcon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		sortIcon->SetAlignedPos(Vector2(0.5f, 0.5f));
		sortIcon->SetAnchorX(Anchor::Center);
		sortIcon->SetAnchorY(Anchor::Center);
		sort->AddChild(sortIcon);

		Button* button = m_manager->Allocate<Button>("Back");
		button->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
		button->SetAlignedPosY(0.5f);
		button->SetAnchorY(Anchor::Center);
		button->SetAlignedSizeY(1.0f);
		button->GetProps().onClicked = [this]() { SelectDirectory(m_viewDirectory->parent, true); };
		button->SetIsDisabled(true);
		topContents->AddChild(button);
		m_backButton = button;

		Icon* back			  = m_manager->Allocate<Icon>("BackIcon");
		back->GetProps().icon = ICON_ARROW_UP;
		back->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		back->SetAlignedPos(Vector2(0.5f, 0.5f));
		back->SetAnchorX(Anchor::Center);
		back->SetAnchorY(Anchor::Center);
		button->AddChild(back);

		Text* path = m_manager->Allocate<Text>("Path");
		path->GetFlags().Set(WF_POS_ALIGN_Y);
		path->SetAlignedPosY(0.5f);
		path->SetAnchorY(Anchor::Center);
		path->GetProps().isDynamic = true;
		topContents->AddChild(path);

		InputField* searchFieldTop = m_manager->Allocate<InputField>("SearchFieldTop");
		searchFieldTop->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X);
		searchFieldTop->SetAlignedPos(Vector2(1.0f, 0.0f));
		searchFieldTop->SetAnchorX(Anchor::End);
		searchFieldTop->SetAlignedSize(Vector2(0.5f, 1.0f));
		searchFieldTop->GetWidgetProps().rounding  = 0.0f;
		searchFieldTop->GetProps().usePlaceHolder  = true;
		searchFieldTop->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
		searchFieldTop->GetProps().onEdited		   = [this](const String& val) {
			   m_fileBrowserSearchStr = val;

			   if (m_fileBrowserSearchStr.empty())
			   {
				   const Vector<Widget*> items = m_fileBrowserController->GetSelectedItems();

				   if (items.size() == 1)
				   {
					   DirectoryItem* selectedItem = static_cast<DirectoryItem*>(items.front()->GetUserData());
					   SelectDirectory(selectedItem->parent, true);
					   SelectInFileBrowser(selectedItem);
				   }
				   else
					   RefreshFileBrowser();
			   }
			   else
				   RefreshFileBrowser();
		};
		topContents->AddChild(searchFieldTop);

		m_path = path;
		return topContents;
	}

	Widget* PanelResources::BuildFileBrowserBottom()
	{
		DirectionalLayout* bottom = m_manager->Allocate<DirectionalLayout>("BotContents");
		bottom->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		bottom->SetAlignedPos(0.0f);
		bottom->SetAlignedSizeX(1.0f);
		bottom->SetFixedSizeY(Theme::GetDef().baseItemHeight * 1.5f);
		bottom->GetWidgetProps().childPadding		 = Theme::GetDef().baseIndentInner;
		bottom->GetWidgetProps().childMargins		 = {.left = Theme::GetDef().baseIndentInner, .right = Theme::GetDef().baseIndent};
		bottom->GetWidgetProps().drawBackground		 = true;
		bottom->GetWidgetProps().rounding			 = 0.0f;
		bottom->GetWidgetProps().outlineThickness	 = 0.0f;
		bottom->GetWidgetProps().colorBackground	 = Theme::GetDef().background1;
		bottom->GetWidgetProps().borderThickness.top = Theme::GetDef().baseOutlineThickness;
		bottom->GetWidgetProps().colorBorders		 = Theme::GetDef().outlineColorBase;

		Text* itemCount = m_manager->Allocate<Text>("ItemCount");
		itemCount->GetFlags().Set(WF_POS_ALIGN_Y);
		itemCount->SetAlignedPosY(0.5f);
		itemCount->SetAnchorY(Anchor::Center);
		itemCount->GetProps().isDynamic = true;
		itemCount->GetProps().text		= "0 " + Locale::GetStr(LocaleStr::Items);
		m_itemCount						= itemCount;
		bottom->AddChild(itemCount);

		Text* itemSelected = m_manager->Allocate<Text>("ItemSelected");
		itemSelected->GetFlags().Set(WF_POS_ALIGN_Y);
		itemSelected->SetAlignedPosY(0.5f);
		itemSelected->SetAnchorY(Anchor::Center);
		itemSelected->GetProps().isDynamic = true;
		itemSelected->GetProps().text	   = "0 " + Locale::GetStr(LocaleStr::Selected);
		m_selectedItemCount				   = itemSelected;
		bottom->AddChild(itemSelected);

		Slider* sizeSlider = m_manager->Allocate<Slider>("SizeSlider");
		sizeSlider->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		sizeSlider->SetAlignedPosY(0.0f);
		sizeSlider->SetAlignedPosX(1.0f);
		sizeSlider->SetAnchorX(Anchor::End);
		sizeSlider->SetAlignedSize(Vector2(0.25f, 1.0f));
		sizeSlider->GetProps().minValue		  = MIN_CONTENTS_SIZE;
		sizeSlider->GetProps().maxValue		  = MAX_CONTENTS_SIZE;
		sizeSlider->GetProps().valuePtr		  = &m_contentsSize;
		sizeSlider->GetProps().onValueChanged = [this](float val) -> void {
			if (val < LIST_CONTENTS_LIMIT && m_fileBrowserContentsAsGrid)
				SwitchFileBrowserContents(false);
			else if (!m_fileBrowserContentsAsGrid && val > LIST_CONTENTS_LIMIT)
				SwitchFileBrowserContents(true);

			for (Widget* w : m_gridItems)
				w->SetFixedSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
		};

		bottom->AddChild(sizeSlider);

		return bottom;
	}

	Widget* PanelResources::BuildFileBrowser()
	{
		DirectionalLayout* contents	   = m_manager->Allocate<DirectionalLayout>("Contents");
		contents->GetProps().direction = DirectionOrientation::Vertical;
		contents->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		contents->SetAlignedPos(Vector2(0.25f, 0.0f));
		contents->SetAlignedSize(Vector2(0.75f, 1.0f));
		AddChild(contents);

		contents->AddChild(BuildFileBrowserTop());

		ItemController* itemController = m_manager->Allocate<ItemController>("ItemController");
		itemController->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemController->SetAlignedPosX(0.0f);
		itemController->SetAlignedSize(Vector2(1.0f, 0.0f));
		itemController->GetWidgetProps().drawBackground	  = true;
		itemController->GetWidgetProps().outlineThickness = 0.0f;
		itemController->GetWidgetProps().rounding		  = 0.0f;
		itemController->GetContextMenu()->SetListener(this);
		itemController->GetProps().payloadType = PayloadType::Resource;

		itemController->GetProps().onPayloadAccepted = [this](void* userData) {
			if (userData == nullptr && m_viewDirectory != nullptr)
				PerformPayload(m_viewDirectory);
			else
				PerformPayload(static_cast<DirectoryItem*>(userData));
		};

		itemController->GetProps().onCreatePayload = [this](void* userData) {
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			m_payloadItem		= item;
			Widget* root		= Editor::Get()->GetWindowPanelManager().GetPayloadRoot();
			Widget* w			= CommonWidgets::BuildPayloadForDirectoryItem(root, item);
			Editor::Get()->GetWindowPanelManager().CreatePayload(w, PayloadType::Resource, w->GetSize());
		};

		itemController->GetProps().onItemRenamed = [this](void* userdata) {
			DirectoryItem* item = static_cast<DirectoryItem*>(userdata);
			if (item == m_editor->GetFileManager().GetRoot())
				return;
			RequestRename(item, false);
		};

		itemController->GetProps().onItemSelected = [this](void* userdata) {
			m_selectedItemCount->GetProps().text = TO_STRING(m_fileBrowserController->GetSelectedItems().size()) + " " + Locale::GetStr(LocaleStr::Selected);
			m_selectedItemCount->CalculateTextSize();
		};

		itemController->GetProps().onInteract = [this]() {
			const Vector<DirectoryItem*> selections = m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			DirectoryItem*				 first		= selections.front();

			if (first->isDirectory)
			{
				SelectDirectory(first, true);
				m_folderBrowserController->MakeVisibleRecursively(m_folderBrowserController->GetItem(first));
				m_folderBrowserController->SetFocus(false);
				m_fileBrowserController->SetFocus(true);

				if (!m_viewDirectory->children.empty())
				{
					SelectInFileBrowser(m_viewDirectory->children.front());
				}
			}
			else
			{
				// ON INTERACT ITEM
			}
		};
		itemController->GetProps().onDelete = [this]() {
			Vector<DirectoryItem*> selection = m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestDelete(selection, false);
		};
		itemController->GetProps().onDuplicate = [this]() {
			Vector<DirectoryItem*> selection = m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestDuplicate(selection, false);
		};

		itemController->GetWidgetProps().dropshadow.enabled	  = true;
		itemController->GetWidgetProps().dropshadow.direction = Direction::Top;
		itemController->GetWidgetProps().dropshadow.isInner	  = true;
		itemController->GetWidgetProps().dropshadow.color	  = Theme::GetDef().black;
		itemController->GetWidgetProps().dropshadow.color.w	  = 0.25f;
		itemController->GetWidgetProps().dropshadow.steps	  = 8;

		contents->AddChild(itemController);
		m_fileBrowserController = itemController;

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		itemController->AddChild(scroll);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		scroll->AddChild(layout);
		m_fileBrowserLayout = layout;

		Widget* filler = m_manager->Allocate<Widget>("Filler");
		filler->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		filler->SetAlignedPosX(0.0f);
		filler->SetAlignedSizeX(1.0f);
		filler->SetFixedSizeY(Theme::GetDef().baseOutlineThickness);
		contents->AddChild(filler);

		contents->AddChild(BuildFileBrowserBottom());

		return contents;
	}

	Widget* PanelResources::BuildFolderBrowser()
	{
		DirectionalLayout* browser	  = m_manager->Allocate<DirectionalLayout>("Browser");
		browser->GetProps().direction = DirectionOrientation::Vertical;
		browser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		browser->SetAlignedPos(Vector2::Zero);
		browser->SetAlignedSize(Vector2(0.25f, 1.0f));
		browser->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		browser->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		AddChild(browser);

		ItemController* itemController = m_manager->Allocate<ItemController>("ItemController");
		itemController->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemController->SetAlignedPosX(0.0f);
		itemController->SetAlignedSize(Vector2(1.0f, 0.0f));
		itemController->GetWidgetProps().drawBackground	  = true;
		itemController->GetWidgetProps().outlineThickness = 0.0f;
		itemController->GetWidgetProps().rounding		  = 0.0f;
		itemController->GetContextMenu()->SetListener(this);
		itemController->GetProps().payloadType = PayloadType::Resource;

		itemController->GetProps().onCheckCanCreatePayload = [this](void* userdata) -> bool {
			if (userdata == &m_favouritesDummyData)
				return false;

			DirectoryItem* item = static_cast<DirectoryItem*>(userdata);
			if (item == m_editor->GetFileManager().GetRoot())
				return false;

			return true;
		};
		itemController->GetProps().onPayloadAccepted = [this](void* userData) {
			if (userData == nullptr)
				PerformPayload(m_editor->GetFileManager().GetRoot());
			else
				PerformPayload(static_cast<DirectoryItem*>(userData));
		};

		itemController->GetProps().onCreatePayload = [this](void* userData) {
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			m_payloadItem		= item;
			Widget* root		= Editor::Get()->GetWindowPanelManager().GetPayloadRoot();
			Widget* w			= CommonWidgets::BuildPayloadForDirectoryItem(root, item);
			Editor::Get()->GetWindowPanelManager().CreatePayload(w, PayloadType::Resource, w->GetSize());
		};

		itemController->GetProps().onItemRenamed = [this](void* userdata) {
			DirectoryItem* item = static_cast<DirectoryItem*>(userdata);
			if (item == m_editor->GetFileManager().GetRoot())
				return;
			RequestRename(item, true);
		};

		itemController->GetProps().onInteract = [this]() {
			const Vector<Widget*> selections = m_folderBrowserController->GetSelectedItems();

			if (selections.size() == 1 && selections.front()->GetUserData() == &m_favouritesDummyData)
			{
				Widget* front = selections.front();
				Text*	txt	  = front->GetWidgetOfType<Text>(front);
				if (txt)
				{
					DirectoryItem* targetItem = m_editor->GetFileManager().GetRoot()->GetChildrenByRelativePath(txt->GetProps().text, true);
					if (targetItem)
					{
						SelectDirectory(targetItem->parent, true);
						SelectInFileBrowser(targetItem);
					}
				}
			}
		};

		itemController->GetProps().onItemSelected = [this](void* userdata) {
			if (userdata != &m_favouritesDummyData)
				SelectDirectory(userdata == nullptr ? nullptr : static_cast<DirectoryItem*>(userdata), false);
		};
		itemController->GetProps().onDelete = [this]() {
			Vector<DirectoryItem*> selection = m_folderBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestDelete(selection, true);
		};
		itemController->GetProps().onDuplicate = [this]() {
			Vector<DirectoryItem*> selection = m_folderBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestDuplicate(selection, true);
		};

		itemController->GetWidgetProps().dropshadow.enabled	  = true;
		itemController->GetWidgetProps().dropshadow.direction = Direction::Top;
		itemController->GetWidgetProps().dropshadow.isInner	  = true;
		itemController->GetWidgetProps().dropshadow.color	  = Theme::GetDef().black;
		itemController->GetWidgetProps().dropshadow.color.w	  = 0.25f;
		itemController->GetWidgetProps().dropshadow.steps	  = 8;
		browser->AddChild(itemController);
		m_folderBrowserController = itemController;

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		itemController->AddChild(scroll);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("FolderBrowserLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetWidgetProps().clipChildren	  = true;
		layout->GetWidgetProps().childMargins.top = layout->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndentInner;
		scroll->AddChild(layout);
		m_folderBrowserLayout = layout;

		return browser;
	}

	void PanelResources::RefreshFolderBrowser()
	{
		m_folderBrowserController->ClearItems();
		m_folderBrowserLayout->DeallocAllChildren();
		m_folderBrowserLayout->RemoveAllChildren();

		DirectoryItem* root = m_editor->GetFileManager().GetRoot();

		if (root == nullptr)
			return;

		Widget* favWidget =
			CommonWidgets::BuildDefaultFoldItem(m_folderBrowserLayout, &m_favouritesDummyData, Theme::GetDef().baseIndent, ICON_STAR, Theme::GetDef().accentSecondary, Locale::GetStr(LocaleStr::Favourites), !m_favouriteDirectories.empty(), &m_favsUnfolded);
		m_folderBrowserController->AddItem(favWidget->GetChildren().front());
		m_folderBrowserLayout->AddChild(favWidget);

		for (const String& fav : m_favouriteDirectories)
		{
			Widget* w = CommonWidgets::BuildDefaultFoldItem(m_folderBrowserLayout, &m_favouritesDummyData, Theme::GetDef().baseIndent * 2, ICON_STAR, Theme::GetDef().accentSecondary, fav, false, nullptr);
			m_folderBrowserController->AddItem(w->GetChildren().front());
			favWidget->AddChild(w);
		}

		CreateItemForFolderBrowser(m_folderBrowserLayout, root, Theme::GetDef().baseIndent);
	}

	void PanelResources::RefreshFileBrowser()
	{
		m_fileBrowserController->ClearItems();
		m_fileBrowserLayout->DeallocAllChildren();
		m_fileBrowserLayout->RemoveAllChildren();
		m_gridItems.clear();

		Vector<DirectoryItem*> searchResult;

		if (m_fileBrowserSearchStr.empty() && m_viewDirectory == nullptr)
		{
			m_itemCount->GetProps().text = "0 " + Locale::GetStr(LocaleStr::Items);
			m_itemCount->CalculateTextSize();
			return;
		}

		Vector<DirectoryItem*>* target = nullptr;

		if (m_fileBrowserSearchStr.empty())
			target = &m_viewDirectory->children;
		else
		{
			m_editor->GetFileManager().GetRoot()->GetItemsContainingSearchString(searchResult, m_fileBrowserSearchStr, true);
			target = &searchResult;
		}

		m_itemCount->GetProps().text = TO_STRING(target->size());
		m_itemCount->GetProps().text += " " + Locale::GetStr(LocaleStr::Items);
		m_itemCount->CalculateTextSize();

		for (DirectoryItem* c : *target)
		{
			if (m_fileBrowserContentsAsGrid)
			{
				Widget* w	 = CommonWidgets::BuildDirectoryItemGridView(this, c, Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
				Text*	text = w->GetWidgetOfType<Text>(w);
				m_fileBrowserController->AddItem(w->GetChildren()[1]);
				m_fileBrowserLayout->AddChild(w);
				m_gridItems.push_back(w);
			}
			else
			{
				Widget* w = CommonWidgets::BuildDirectoryItemListView(this, c);
				m_fileBrowserController->AddItem(w);
				m_fileBrowserLayout->AddChild(w);
			}
		}
	}

	void PanelResources::CreateItemForFolderBrowser(Widget* parentWidget, DirectoryItem* targetItem, float margin)
	{
		Widget* w = CommonWidgets::BuildDirectoryItemFolderView(parentWidget, targetItem, margin);

		Text* text = w->GetWidgetOfType<Text>(w);
		m_folderBrowserController->AddItem(w->GetChildren().front());
		parentWidget->AddChild(w);

		for (DirectoryItem* child : targetItem->children)
		{
			if (!child->isDirectory)
				continue;

			CreateItemForFolderBrowser(w, child, margin + Theme::GetDef().baseIndent);
		}
	}

	void PanelResources::PerformPayload(DirectoryItem* targetItem)
	{
		if (!targetItem->isDirectory)
			return;

		DirectoryItem*	   payloadItem = m_payloadItem;
		TextureAtlasImage* atlas	   = payloadItem->textureAtlas;
		payloadItem->textureAtlas	   = nullptr;
		const String  newAbsPath	   = targetItem->absolutePath + "/" + payloadItem->name;
		const String& viewDirectoryAbs = m_viewDirectory->absolutePath;

		if (payloadItem->parent == targetItem)
			return;

		// Move
		FileSystem::PerformMove(payloadItem->absolutePath, targetItem->absolutePath);
		m_editor->GetFileManager().ScanItemRecursively(m_editor->GetFileManager().GetRoot());

		// Select next view directory.
		DirectoryItem* root = m_editor->GetFileManager().GetRoot();
		DirectoryItem* view = root->GetChildrenByAbsPath(viewDirectoryAbs, true);
		if (view != nullptr)
			SelectDirectory(view, true);
		else
			SelectDirectory(root, true);

		root->GetChildrenByAbsPath(newAbsPath, true)->textureAtlas = atlas;
	}

	ItemDefinition PanelResources::CreateDefinitionForItem(DirectoryItem* it, bool onlyDirectories, bool unfoldOverride)
	{
		ItemDefinition def = {
			.userData		  = it,
			.useOutlineInGrid = it->outlineFX,
			.unfoldOverride	  = unfoldOverride,
			.icon			  = it->isDirectory ? ICON_FOLDER : "",
			.texture		  = it->textureAtlas,
			.name			  = it->name,
		};

		for (DirectoryItem* c : it->children)
		{
			if (onlyDirectories && !c->isDirectory)
				continue;

			def.children.push_back(CreateDefinitionForItem(c, onlyDirectories, unfoldOverride));
		}

		return def;
	}

	void PanelResources::SelectDirectory(DirectoryItem* item, bool selectInFolderBrowserController)
	{
		if (selectInFolderBrowserController)
			m_folderBrowserController->SelectItem(m_folderBrowserController->GetItem(item), true, false);

		m_viewDirectory = item;
		RefreshFileBrowser();

		m_path->GetProps().text = m_viewDirectory ? FileSystem::FixPath(m_viewDirectory->relativePath) : "";
		m_path->CalculateTextSize();
		m_backButton->SetIsDisabled(m_viewDirectory == nullptr || m_viewDirectory == m_editor->GetFileManager().GetRoot());

		if (item != nullptr)
			m_folderBrowserController->MakeVisibleRecursively(m_folderBrowserController->GetItem(item));
	}

	void PanelResources::SelectInFileBrowser(DirectoryItem* targetItem)
	{
		m_fileBrowserController->SelectItem(m_fileBrowserController->GetItem(targetItem), true, true);
		m_folderBrowserController->SetFocus(false);
		m_fileBrowserController->SetFocus(true);
	}

	DirectoryItem* PanelResources::FindFirstParentExcluding(DirectoryItem* searchStart, const Vector<DirectoryItem*>& excludedItems)
	{
		auto it = linatl::find_if(excludedItems.begin(), excludedItems.end(), [searchStart](DirectoryItem* it) -> bool { return searchStart == it; });
		if (it == excludedItems.end())
			return searchStart;

		if (searchStart->parent != nullptr)
			return FindFirstParentExcluding(searchStart->parent, excludedItems);

		return nullptr;
	}

	void PanelResources::RequestDelete(const Vector<DirectoryItem*>& items, bool isFolderBrowser)
	{
		DirectoryItem* root	  = m_editor->GetFileManager().GetRoot();
		auto		   rootIt = linatl::find_if(items.begin(), items.end(), [root](DirectoryItem* it) -> bool { return it == root; });
		if (rootIt != items.end())
			return;

		const String  text	= Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteI) + " " + TO_STRING(items.size()) + " " + Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteII);
		GenericPopup* popup = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::Delete), text, ICON_TRASH, this);
		popup->AddButton({
			.text = Locale::GetStr(LocaleStr::Yes),
			.onClicked =
				[items, this, popup, isFolderBrowser]() {
					DeleteItems(items, isFolderBrowser);
					m_manager->AddToKillList(popup);
				},
		});
		popup->AddButton({.text = Locale::GetStr(LocaleStr::No), .onClicked = [this, popup]() { m_manager->AddToKillList(popup); }});
		m_manager->AddToForeground(popup);
	}

	void PanelResources::RequestDuplicate(const Vector<DirectoryItem*>& items, bool isFolderBrowser)
	{
		DirectoryItem* root	  = m_editor->GetFileManager().GetRoot();
		auto		   rootIt = linatl::find_if(items.begin(), items.end(), [root](DirectoryItem* it) -> bool { return it == root; });
		if (rootIt != items.end())
			return;

		bool   containsDirectory = false;
		String duplicateAbs		 = "";
		for (DirectoryItem* item : items)
		{
			duplicateAbs = FileSystem::Duplicate(item->absolutePath);

			if (item->isDirectory)
				containsDirectory = true;
		}

		if (isFolderBrowser)
		{
			DirectoryItem* root = m_editor->GetFileManager().GetRoot();
			m_editor->GetFileManager().ScanItemRecursively(root);
			DirectoryItem* lastDuplicate = root->GetChildrenByAbsPath(duplicateAbs, true);
			RefreshFolderBrowser();
			SelectDirectory(lastDuplicate, true);
		}
		else
		{
			m_editor->GetFileManager().ScanItemRecursively(m_viewDirectory);

			if (containsDirectory)
			{
				RefreshFolderBrowser();
				SelectDirectory(m_viewDirectory, true);
			}

			m_folderBrowserController->SetFocus(false);
			m_fileBrowserController->SetFocus(true);
			RefreshFileBrowser();
			DirectoryItem* lastDuplicate = m_viewDirectory->GetChildrenByAbsPath(duplicateAbs, false);
			SelectInFileBrowser(lastDuplicate);
		}
	}

	void PanelResources::RequestRename(DirectoryItem* item, bool isFolderBrowser)
	{
		if (item == m_editor->GetFileManager().GetRoot())
			return;

		Widget* parent = isFolderBrowser ? m_folderBrowserController->GetItem(item) : m_fileBrowserController->GetItem(item);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>();
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = text->GetProps().text;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX((isFolderBrowser || !m_fileBrowserContentsAsGrid) ? parent->GetSizeX() / 2 : parent->GetSizeX());
		inp->Initialize();

		if (isFolderBrowser || (!isFolderBrowser && !m_fileBrowserContentsAsGrid))
		{
			inp->SetPosX(text->GetPosX());
			inp->SetPosY(text->GetParent()->GetPosY());
		}
		else
		{
			inp->SetPos(text->GetParent()->GetPos());
			inp->SetFixedSizeY(text->GetParent()->GetSizeY());
			// inp->SetPosY(text->GetPosY());
		}

		inp->GetProps().onEditEnd = [text, inp, item, isFolderBrowser, parent, this](const String& str) {
			text->GetProps().text = str;
			text->CalculateTextSize();
			item->Rename(str);
			text->GetWidgetManager()->AddToKillList(inp);

			if (item == m_viewDirectory)
			{
				m_path->GetProps().text = item->relativePath;
				m_path->CalculateTextSize();
			}
			const String   absPath		= item->absolutePath;
			const bool	   wasDirectory = item->isDirectory;
			DirectoryItem* dirParent	= item->parent;
			m_editor->GetFileManager().ScanItem(item->parent);

			if (isFolderBrowser)
			{
				RefreshFolderBrowser();
				SelectDirectory(item, true);
				m_folderBrowserController->SetFocus(true);
				m_fileBrowserController->SetFocus(false);
			}
			else
			{
				if (wasDirectory)
				{
					RefreshFolderBrowser();
					SelectDirectory(dirParent, true);
				}
				else
					RefreshFileBrowser();

				DirectoryItem* newItem = m_editor->GetFileManager().GetRoot()->GetChildrenByAbsPath(absPath, true);
				SelectInFileBrowser(newItem);
			}
		};

		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_folderBrowserController->SetFocus(false);
		m_fileBrowserController->SetFocus(false);
		m_manager->GrabControls(inp);
	}

	void PanelResources::AddToFavourites(DirectoryItem* item)
	{
		if (item == m_editor->GetFileManager().GetRoot())
			return;

		const Vector<DirectoryItem*> selections = m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
		m_favouriteDirectories.push_back(item->relativePath);
		RefreshFolderBrowser();
		SelectDirectory(m_viewDirectory, true);

		if (selections.size() == 1)
			SelectInFileBrowser(selections.front());
	}

	void PanelResources::RemoveFromFavourites(DirectoryItem* item)
	{
		if (item == m_editor->GetFileManager().GetRoot())
			return;

		const Vector<DirectoryItem*> selections = m_fileBrowserController->GetSelectedUserData<DirectoryItem>();

		auto it = linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return str.compare(item->relativePath) == 0; });
		m_favouriteDirectories.erase(it);
		RefreshFolderBrowser();
		SelectDirectory(m_viewDirectory, true);

		if (selections.size() == 1)
			SelectInFileBrowser(selections.front());
	}

	void PanelResources::DeleteItems(const Vector<DirectoryItem*>& items, bool isFolderBrowser)
	{
		bool containsDirectories = false;
		for (DirectoryItem* it : items)
		{
			if (!FileSystem::FileOrPathExists(it->absolutePath))
				continue;

			if (it->isDirectory)
			{
				FileSystem::DeleteDirectory(it->absolutePath);
				containsDirectories = true;
			}
			else
			{
				FileSystem::DeleteFileInPath(it->absolutePath);
			}
		}

		if (!isFolderBrowser)
		{
			m_editor->GetFileManager().ScanItemRecursively(m_viewDirectory);

			if (containsDirectories)
			{
				RefreshFolderBrowser();
				SelectDirectory(m_viewDirectory, true);
				m_folderBrowserController->SetFocus(false);
				m_fileBrowserController->SetFocus(true);
			}
			else
				RefreshFileBrowser();

			m_selectedItemCount->GetProps().text = "0 " + Locale::GetStr(LocaleStr::Selected);
			m_selectedItemCount->CalculateTextSize();
		}
		else
		{
			DirectoryItem* parent = FindFirstParentExcluding(m_viewDirectory, items);
			m_editor->GetFileManager().ScanItemRecursively(parent);
			RefreshFolderBrowser();
			SelectDirectory(parent, true);
		}

		bool favsChanged = false;
		for (Vector<String>::iterator it = m_favouriteDirectories.begin(); it != m_favouriteDirectories.end();)
		{
			const String&  str	= *it;
			DirectoryItem* item = m_editor->GetFileManager().FindItemFromRelativePath(str, nullptr);
			if (item == nullptr)
			{
				it			= m_favouriteDirectories.erase(it);
				favsChanged = true;
			}
			else
				++it;
		}

		// if (favsChanged)
		//	m_folderBrowserItemLayout->RefreshItems();

		/* TODO REFERENCES */
	}

	bool PanelResources::OnKey(uint32 key, int32 scancode, LinaGX::InputAction act)
	{
		if (m_fileBrowserController->GetIsFocused() && act == LinaGX::InputAction::Pressed && key == LINAGX_KEY_BACKSPACE)
		{
			SelectDirectory(m_viewDirectory->parent, true);
			return true;
		}

		if (act == LinaGX::InputAction::Pressed && key == LINAGX_KEY_F && m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
		{
			if (m_folderBrowserController->GetIsFocused())
			{
				const Vector<Widget*> selection = m_folderBrowserController->GetSelectedItems();
				if (selection.size() == 1)
				{
					void* ud = selection.front()->GetUserData();
					if (ud == &m_favouritesDummyData)
					{
						Text* txt = selection.front()->GetWidgetOfType<Text>(selection.front());
						if (txt)
						{
							DirectoryItem* item = m_editor->GetFileManager().GetRoot()->GetChildrenByRelativePath(txt->GetProps().text, true);
							if (item)
								RemoveFromFavourites(item);
						}
					}
					else
					{
						DirectoryItem* item = static_cast<DirectoryItem*>(ud);
						auto		   it	= linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return item->relativePath.compare(str) == 0; });
						if (it != m_favouriteDirectories.end())
							RemoveFromFavourites(item);
						else
							AddToFavourites(item);
					}

					return true;
				}
			}
			else if (m_fileBrowserController->GetIsFocused())
			{
				const Vector<Widget*> selection = m_fileBrowserController->GetSelectedItems();

				if (selection.size() == 1)
				{
					DirectoryItem* item = static_cast<DirectoryItem*>(selection.front()->GetUserData());
					auto		   it	= linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return item->relativePath.compare(str) == 0; });
					if (it != m_favouriteDirectories.end())
						RemoveFromFavourites(item);
					else
						AddToFavourites(item);
					return true;
				}
			}
		}

		if (act == LinaGX::InputAction::Pressed && key == LINAGX_KEY_R && m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
		{
			if (m_folderBrowserController->GetIsFocused())
			{
				const Vector<Widget*> selection = m_folderBrowserController->GetSelectedItems();
				if (selection.size() == 1)
				{
					void* ud = selection.front()->GetUserData();

					if (ud != &m_favouritesDummyData)
					{
						DirectoryItem* item = static_cast<DirectoryItem*>(ud);
						RequestRename(item, true);
						return true;
					}
				}
			}
			else if (m_fileBrowserController->GetIsFocused())
			{
				const Vector<Widget*> selection = m_fileBrowserController->GetSelectedItems();
				if (selection.size() == 1)
				{
					DirectoryItem* item = static_cast<DirectoryItem*>(selection.front()->GetUserData());
					RequestRename(item, false);
					return true;
				}
			}
		}

		return false;
	}

	bool PanelResources::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		Vector<void*> selections = filemenu == m_folderBrowserController->GetContextMenu() ? m_folderBrowserController->GetSelectedUserData<void>() : m_fileBrowserController->GetSelectedUserData<void>();

		if (selections.empty())
		{
			if (filemenu == m_folderBrowserController->GetContextMenu())
				selections.push_back(m_editor->GetFileManager().GetRoot());
			else
				selections.push_back(m_viewDirectory);
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Folder)))
		{
			for (void* ud : selections)
			{
				DirectoryItem* item			   = static_cast<DirectoryItem*>(ud);
				const String   name			   = item->GetNewItemName("Folder");
				const String   newRelativePath = item->relativePath + "/" + name;
				FileSystem::CreateFolderInPath(item->absolutePath + "/" + name);
				m_editor->GetFileManager().ScanItem(item);
				DirectoryItem* newItem = item->GetChildrenByName(name);

				if (filemenu == m_folderBrowserController->GetContextMenu())
				{
					RefreshFolderBrowser();
					Widget* newWidget = m_folderBrowserController->GetItem(newItem);
					m_folderBrowserController->SelectItem(newWidget, true, true);
					m_folderBrowserController->MakeVisibleRecursively(newWidget);
				}
				else
				{
					RefreshFolderBrowser();
					SelectDirectory(m_viewDirectory, true);
					RefreshFileBrowser();
					SelectInFileBrowser(newItem);
				}
			}

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::World)))
		{
			// const String name  = item->GetNewItemName("World") + ".linaworld";
			// EntityWorld	 world = EntityWorld(item->relativePath + "/" + name, 0);
			// world.SaveToFileAsBinary(item->absolutePath + "/" + name);
			// m_editor->GetFileManager().ScanItem(item);
			// DirectoryItem* newItem = item->GetChildrenByName(name);
			// m_editor->GetFileManager().GenerateThumbnails(newItem, false);
			// m_fileBrowserItemLayout->RefreshItems();
			// m_fileBrowserItemLayout->SetSelectedItem(newItem);
			// m_folderBrowserItemLayout->SetFocus(false);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Material)))
		{
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::PhysicsMaterial)))
		{

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Shader)))
		{

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Rename)))
		{
			Vector<DirectoryItem*> items = filemenu == m_folderBrowserController->GetContextMenu() ? m_folderBrowserController->GetSelectedUserData<DirectoryItem>() : m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestRename(items.front(), filemenu == m_folderBrowserController->GetContextMenu());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{
			Vector<DirectoryItem*> items = filemenu == m_folderBrowserController->GetContextMenu() ? m_folderBrowserController->GetSelectedUserData<DirectoryItem>() : m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestDelete(items, filemenu == m_folderBrowserController->GetContextMenu());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			Vector<DirectoryItem*> items = filemenu == m_folderBrowserController->GetContextMenu() ? m_folderBrowserController->GetSelectedUserData<DirectoryItem>() : m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			RequestDuplicate(items, filemenu == m_folderBrowserController->GetContextMenu());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::AddToFavourites)))
		{
			Vector<DirectoryItem*> items = filemenu == m_folderBrowserController->GetContextMenu() ? m_folderBrowserController->GetSelectedUserData<DirectoryItem>() : m_fileBrowserController->GetSelectedUserData<DirectoryItem>();
			AddToFavourites(items.front());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::RemoveFromFavourites)))
		{
			if (selections.front() == &m_favouritesDummyData)
			{
				Widget* w	= m_folderBrowserController->GetSelectedItems().front();
				Text*	txt = w->GetWidgetOfType<Text>(w);
				if (txt)
				{
					DirectoryItem* item = m_editor->GetFileManager().GetRoot()->GetChildrenByRelativePath(txt->GetProps().text, true);
					if (item)
						RemoveFromFavourites(item);
				}
			}
			else
				RemoveFromFavourites(static_cast<DirectoryItem*>(selections.front()));
			return true;
		}

		return false;
	}

	void PanelResources::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		if (userData == &m_favouritesDummyData)
		{
			outData.push_back(FileMenuItem::Data{
				.text	  = Locale::GetStr(LocaleStr::RemoveAll),
				.userData = userData,
			});
			return;
		}

		if (sid == 0)
		{
			bool deleteDisabled	   = false;
			bool duplicateDisabled = false;
			bool favDisabled	   = false;
			bool createDisabled	   = false;
			bool alreadyInFav	   = false;
			bool renameDisabled	   = false;
			bool includesFavs	   = false;

			const Vector<void*> selections = filemenu == m_folderBrowserController->GetContextMenu() ? m_folderBrowserController->GetSelectedUserData<void>() : m_fileBrowserController->GetSelectedUserData<void>();

			for (void* ud : selections)
			{
				if (ud == &m_favouritesDummyData)
				{
					includesFavs = true;
					break;
				}
			}

			if (includesFavs)
			{
				deleteDisabled = duplicateDisabled = createDisabled = renameDisabled = favDisabled = true;

				if (selections.size() == 1 && !m_favouriteDirectories.empty())
				{
					alreadyInFav = true;
					favDisabled	 = false;
				}
			}
			else
			{
				if (selections.empty())
				{
					deleteDisabled = duplicateDisabled = favDisabled = renameDisabled = true;
				}
				else
				{
					// No duplicate or delete if root is in the selection.
					DirectoryItem* root = m_editor->GetFileManager().GetRoot();
					if (linatl::find_if(selections.begin(), selections.end(), [root](void* dirItem) -> bool { return dirItem == root; }) != selections.end())
					{
						duplicateDisabled = true;
						deleteDisabled	  = true;
						renameDisabled	  = true;
						favDisabled		  = true;
					}

					// Can only add to/remove from fav if single selection.
					if (selections.size() > 1)
					{
						favDisabled	   = true;
						createDisabled = true;
						renameDisabled = true;
					}
					else
					{
						DirectoryItem* item = static_cast<DirectoryItem*>(selections.front());

						if (selections.size() == 1 && !item->isDirectory)
							createDisabled = true;

						alreadyInFav = linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return str.compare(item->relativePath) == 0; }) != m_favouriteDirectories.end();
					}
				}
			}

			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Rename),
				.altText	 = "CTRL + R",
				.hasDropdown = false,
				.isDisabled	 = renameDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		  = Locale::GetStr(LocaleStr::Create),
				.dropdownIcon = ICON_CHEVRON_RIGHT,
				.hasDropdown  = true,
				.isDisabled	  = createDisabled,
				.userData	  = userData,
			});
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
			outData.push_back(FileMenuItem::Data{.isDivider = true});

			const String favText = alreadyInFav ? Locale::GetStr(LocaleStr::RemoveFromFavourites) : Locale::GetStr(LocaleStr::AddToFavourites);

			outData.push_back(FileMenuItem::Data{
				.text			 = favText,
				.altText		 = "CTRL + F",
				.headerIcon		 = ICON_STAR,
				.headerIconColor = Theme::GetDef().accentSecondary,
				.isDisabled		 = favDisabled,
				.userData		 = userData,
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Create)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Folder), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Shader), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Material), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::PhysicsMaterial), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::World), .userData = userData},
			};
		}
	}

	void PanelResources::OnFileManagerThumbnailsGenerated(DirectoryItem* src, bool wasRecursive)
	{
		RefreshFileBrowser();
	}

} // namespace Lina::Editor
