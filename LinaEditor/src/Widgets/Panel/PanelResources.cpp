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
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/Effects/Dropshadow.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/LayoutBorder.hpp"
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

		LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
		border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		border->SetAlignedPos(Vector2(0.25f, 0.0f));
		border->SetAlignedSizeY(1.0f);
		border->GetProps().orientation			 = DirectionOrientation::Vertical;
		border->GetWidgetProps().colorBackground = Theme::GetDef().background3;
		border->GetWidgetProps().colorHovered	 = Theme::GetDef().background4;

		AddChild(border);

		border->AssignSides(folderBrowser, fileBrowser);

		m_editor->GetFileManager().AddListener(this);
		m_border = border;
	}

	void PanelResources::Destruct()
	{
		m_editor->GetFileManager().RemoveListener(this);
		m_editor->GetWindowPanelManager().RemovePayloadListener(this);
	}

	void PanelResources::Initialize()
	{
		Widget::Initialize();
		m_editor->GetWindowPanelManager().AddPayloadListener(this);
	}

	void PanelResources::Draw()
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		Widget::Draw();
	}

	void PanelResources::LoadLayoutFromStream(IStream& stream)
	{
		float alignedBorder = 0.0f;
		stream >> alignedBorder;
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

		m_border->SetAlignedPosX(alignedBorder);
		m_border->GetNegative()->SetAlignedSizeX(alignedBorder);
		m_border->GetPositive()->SetAlignedPosX(alignedBorder);
		m_border->GetPositive()->SetAlignedSizeX(1.0f - alignedBorder);
		const bool showAsGrid = m_contentsSize > LIST_CONTENTS_LIMIT;
		m_folderBrowserItemLayout->RefreshItems();
		SwitchFileBrowserContents(showAsGrid);
		m_fileBrowserItemLayout->SetGridItemSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
	}

	void PanelResources::SaveLayoutToStream(OStream& stream)
	{
		stream << m_border->GetAlignedPosX();
		stream << m_contentsSize;
		stream << static_cast<uint32>(m_favouriteDirectories.size());
		for (const String& str : m_favouriteDirectories)
			StringSerialization::SaveToStream(stream, str);
	}

	void PanelResources::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::Resource)
			return;
		m_payloadActive = true;
		m_fileBrowserItemLayout->SetOutlineThicknessAndColor(Theme::GetDef().baseOutlineThickness, Theme::GetDef().accentPrimary0);
		m_folderBrowserItemLayout->SetOutlineThicknessAndColor(Theme::GetDef().baseOutlineThickness, Theme::GetDef().accentPrimary0);
	}

	void PanelResources::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::Resource)
			return;
		m_payloadActive = false;
		m_fileBrowserItemLayout->SetOutlineThicknessAndColor(0.0f, Theme::GetDef().accentPrimary0);
		m_folderBrowserItemLayout->SetOutlineThicknessAndColor(0.0f, Theme::GetDef().accentPrimary0);
	}

	bool PanelResources::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::Resource)
			return false;

		return false;
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
		m_fileBrowserContentsAsGrid = showAsGrid;
		m_fileBrowserItemLayout->SetUseGridLayout(m_fileBrowserContentsAsGrid);
		m_fileBrowserItemLayout->RefreshItems();
	}

	Widget* PanelResources::BuildFileBrowserTop()
	{
		DirectionalLayout* topContents = m_manager->Allocate<DirectionalLayout>("TopContents");
		topContents->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		topContents->SetAlignedPos(0.0f);
		topContents->SetAlignedSizeX(1.0f);
		topContents->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		topContents->SetChildPadding(Theme::GetDef().baseIndent);
		topContents->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};

		Button* sort = m_manager->Allocate<Button>("Sort");
		sort->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
		sort->SetAlignedPosY(0.5f);
		sort->SetPosAlignmentSourceY(PosAlignmentSource::Center);
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
				m_fileBrowserItemLayout->RefreshItems();
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
		sortIcon->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		sortIcon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		sort->AddChild(sortIcon);

		Button* button = m_manager->Allocate<Button>("Back");
		button->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
		button->SetAlignedPosY(0.5f);
		button->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		button->SetAlignedSizeY(1.0f);
		button->GetProps().onClicked = [this]() {
			m_folderBrowserItemLayout->SetSelectedItem(m_viewDirectory->parent);
			SelectDirectory(m_viewDirectory->parent);
			m_folderBrowserItemLayout->SetFocus(false);
			m_fileBrowserItemLayout->SetFocus(true);
		};
		button->SetIsDisabled(true);
		topContents->AddChild(button);
		m_backButton = button;

		Icon* back			  = m_manager->Allocate<Icon>("BackIcon");
		back->GetProps().icon = ICON_ARROW_LEFT;
		back->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		back->SetAlignedPos(Vector2(0.5f, 0.5f));
		back->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		back->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		button->AddChild(back);

		Text* path = m_manager->Allocate<Text>("Path");
		path->GetFlags().Set(WF_POS_ALIGN_Y);
		path->SetAlignedPosY(0.5f);
		path->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		path->GetProps().fetchCustomClipFromSelf = true;
		path->GetProps().isDynamic				 = true;
		topContents->AddChild(path);

		InputField* searchFieldTop = m_manager->Allocate<InputField>("SearchFieldTop");
		searchFieldTop->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X);
		searchFieldTop->SetAlignedPos(Vector2(1.0f, 0.0f));
		searchFieldTop->SetPosAlignmentSourceX(PosAlignmentSource::End);
		searchFieldTop->SetAlignedSize(Vector2(0.5f, 1.0f));
		searchFieldTop->GetWidgetProps().rounding  = 0.0f;
		searchFieldTop->GetProps().usePlaceHolder  = true;
		searchFieldTop->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
		searchFieldTop->GetProps().onEdited		   = [this](const String& val) {
			   m_fileBrowserSearchStr = val;
			   m_fileBrowserItemLayout->RefreshItems();
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
		bottom->SetChildPadding(Theme::GetDef().baseIndentInner);
		bottom->GetChildMargins()				 = {.left = Theme::GetDef().baseIndentInner, .right = Theme::GetDef().baseIndent};
		bottom->GetWidgetProps().drawBackground	 = true;
		bottom->GetWidgetProps().colorBackground = Theme::GetDef().background1;
		bottom->GetBorderThickness().top		 = Theme::GetDef().baseBorderThickness;
		bottom->SetBorderColor(Theme::GetDef().background0);

		Text* itemCount = m_manager->Allocate<Text>("ItemCount");
		itemCount->GetFlags().Set(WF_POS_ALIGN_Y);
		itemCount->SetAlignedPosY(0.5f);
		itemCount->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		itemCount->GetProps().fetchCustomClipFromSelf = true;
		itemCount->GetProps().isDynamic				  = true;
		itemCount->GetProps().text					  = "0 " + Locale::GetStr(LocaleStr::Items);
		m_itemCount									  = itemCount;
		bottom->AddChild(itemCount);

		Widget* divider = m_manager->Allocate<Widget>("Divider");
		divider->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		divider->SetAlignedPosY(0.0f);
		divider->SetAlignedSizeY(1.0f);
		divider->SetFixedSizeX(2.0f);
		divider->GetWidgetProps().drawBackground  = true;
		divider->GetWidgetProps().colorBackground = Theme::GetDef().black;
		bottom->AddChild(divider);

		Text* itemSelected = m_manager->Allocate<Text>("ItemSelected");
		itemSelected->GetFlags().Set(WF_POS_ALIGN_Y);
		itemSelected->SetAlignedPosY(0.5f);
		itemSelected->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		itemSelected->GetProps().fetchCustomClipFromSelf = true;
		itemSelected->GetProps().isDynamic				 = true;
		itemSelected->GetProps().text					 = "0 " + Locale::GetStr(LocaleStr::Selected);
		m_selectedItemCount								 = itemSelected;
		bottom->AddChild(itemSelected);

		Widget* divider2 = m_manager->Allocate<Widget>("Divider");
		divider2->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		divider2->SetAlignedPosY(0.0f);
		divider2->SetAlignedSizeY(1.0f);
		divider2->SetFixedSizeX(2.0f);
		divider2->GetWidgetProps().drawBackground  = true;
		divider2->GetWidgetProps().colorBackground = Theme::GetDef().black;
		bottom->AddChild(divider2);

		Slider* sizeSlider = m_manager->Allocate<Slider>("SizeSlider");
		sizeSlider->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		sizeSlider->SetAlignedPosY(0.0f);
		sizeSlider->SetAlignedPosX(1.0f);
		sizeSlider->SetPosAlignmentSourceX(PosAlignmentSource::End);
		sizeSlider->SetAlignedSize(Vector2(0.25f, 1.0f));
		sizeSlider->GetProps().minValue		  = MIN_CONTENTS_SIZE;
		sizeSlider->GetProps().maxValue		  = MAX_CONTENTS_SIZE;
		sizeSlider->GetProps().valuePtr		  = &m_contentsSize;
		sizeSlider->GetProps().onValueChanged = [this](float val) -> void {
			if (val < LIST_CONTENTS_LIMIT && m_fileBrowserContentsAsGrid)
				SwitchFileBrowserContents(false);
			else if (!m_fileBrowserContentsAsGrid && val > LIST_CONTENTS_LIMIT)
				SwitchFileBrowserContents(true);
			m_fileBrowserItemLayout->SetGridItemSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
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
		contents->GetChildMargins() = {.top = Theme::GetDef().baseIndent};
		AddChild(contents);

		contents->AddChild(BuildFileBrowserTop());

		Widget* filler = m_manager->Allocate<Widget>("Filler");
		filler->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		filler->SetAlignedPosX(0.0f);
		filler->SetAlignedSizeX(1.0f);
		filler->SetFixedSizeY(Theme::GetDef().baseItemHeight / 2);
		contents->AddChild(filler);

		Widget* bg = m_manager->Allocate<Widget>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bg->SetAlignedPosX(0.0f);
		bg->SetAlignedSize(Vector2(1.0f, 0.0f));
		bg->GetWidgetProps().drawBackground	 = true;
		bg->GetWidgetProps().colorBackground = Theme::GetDef().background0;
		contents->AddChild(bg);

		ItemLayout* itemLayout = m_manager->Allocate<ItemLayout>("ItemLayout");
		itemLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_CONTROL_MANAGER);
		itemLayout->SetAlignedPos(Vector2::Zero);
		itemLayout->SetAlignedSize(Vector2::One);
		itemLayout->GetProps().itemsCanHaveChildren = true;
		itemLayout->SetUseGridLayout(false);
		bg->AddChild(itemLayout);
		m_fileBrowserItemLayout = itemLayout;
		m_fileBrowserItemLayout->GetContextMenu()->SetListener(this);
		m_fileBrowserItemLayout->SetGridItemSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
		m_fileBrowserItemLayout->SetUseGridLayout(m_fileBrowserContentsAsGrid);

		m_fileBrowserItemLayout->GetProps().itemsCanHaveChildren = false;
		m_fileBrowserItemLayout->GetProps().onItemSelected		 = [this](void* userData) {
			  m_selectedItemCount->GetProps().text = TO_STRING(m_fileBrowserItemLayout->GetSelectedItems().size()) + " " + Locale::GetStr(LocaleStr::Selected);
			  m_selectedItemCount->CalculateTextSize();
		};
		m_fileBrowserItemLayout->GetProps().onItemInteracted = [this](void* userData) {
			DirectoryItem* it = static_cast<DirectoryItem*>(userData);

			m_folderBrowserItemLayout->SetFocus(false);
			m_fileBrowserItemLayout->SetFocus(true);

			if (!it->isDirectory)
			{
				// interact w/ item
			}
			else
			{
				SelectDirectory(it);
			}
		};

		m_fileBrowserItemLayout->GetProps().onItemRenamed = [this](void* userData, const String& newName) {
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			item->Rename(newName);
			m_fileBrowserItemLayout->RefreshItems();
			m_fileBrowserItemLayout->SetSelectedItem(item);
		};

		m_fileBrowserItemLayout->GetProps().onDelete		= [this](void* userData) { RequestDelete(false); };
		m_fileBrowserItemLayout->GetProps().onDuplicate		= [this](void* userData) { RequestDuplicate(static_cast<DirectoryItem*>(userData), false); };
		m_fileBrowserItemLayout->GetProps().onCreatePayload = [this](void* userData) {
			DirectoryItem*	   item			= static_cast<DirectoryItem*>(userData);
			DirectionalLayout* payload		= m_manager->Allocate<DirectionalLayout>();
			payload->GetChildMargins().left = Theme::GetDef().baseIndent;
			payload->SetUserData(item);
			Text* text = m_manager->Allocate<Text>();
			text->GetFlags().Set(WF_POS_ALIGN_Y);
			text->SetAlignedPosY(0.5f);
			text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			text->GetProps().text = item->name;
			payload->AddChild(text);
			payload->Initialize();
			m_editor->GetWindowPanelManager().CreatePayload(payload, PayloadType::Resource, Vector2ui(static_cast<uint32>(text->GetSizeX() + Theme::GetDef().baseIndent * 2), static_cast<uint32>(Theme::GetDef().baseItemHeight)));
		};

		m_fileBrowserItemLayout->GetProps().onGatherItems = [this](Vector<ItemDefinition>& outItems) {
			if (m_viewDirectory == nullptr)
				return;

			const bool unfoldOverride = !m_fileBrowserSearchStr.empty();

			Vector<DirectoryItem*> children = m_viewDirectory->children;
			if (m_contentSorting == ContentSorting::TypeBased)
				linatl::sort(children.begin(), children.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return a->tid < b->tid; });

			for (DirectoryItem* c : children)
			{
				if (!m_fileBrowserSearchStr.empty() && !c->ContainsSearchString(m_fileBrowserSearchStr, false, false))
					continue;

				outItems.push_back(CreateDefinitionForItem(c, false, unfoldOverride));
			}
		};

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
		browser->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
		browser->SetChildPadding(Theme::GetDef().baseIndent);
		AddChild(browser);

		InputField* searchField = m_manager->Allocate<InputField>("SearchField");
		searchField->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		searchField->SetAlignedPosX(0.0f);
		searchField->SetAlignedSizeX(1.0f);
		searchField->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		searchField->GetWidgetProps().rounding	= 0.0f;
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
		searchField->GetProps().onEdited		= [this](const String& val) {
			   m_folderBrowserSearchStr = val;
			   m_folderBrowserItemLayout->RefreshItems();
		};
		browser->AddChild(searchField);

		Widget* bg = m_manager->Allocate<Widget>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bg->SetAlignedPosX(0.0f);
		bg->SetAlignedSize(Vector2(1.0f, 0.0f));
		bg->GetWidgetProps().drawBackground	 = true;
		bg->GetWidgetProps().colorBackground = Theme::GetDef().background0;
		browser->AddChild(bg);

		ItemLayout* itemLayout = m_manager->Allocate<ItemLayout>("ItemLayout");
		itemLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_CONTROL_MANAGER);
		itemLayout->SetAlignedPos(Vector2::Zero);
		itemLayout->SetAlignedSize(Vector2::One);
		itemLayout->GetProps().itemsCanHaveChildren = true;
		itemLayout->SetUseGridLayout(false);
		bg->AddChild(itemLayout);
		m_folderBrowserItemLayout = itemLayout;
		m_folderBrowserItemLayout->GetContextMenu()->SetListener(this);

		m_folderBrowserItemLayout->GetProps().onItemRenamed = [this](void* userData, const String& newName) {
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			item->Rename(newName);
			SelectDirectory(item);
			m_folderBrowserItemLayout->SetFocus(true);
		};
		m_folderBrowserItemLayout->GetProps().onDelete	  = [this](void* userData) { RequestDelete(true); };
		m_folderBrowserItemLayout->GetProps().onDuplicate = [this](void* userData) { RequestDuplicate(static_cast<DirectoryItem*>(userData), true); };

		m_folderBrowserItemLayout->GetProps().onGatherItems = [this](Vector<ItemDefinition>& outItems) {
			ItemDefinition favs = {
				.userData = &m_favouritesDummyData,
				.icon	  = ICON_STAR,
				.color	  = Theme::GetDef().accentPrimary1,
				.name	  = Locale::GetStr(LocaleStr::Favourites),
			};

			for (const String& dir : m_favouriteDirectories)
			{
				DirectoryItem* it = m_editor->GetFileManager().FindItemFromRelativePath(dir, m_editor->GetFileManager().GetRoot());
				if (it == nullptr)
					continue;
				favs.children.push_back(CreateDefinitionForItem(it, true, false));
			}
			outItems.push_back(favs);

			const bool	   unfoldOverride = !m_folderBrowserSearchStr.empty();
			DirectoryItem* root			  = m_editor->GetFileManager().GetRoot();
			ItemDefinition rootDef		  = {
					   .userData	   = root,
					   .unfoldOverride = unfoldOverride,
					   .nameEditable   = false,
					   .icon		   = ICON_FOLDER,
					   .name		   = root->name,
			   };

			for (DirectoryItem* it : root->children)
			{
				if (!it->isDirectory)
					continue;

				if (!m_folderBrowserSearchStr.empty() && !it->ContainsSearchString(m_folderBrowserSearchStr, true, false))
					continue;

				ItemDefinition def = CreateDefinitionForItem(it, true, unfoldOverride);

				rootDef.children.push_back(def);
			}

			outItems.push_back(rootDef);
		};

		m_folderBrowserItemLayout->GetProps().onItemSelected = [this](void* userData) {
			DirectoryItem* item = userData == nullptr ? nullptr : static_cast<DirectoryItem*>(userData);
			SelectDirectory(item);
		};
		return browser;
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

	void PanelResources::SelectDirectory(DirectoryItem* item)
	{
		m_viewDirectory = item;
		m_fileBrowserItemLayout->RefreshItems();
		m_path->GetProps().text = m_viewDirectory ? FileSystem::FixPath(m_viewDirectory->relativePath) : "";
		m_path->CalculateTextSize();
		m_backButton->SetIsDisabled(m_viewDirectory == nullptr || m_viewDirectory == m_editor->GetFileManager().GetRoot());
		m_itemCount->GetProps().text = m_viewDirectory == nullptr ? "0" : TO_STRING(m_viewDirectory->children.size());
		m_itemCount->GetProps().text += " " + Locale::GetStr(LocaleStr::Items);
		m_itemCount->CalculateTextSize();
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

	void PanelResources::RequestDelete(bool isFolderBrowser)
	{
		Vector<DirectoryItem*> selectedItems;

		if (!isFolderBrowser)
			m_fileBrowserItemLayout->FillSelectedUserData<DirectoryItem>(selectedItems);
		else
			m_folderBrowserItemLayout->FillSelectedUserData<DirectoryItem>(selectedItems);

		for (DirectoryItem* t : selectedItems)
		{
			if (t == m_editor->GetFileManager().GetRoot())
				return;
		}

		const String  text	= Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteI) + " " + TO_STRING(selectedItems.size()) + " " + Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteII);
		GenericPopup* popup = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::Delete), text, this);
		popup->AddButton({
			.text = Locale::GetStr(LocaleStr::Yes),
			.onClicked =
				[selectedItems, this, popup, isFolderBrowser]() {
					DeleteItems(selectedItems, isFolderBrowser);
					m_manager->AddToKillList(popup);
				},
		});
		popup->AddButton({.text = Locale::GetStr(LocaleStr::No), .onClicked = [this, popup]() { m_manager->AddToKillList(popup); }});
		m_manager->AddToForeground(popup);
	}

	void PanelResources::RequestDuplicate(DirectoryItem* item, bool isFolderBrowser)
	{
		if (item == m_editor->GetFileManager().GetRoot())
			return;

		const String copy = FileSystem::Duplicate(item->absolutePath);

		if (isFolderBrowser)
		{
			if (m_folderBrowserItemLayout->GetSelectedItems().size() != 1)
				return;

			DirectoryItem* parent = m_viewDirectory->parent;
			m_editor->GetFileManager().ScanItem(parent);
			m_folderBrowserItemLayout->RefreshItems();
			DirectoryItem* newItem = parent->GetChildrenByName(FileSystem::GetFilenameAndExtensionFromPath(copy));
			m_editor->GetFileManager().GenerateThumbnails(newItem, false);
			m_folderBrowserItemLayout->SetSelectedItem(newItem);
			m_folderBrowserItemLayout->SetFocus(true);
			m_fileBrowserItemLayout->SetFocus(false);
		}
		else
		{
			if (m_fileBrowserItemLayout->GetSelectedItems().size() != 1)
				return;

			DirectoryItem* parent = m_viewDirectory;
			m_editor->GetFileManager().ScanItem(parent);
			m_fileBrowserItemLayout->RefreshItems();
			DirectoryItem* newItem = parent->GetChildrenByName(FileSystem::GetFilenameAndExtensionFromPath(copy));
			m_editor->GetFileManager().GenerateThumbnails(newItem, false);
			m_fileBrowserItemLayout->SetSelectedItem(newItem);
			m_folderBrowserItemLayout->SetFocus(false);
			m_fileBrowserItemLayout->SetFocus(true);
		}
	}

	void PanelResources::AddToFavourites(DirectoryItem* item)
	{
		if (item == m_editor->GetFileManager().GetRoot())
			return;

		m_favouriteDirectories.push_back(item->relativePath);
		m_folderBrowserItemLayout->RefreshItems();
	}

	void PanelResources::RemoveFromFavourites(DirectoryItem* item)
	{
		if (item == m_editor->GetFileManager().GetRoot())
			return;

		auto it = linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return str.compare(item->relativePath) == 0; });
		m_favouriteDirectories.erase(it);
		m_folderBrowserItemLayout->RefreshItems();
	}

	void PanelResources::DeleteItems(const Vector<DirectoryItem*>& items, bool isFolderLayout)
	{
		for (DirectoryItem* it : items)
		{
			if (!FileSystem::FileOrPathExists(it->absolutePath))
				continue;

			if (it->isDirectory)
				FileSystem::DeleteDirectory(it->absolutePath);
			else
				FileSystem::DeleteFileInPath(it->absolutePath);
		}

		if (isFolderLayout)
		{
			if (items.size() == 1)
			{
				DirectoryItem* parent	   = items[0]->parent;
				const int32	   index	   = UtilVector::IndexOf(parent->children, items[0]);
				const int32	   targetIndex = UtilVector::FindNextIndexIfItemIsRemoved(parent->children, items[0]);
				m_editor->GetFileManager().ScanItem(parent);
				DirectoryItem* target = targetIndex == -1 ? parent : parent->children[targetIndex];

				if (target && !target->isDirectory)
					target = target->parent;

				m_folderBrowserItemLayout->RefreshItems();
				m_folderBrowserItemLayout->SetSelectedItem(target);
				SelectDirectory(target);
			}
			else
			{
				DirectoryItem* parent = FindFirstParentExcluding(m_viewDirectory, items);
				m_editor->GetFileManager().ScanItem(parent);
				m_folderBrowserItemLayout->RefreshItems();
				m_folderBrowserItemLayout->SetSelectedItem(parent);
				SelectDirectory(parent);
			}

			m_folderBrowserItemLayout->SetFocus(true);
			m_fileBrowserItemLayout->SetFocus(false);
		}
		else
		{
			if (items.size() == 1)
			{
				const int32 index		= UtilVector::IndexOf(m_viewDirectory->children, items[0]);
				const int32 targetIndex = UtilVector::FindNextIndexIfItemIsRemoved(m_viewDirectory->children, items[0]);
				m_editor->GetFileManager().ScanItem(m_viewDirectory);
				DirectoryItem* target = targetIndex == -1 ? nullptr : m_viewDirectory->children[targetIndex];

				m_folderBrowserItemLayout->RefreshItems();
				m_folderBrowserItemLayout->SetSelectedItem(m_viewDirectory);
				m_fileBrowserItemLayout->RefreshItems();

				if (!m_viewDirectory->children.empty())
					m_fileBrowserItemLayout->SetSelectedItem(target);
			}
			else
			{
				m_editor->GetFileManager().ScanItem(m_viewDirectory);
				m_folderBrowserItemLayout->RefreshItems();
				m_folderBrowserItemLayout->SetSelectedItem(m_viewDirectory);
				m_fileBrowserItemLayout->RefreshItems();
				if (!m_viewDirectory->children.empty())
					m_fileBrowserItemLayout->SetSelectedItem(m_viewDirectory->children[0]);
			}

			m_folderBrowserItemLayout->SetFocus(false);
			m_fileBrowserItemLayout->SetFocus(true);
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

		if (favsChanged)
			m_folderBrowserItemLayout->RefreshItems();

		/* TODO REFERENCES */
	}

	bool PanelResources::OnKey(uint32 key, int32 scancode, LinaGX::InputAction act)
	{
		if (m_fileBrowserItemLayout->GetFocus() && act == LinaGX::InputAction::Pressed && key == LINAGX_KEY_BACKSPACE)
		{
			m_folderBrowserItemLayout->SetSelectedItem(m_viewDirectory->parent);
			SelectDirectory(m_viewDirectory->parent);
			m_folderBrowserItemLayout->SetFocus(false);
			m_fileBrowserItemLayout->SetFocus(true);

			return true;
		}
		if (act == LinaGX::InputAction::Pressed && key == LINAGX_KEY_F && m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
		{
			if (m_fileBrowserItemLayout->GetFocus() && !m_fileBrowserItemLayout->GetSelectedItems().empty())
			{
				DirectoryItem* item = static_cast<DirectoryItem*>(m_fileBrowserItemLayout->GetSelectedItems().back()->GetUserData());
				auto		   it	= linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return item->relativePath.compare(str) == 0; });
				if (it == m_favouriteDirectories.end())
					AddToFavourites(item);
				else
					RemoveFromFavourites(item);

				return true;
			}

			if (m_folderBrowserItemLayout->GetFocus() && !m_folderBrowserItemLayout->GetSelectedItems().empty())
			{
				DirectoryItem* item = static_cast<DirectoryItem*>(m_folderBrowserItemLayout->GetSelectedItems().back()->GetUserData());
				auto		   it	= linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return item->relativePath.compare(str) == 0; });
				if (it == m_favouriteDirectories.end())
					AddToFavourites(item);
				else
					RemoveFromFavourites(item);
				return true;
			}
		}

		return false;
	}

	bool PanelResources::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		DirectoryItem* item = userData == nullptr ? (filemenu == m_folderBrowserItemLayout->GetContextMenu() ? m_editor->GetFileManager().GetRoot() : m_viewDirectory) : static_cast<DirectoryItem*>(userData);

		if (userData == &m_favouritesDummyData)
		{
			m_favouriteDirectories.clear();
			m_folderBrowserItemLayout->RefreshItems();
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Folder)))
		{
			const String name			 = item->GetNewItemName("Folder");
			const String newRelativePath = item->relativePath + "/" + name;
			FileSystem::CreateFolderInPath(item->absolutePath + "/" + name);
			m_editor->GetFileManager().ScanItem(item);
			DirectoryItem* newItem = item->GetChildrenByName(name);
			m_folderBrowserItemLayout->RefreshItems();

			if (filemenu == m_folderBrowserItemLayout->GetContextMenu())
			{
				m_folderBrowserItemLayout->SetSelectedItem(newItem);
				SelectDirectory(newItem);
			}
			else
			{
				m_fileBrowserItemLayout->RefreshItems();
				m_fileBrowserItemLayout->SetSelectedItem(newItem);
			}

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::World)))
		{
			const String name  = item->GetNewItemName("World") + ".linaworld";
			EntityWorld	 world = EntityWorld(item->relativePath + "/" + name, 0);
			world.SaveToFileAsBinary(item->absolutePath + "/" + name);
			m_editor->GetFileManager().ScanItem(item);
			DirectoryItem* newItem = item->GetChildrenByName(name);
			m_editor->GetFileManager().GenerateThumbnails(newItem, false);
			m_fileBrowserItemLayout->RefreshItems();
			m_fileBrowserItemLayout->SetSelectedItem(newItem);
			m_folderBrowserItemLayout->SetFocus(false);
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

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{
			RequestDelete(filemenu == m_folderBrowserItemLayout->GetContextMenu());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			RequestDuplicate(item, filemenu == m_folderBrowserItemLayout->GetContextMenu());

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::AddToFavourites)))
		{
			AddToFavourites(item);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::RemoveFromFavourites)))
		{
			RemoveFromFavourites(item);
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
			DirectoryItem* item = userData == nullptr ? nullptr : static_cast<DirectoryItem*>(userData);

			const uint32 itemSize = static_cast<uint32>(filemenu == m_fileBrowserItemLayout->GetContextMenu() ? m_fileBrowserItemLayout->GetSelectedItems().size() : m_folderBrowserItemLayout->GetSelectedItems().size());

			bool canCreate = itemSize < 2;

			if (canCreate && item)
			{
				if (!item->isDirectory)
					canCreate = false;
			}

			if (canCreate && item == nullptr)
			{
				if (filemenu == m_folderBrowserItemLayout->GetContextMenu())
					canCreate = false;
				else if (m_viewDirectory == nullptr)
					canCreate = false;
			}

			outData.push_back(FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Create), .dropdownIcon = ICON_CHEVRON_RIGHT, .hasDropdown = true, .isDisabled = !canCreate, .userData = userData});

			bool canPerformActions = item != m_editor->GetFileManager().GetRoot();

			if (itemSize > 1)
			{
				Vector<DirectoryItem*> selected;

				if (filemenu == m_fileBrowserItemLayout->GetContextMenu())
					m_fileBrowserItemLayout->FillSelectedUserData(selected);
				else
					m_folderBrowserItemLayout->FillSelectedUserData(selected);

				auto it = linatl::find_if(selected.begin(), selected.end(), [this](DirectoryItem* it) -> bool { return it == m_editor->GetFileManager().GetRoot(); });
				if (it != selected.end())
					canPerformActions = false;
			}

			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Delete),
				.altText	= "DEL",
				.isDisabled = userData == nullptr || !canPerformActions,
				.userData	= userData,
			});

			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Duplicate),
				.altText	= "CTRL + D",
				.isDisabled = itemSize != 1 || userData == nullptr || !canPerformActions,
				.userData	= userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			bool alreadyInFav = false;
			if (item)
			{
				auto it		 = linatl::find_if(m_favouriteDirectories.begin(), m_favouriteDirectories.end(), [item](const String& str) -> bool { return str.compare(item->relativePath) == 0; });
				alreadyInFav = it != m_favouriteDirectories.end();
			}

			const String str = alreadyInFav ? Locale::GetStr(LocaleStr::RemoveFromFavourites) : Locale::GetStr(LocaleStr::AddToFavourites);
			outData.push_back(FileMenuItem::Data{
				.text			 = str,
				.altText		 = "CTRL + F",
				.headerIcon		 = ICON_STAR,
				.headerIconColor = Theme::GetDef().accentPrimary1,
				.isDisabled		 = itemSize != 1 || userData == nullptr || !canPerformActions,
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
		DirectoryItem* currentFileBrowserSelection = nullptr;
		const auto&	   selections				   = m_fileBrowserItemLayout->GetSelectedItems();
		if (!selections.empty())
			currentFileBrowserSelection = static_cast<DirectoryItem*>(m_fileBrowserItemLayout->GetSelectedItems().back()->GetUserData());

		SelectDirectory(m_viewDirectory);
		m_fileBrowserItemLayout->SetSelectedItem(currentFileBrowserSelection);
	}

} // namespace Lina::Editor
