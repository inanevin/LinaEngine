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
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
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
#include "Core/GUI/Widgets/Layout/LayoutBorder.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

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
		border->GetProps().orientation = DirectionOrientation::Vertical;
		AddChild(border);

		border->AssignSides(folderBrowser, fileBrowser);

		m_editor->GetFileManager().AddListener(this);
		m_border = border;
	}

	void PanelResources::Destruct()
	{
		m_editor->GetFileManager().RemoveListener(this);
	}

	void PanelResources::Initialize()
	{
		Widget::Initialize();
		m_folderBrowserItemLayout->RefreshItems();
	}
	void PanelResources::Draw()
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		Widget::Draw();
	}

	PanelLayoutExtra PanelResources::GetExtraLayoutData()
	{
		PanelLayoutExtra extra = {};
		extra.f[0]			   = m_border->GetAlignedPosX();
		extra.f[1]			   = m_contentsSize;
		return extra;
	}

	void PanelResources::SetExtraLayoutData(const PanelLayoutExtra& data)
	{
		m_border->SetAlignedPosX(data.f[0]);
		m_border->GetNegative()->SetAlignedSizeX(data.f[0]);
		m_border->GetPositive()->SetAlignedPosX(data.f[0]);
		m_border->GetPositive()->SetAlignedSizeX(1.0f - data.f[0]);
		m_contentsSize		  = data.f[1];
		const bool showAsGrid = m_contentsSize > LIST_CONTENTS_LIMIT;
		SwitchFileBrowserContents(showAsGrid);
		m_fileBrowserItemLayout->SetGridItemSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
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
		topContents->SetChildPadding(Theme::GetDef().baseIndentInner);
		topContents->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};

		Icon* folder			= m_manager->Allocate<Icon>("PathIcon");
		folder->GetProps().icon = ICON_FOLDER;
		folder->GetFlags().Set(WF_POS_ALIGN_Y);
		folder->SetAlignedPosY(0.5f);
		folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		topContents->AddChild(folder);

		Icon* chev				   = m_manager->Allocate<Icon>("Path>");
		chev->GetProps().icon	   = ICON_CHEVRON_RIGHT;
		chev->GetProps().textScale = 0.3f;
		chev->GetFlags().Set(WF_POS_ALIGN_Y);
		chev->SetAlignedPosY(0.5f);
		chev->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		topContents->AddChild(chev);

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
		searchFieldTop->GetProps().rounding		   = 0.0f;
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
		bottom->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		bottom->SetChildPadding(Theme::GetDef().baseIndentInner);
		bottom->GetChildMargins()				= {.left = Theme::GetDef().baseIndentInner, .right = Theme::GetDef().baseIndent};
		bottom->GetProps().backgroundStyle		= DirectionalLayout::BackgroundStyle::Default;
		bottom->GetProps().colorBackgroundStart = bottom->GetProps().colorBackgroundEnd = Theme::GetDef().background1;
		bottom->GetBorderThickness().top												= Theme::GetDef().baseBorderThickness;
		bottom->SetBorderColor(Theme::GetDef().background0);

		Text* itemCount = m_manager->Allocate<Text>("ItemCount");
		itemCount->GetFlags().Set(WF_POS_ALIGN_Y);
		itemCount->SetAlignedPosY(0.5f);
		itemCount->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		itemCount->GetProps().fetchCustomClipFromSelf = true;
		itemCount->GetProps().isDynamic				  = true;
		itemCount->GetProps().text					  = "0 " + Locale::GetStr(LocaleStr::Items);
		bottom->AddChild(itemCount);

		ShapeRect* divider = m_manager->Allocate<ShapeRect>("Divider");
		divider->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		divider->SetAlignedPosY(0.0f);
		divider->SetAlignedSizeY(1.0f);
		divider->SetFixedSizeX(2.0f);
		divider->GetProps().colorStart = divider->GetProps().colorEnd = Theme::GetDef().background0;
		bottom->AddChild(divider);

		Text* itemSelected = m_manager->Allocate<Text>("ItemSelected");
		itemSelected->GetFlags().Set(WF_POS_ALIGN_Y);
		itemSelected->SetAlignedPosY(0.5f);
		itemSelected->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		itemSelected->GetProps().fetchCustomClipFromSelf = true;
		itemSelected->GetProps().isDynamic				 = true;
		itemSelected->GetProps().text					 = "0 " + Locale::GetStr(LocaleStr::Selected);
		bottom->AddChild(itemSelected);

		ShapeRect* divider2 = m_manager->Allocate<ShapeRect>("Divider");
		divider2->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		divider2->SetAlignedPosY(0.0f);
		divider2->SetAlignedSizeY(1.0f);
		divider2->SetFixedSizeX(2.0f);
		divider2->GetProps().colorStart = divider2->GetProps().colorEnd = Theme::GetDef().background0;
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

		m_itemCount			= itemCount;
		m_selectedItemCount = itemSelected;

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

		ShapeRect* bg = m_manager->Allocate<ShapeRect>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bg->SetAlignedPosX(0.0f);
		bg->SetAlignedSize(Vector2(1.0f, 0.0f));
		bg->GetProps().colorStart = bg->GetProps().colorEnd = Theme::GetDef().background0;
		contents->AddChild(bg);

		ItemLayout* itemLayout = m_manager->Allocate<ItemLayout>("ItemLayout");
		itemLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemLayout->SetAlignedPos(Vector2::Zero);
		itemLayout->SetAlignedSize(Vector2::One);
		itemLayout->GetProps().itemsCanHaveChildren = true;
		itemLayout->SetUseGridLayout(false);
		bg->AddChild(itemLayout);
		m_fileBrowserItemLayout = itemLayout;
		m_fileBrowserItemLayout->SetGridItemSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1.25f)));
		m_fileBrowserItemLayout->SetUseGridLayout(m_fileBrowserContentsAsGrid);

		m_fileBrowserItemLayout->GetProps().itemsCanHaveChildren = false;
		m_fileBrowserItemLayout->GetProps().onGatherItems		 = [this](Vector<ItemDefinition>& outItems) {
			   if (m_folderBrowserSelection == nullptr)
				   return;

			   for (DirectoryItem* c : m_folderBrowserSelection->children)
			   {
				   if (!m_fileBrowserSearchStr.empty() && !c->ContainsSearchString(m_fileBrowserSearchStr, false, false))
					   continue;

				   outItems.push_back(CreateDefinitionForItem(c, false));
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
		searchField->GetProps().rounding		= 0.0f;
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
		searchField->GetProps().onEdited		= [this](const String& val) {
			   m_folderBrowserSearchStr = val;
			   m_folderBrowserItemLayout->RefreshItems();
		};
		browser->AddChild(searchField);

		ShapeRect* bg = m_manager->Allocate<ShapeRect>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bg->SetAlignedPosX(0.0f);
		bg->SetAlignedSize(Vector2(1.0f, 0.0f));
		bg->GetProps().colorStart = bg->GetProps().colorEnd = Theme::GetDef().background0;
		browser->AddChild(bg);

		ItemLayout* itemLayout = m_manager->Allocate<ItemLayout>("ItemLayout");
		itemLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemLayout->SetAlignedPos(Vector2::Zero);
		itemLayout->SetAlignedSize(Vector2::One);
		itemLayout->GetProps().itemsCanHaveChildren = true;
		itemLayout->SetUseGridLayout(false);
		bg->AddChild(itemLayout);
		m_folderBrowserItemLayout = itemLayout;

		m_folderBrowserItemLayout->GetProps().onGatherItems = [this](Vector<ItemDefinition>& outItems) {
			DirectoryItem* root	   = m_editor->GetFileManager().GetRoot();
			ItemDefinition rootDef = {
				.userData = root,
				.icon	  = ICON_FOLDER,
				.name	  = root->name,
			};

			for (DirectoryItem* it : root->children)
			{
				if (!it->isDirectory)
					continue;

				if (!m_folderBrowserSearchStr.empty() && !it->ContainsSearchString(m_folderBrowserSearchStr, true, true))
					continue;

				rootDef.children.push_back(CreateDefinitionForItem(it, true));
			}

			outItems.push_back(rootDef);
		};

		m_folderBrowserItemLayout->GetProps().onItemSelected = [this](void* userData) {
			if (userData == nullptr)
			{
				ChangeFolderBrowserSelection(nullptr, false);
				return;
			}

			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			ChangeFolderBrowserSelection(item, false);
		};
		return browser;
	}

	ItemDefinition PanelResources::CreateDefinitionForItem(DirectoryItem* it, bool onlyDirectories)
	{
		ItemDefinition def = {
			.userData		  = it,
			.useOutlineInGrid = it->outlineFX,
			.icon			  = it->isDirectory ? ICON_FOLDER : "",
			.texture		  = it->textureAtlas,
			.name			  = it->name,
		};

		for (DirectoryItem* c : it->children)
		{
			if (onlyDirectories && !c->isDirectory)
				continue;

			def.children.push_back(CreateDefinitionForItem(c, onlyDirectories));
		}

		return def;
	}

	bool PanelResources::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		if (sid == TO_SID(Locale::GetStr(LocaleStr::NewWorld)))
		{
			// DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			// m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->SaveEmptyWorld(item->absolutePath + "/NewWorld.linaworld");
			return true;
		}
		return false;
	}

	void PanelResources::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		/*
		if (filemenu == m_contentsSelectableList->GetFileMenu())
		{
			if (userData == nullptr)
			{
				// on empty area.
			}
			else
			{
				// On folder
				DirectoryItem* item = static_cast<DirectoryItem*>(userData);
				int			   a	= 5;
			}
		}
		else if (filemenu == m_browserSelectableList->GetFileMenu())
		{
			if (userData == nullptr)
			{
				// on empty are.
			}
			else
			{
				// on some item.
				DirectoryItem* item = static_cast<DirectoryItem*>(userData);
				int			   a	= 5;
			}
		}

		return;

		if (sid == 0)
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Create), .dropdownIcon = ICON_CHEVRON_RIGHT, .hasDropdown = true, .userData = userData},
			};
			return;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Create)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Folder), .userData = userData},
				FileMenuItem::Data{.isDivider = true},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::NewWorld), .userData = userData},
			};
			return;
		}
		*/
	}

	void PanelResources::OnFileManagerThumbnailsGenerated(DirectoryItem* src, bool wasRecursive)
	{
		ChangeFolderBrowserSelection(m_folderBrowserSelection, true);
	}

	void PanelResources::OnSelectableListFillItems(SelectableListLayout* list, Vector<SelectableListItem>& outItems, void* parentUserData)
	{
		/*
		if (list == m_browserSelectableList)
		{
			if (parentUserData)
			{
				DirectoryItem* parent = static_cast<DirectoryItem*>(parentUserData);

				outItems.reserve(parent->children.size());

				for (size_t i = 0; i < parent->children.size(); i++)
				{
					DirectoryItem* item = parent->children[i];
					if (!item->isDirectory)
						continue;

					outItems.push_back({
						.title			 = item->name,
						.userData		 = item,
						.hasChildren	 = !item->children.empty(),
						.useDropdownIcon = true,
						.useFolderIcon	 = true,
					});
				}
				return;
			}

			DirectoryItem* root = m_editor->GetFileManager().GetRoot();
			if (root)
			{
				SelectableListItem item = {
					.title			 = root->name,
					.userData		 = root,
					.hasChildren	 = !root->children.empty(),
					.useDropdownIcon = true,
					.useFolderIcon	 = true,
				};
				outItems.push_back(item);
			}
		}
		else if (list == m_contentsSelectableList)
		{
			if (m_folderBrowserSelection == nullptr)
				return;

			outItems.resize(m_folderBrowserSelection->children.size());

			for (size_t i = 0; i < m_folderBrowserSelection->children.size(); i++)
			{
				DirectoryItem* item = m_folderBrowserSelection->children[i];
				outItems[i]			= {
							.title				  = item->name,
							.userData			  = item,
							.useOutline			  = item->outlineFX,
							.hasChildren		  = !item->children.empty(),
							.useDropdownIcon	  = false,
							.useFolderIcon		  = item->isDirectory,
							.useCustomInteraction = true,
							.useCustomTexture	  = true,
							.customTexture		  = item->textureAtlas,
				};
			}
		}*/
	}

	void PanelResources::OnSelectableListItemControl(SelectableListLayout* list, void* userData)
	{
		// if (list == m_browserSelectableList)
		//{
		//	m_folderBrowserSelection = userData == nullptr ? nullptr : static_cast<DirectoryItem*>(userData);
		//	m_contentsSelectableList->RefreshItems();
		//
		//	m_path->GetProps().text = userData == nullptr ? "" : m_folderBrowserSelection->relativePath;
		//	m_path->CalculateTextSize();
		// }
		// else if (list == m_contentsSelectableList)
		//{
		//	m_currentContentsSelection = userData == nullptr ? nullptr : static_cast<DirectoryItem*>(userData);
		// }
	}

	void PanelResources::OnSelectableListPayloadDropped(SelectableListLayout* list, void* payloadUserData, void* droppedItemuserData)
	{
	}

	void PanelResources::OnSelectableListItemInteracted(SelectableListLayout* list, void* userData)
	{
		// if (list == m_contentsSelectableList)
		// {
		// 	DirectoryItem* item = static_cast<DirectoryItem*>(userData);
		//
		// 	if (item->isDirectory)
		// 	{
		// 		m_browserSelectableList->ChangeFold(item->parent, true);
		// 		m_browserSelectableList->GrabControls(item);
		//
		// 		return;
		// 	}
		// 	else
		// 	{
		// 		if (item->tid == GetTypeID<EntityWorld>())
		// 		{
		// 			auto* wm = m_editor->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
		// 			wm->InstallWorld(item->absolutePath);
		// 			return;
		// 		}
		// 	}
		// }
	}

	Widget* PanelResources::OnSelectableListBuildCustomTooltip(SelectableListLayout* list, void* userData)
	{
		return nullptr;
		if (!userData)
			return nullptr;

		DirectoryItem* item = static_cast<DirectoryItem*>(userData);

		const Vector2	   size	 = Vector2(RESOURCE_THUMBNAIL_SIZE * 1.5f, RESOURCE_THUMBNAIL_SIZE * 1.5f);
		DirectionalLayout* bg	 = m_manager->Allocate<DirectionalLayout>();
		bg->GetProps().direction = DirectionOrientation::Vertical;
		bg->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		bg->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
		bg->SetChildPadding(Theme::GetDef().baseIndent);
		bg->GetProps().backgroundStyle	  = DirectionalLayout::BackgroundStyle::Default;
		bg->GetProps().colorBackgroundEnd = bg->GetProps().colorBackgroundStart = Theme::GetDef().background0;
		bg->GetProps().outlineThickness											= Theme::GetDef().baseOutlineThickness;
		bg->GetProps().colorOutline												= Theme::GetDef().background0;
		bg->SetFixedSize(size + bg->GetChildPadding());

		ShapeRect* img = m_manager->Allocate<ShapeRect>();
		img->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		img->SetAlignedPosX(0.0f);
		img->SetAlignedSize(Vector2(1.0f, 0.0f));
		img->GetProps().colorStart = img->GetProps().colorEnd = Color::White;
		img->GetProps().imageTextureAtlas					  = item->textureAtlas;
		img->GetProps().fitImage							  = true;
		bg->AddChild(img);

		Text* txt = m_manager->Allocate<Text>();
		txt->GetFlags().Set(WF_POS_ALIGN_X);
		txt->SetAlignedPosX(0.5f);
		txt->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		txt->GetProps().text = FileSystem::RemoveExtensionFromPath(item->name);
		bg->AddChild(txt);
		bg->Initialize();

		return bg;
	}

	PayloadType PanelResources::OnSelectableListGetPayloadType(SelectableListLayout* list)
	{
		// if (list == m_browserSelectableList)
		// 	return PayloadType::BrowserSelectable;
		// if (list == m_contentsSelectableList)
		// 	return PayloadType::ContentsSelectable;
		return PayloadType::None;
	}

	void PanelResources::ChangeFolderBrowserSelection(DirectoryItem* item, bool refresh)
	{
		if (m_folderBrowserSelection == item && !refresh)
			return;

		m_folderBrowserSelection = item;
		m_fileBrowserItemLayout->RefreshItems();
		m_path->GetProps().text = m_folderBrowserSelection ? m_folderBrowserSelection->relativePath : "";
		m_path->CalculateTextSize();
	}

} // namespace Lina::Editor
