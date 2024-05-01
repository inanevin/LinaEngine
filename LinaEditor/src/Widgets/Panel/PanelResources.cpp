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
#include "Editor/Meta/ProjectData.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/IO/FileManager.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
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
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{
	void PanelResources::Construct()
	{
		m_editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		Widget* browser	 = BuildBrowser();
		Widget* contents = BuildContents();

		LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
		border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		border->SetAlignedPos(Vector2(0.25f, 0.0f));
		border->SetAlignedSizeY(1.0f);
		border->GetProps().orientation = DirectionOrientation::Vertical;
		AddChild(border);

		border->AssignSides(browser, contents);

		m_border = border;
	}

	void PanelResources::Initialize()
	{
		Widget::Initialize();
		m_browserSelectableList->RefreshItems();
	}
	void PanelResources::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		Widget::Draw(threadIndex);
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
		m_contentsSize = data.f[1];
		SetShowListContents(m_contentsSize < MIN_CONTENTS_SIZE + 0.5f);
	}

	void PanelResources::SetShowListContents(bool showList)
	{
		if (m_showListContents == showList)
			return;
		m_showListContents = showList;

		auto* bottom = m_contentsListOwner->GetChildren().back();
		m_contentsListOwner->RemoveChild(bottom);
		m_contentsListOwner->RemoveChild(m_contentsSelectableList);
		m_manager->Deallocate(m_contentsSelectableList);
		BuildSelectableListLayout(m_showListContents);
		m_contentsSelectableList->Initialize();
		m_contentsListOwner->AddChild(m_contentsSelectableList);
		m_contentsListOwner->AddChild(bottom);
		m_contentsSelectableList->RefreshItems();
	}

	Widget* PanelResources::BuildTopContents()
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
		topContents->AddChild(searchFieldTop);

		m_path = path;
		return topContents;
	}

	Widget* PanelResources::BuildBottomContents()
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
		sizeSlider->GetProps().value		  = &m_contentsSize;
		sizeSlider->GetProps().onValueChanged = [this](float val) -> void {
			if (val < MIN_CONTENTS_SIZE + 0.25f && !m_showListContents)
				SetShowListContents(true);
			else if (m_showListContents && val > MIN_CONTENTS_SIZE + 0.25f)
				SetShowListContents(false);

			if (!m_showListContents)
				m_contentsSelectableList->SetGridLayoutItemSize(Vector2(Theme::GetDef().baseItemHeight * m_contentsSize, Theme::GetDef().baseItemHeight * (m_contentsSize + 1)));
		};

		bottom->AddChild(sizeSlider);

		m_itemCount			= itemCount;
		m_selectedItemCount = itemSelected;

		return bottom;
	}

	Widget* PanelResources::BuildContents()
	{
		DirectionalLayout* contents	   = m_manager->Allocate<DirectionalLayout>("Contents");
		contents->GetProps().direction = DirectionOrientation::Vertical;
		contents->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		contents->SetAlignedPos(Vector2(0.25f, 0.0f));
		contents->SetAlignedSize(Vector2(0.75f, 1.0f));
		contents->GetChildMargins() = {.top = Theme::GetDef().baseIndent};
		AddChild(contents);

		contents->AddChild(BuildTopContents());

		Widget* filler = m_manager->Allocate<Widget>("Filler");
		filler->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		filler->SetAlignedPosX(0.0f);
		filler->SetAlignedSizeX(1.0f);
		filler->SetFixedSizeY(Theme::GetDef().baseItemHeight / 2);
		contents->AddChild(filler);

		contents->AddChild(BuildSelectableListLayout(m_showListContents));
		contents->AddChild(BuildBottomContents());
		m_contentsListOwner = contents;
		return contents;
	}

	SelectableListLayout* PanelResources::BuildSelectableListLayout(bool isList)
	{
		SelectableListLayout* selectableList = m_manager->Allocate<SelectableListLayout>("SelectableList");
		selectableList->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		selectableList->SetAlignedPosX(0.0f);
		selectableList->SetAlignedSize(Vector2(1.0f, 0.0f));
		selectableList->SetListener(this);
		selectableList->GetFileMenu()->SetListener(this);
		selectableList->GetProps().useGridAsLayout = !isList;
		m_contentsSelectableList				   = selectableList;
		return selectableList;
	}

	Widget* PanelResources::BuildBrowser()
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
		browser->AddChild(searchField);

		SelectableListLayout* selectableList = m_manager->Allocate<SelectableListLayout>("SelectableList");
		selectableList->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		selectableList->SetAlignedPosX(0.0f);
		selectableList->SetAlignedSize(Vector2(1.0f, 0.0f));
		selectableList->SetListener(this);
		selectableList->GetFileMenu()->SetListener(this);
		browser->AddChild(selectableList);

		m_browserSelectableList = selectableList;
		return browser;
	}

	bool PanelResources::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		if (sid == TO_SID(Locale::GetStr(LocaleStr::NewWorld)))
		{
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->SaveEmptyWorld(item->absolutePath + "/NewWorld.linaworld");
			return true;
		}
		return false;
	}

	void PanelResources::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		if (filemenu == m_contentsSelectableList->GetFileMenu())
		{
		}
		else if (filemenu == m_browserSelectableList->GetFileMenu())
		{
		}

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
	}

	void PanelResources::OnSelectableListFillItems(SelectableListLayout* list, Vector<SelectableListItem>& outItems, void* parentUserData)
	{

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
						.title			 = item->isDirectory ? item->folderName : item->fileName,
						.userData		 = item,
						.hasChildren	 = !item->children.empty(),
						.useDropdownIcon = true,
						.useFolderIcon	 = true,
					});
				}
				return;
			}

			DirectoryItem*	   root = m_editor->GetFileManager().GetRoot();
            if(root)
            {
                SelectableListItem item = {
                    .title             = root->folderName,
                    .userData         = root,
                    .hasChildren     = !root->children.empty(),
                    .useDropdownIcon = true,
                    .useFolderIcon     = true,
                };
                outItems.push_back(item);
            }
			
		}
		else if (list == m_contentsSelectableList)
		{
			if (m_currentBrowserSelection == nullptr)
				return;

			outItems.resize(m_currentBrowserSelection->children.size());

			for (size_t i = 0; i < m_currentBrowserSelection->children.size(); i++)
			{
				DirectoryItem* item = m_currentBrowserSelection->children[i];
				outItems[i]			= {
							.title				  = item->isDirectory ? item->folderName : item->fileName,
							.userData			  = item,
							.hasChildren		  = !item->children.empty(),
							.useDropdownIcon	  = false,
							.useFolderIcon		  = item->isDirectory,
							.useCustomInteraction = true,
							.customTexture		  = item->thumbnail,
				};
			}
		}
	}

	void PanelResources::OnSelectableListItemControl(SelectableListLayout* list, void* userData)
	{
		if (list == m_browserSelectableList)
		{
			m_currentBrowserSelection = userData == nullptr ? nullptr : static_cast<DirectoryItem*>(userData);
			m_contentsSelectableList->RefreshItems();

			m_path->GetProps().text = userData == nullptr ? "" : m_currentBrowserSelection->relativePath;
			m_path->CalculateTextSize();
		}
		else if (list == m_contentsSelectableList)
		{
			m_currentContentsSelection = userData == nullptr ? nullptr : static_cast<DirectoryItem*>(userData);
		}
	}

	void PanelResources::OnSelectableListPayloadDropped(SelectableListLayout* list, void* payloadUserData, void* droppedItemuserData)
	{
	}

	void PanelResources::OnSelectableListItemInteracted(SelectableListLayout* list, void* userData)
	{
		if (list == m_contentsSelectableList)
		{
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);

			if (item->isDirectory)
			{
				m_browserSelectableList->ChangeFold(item->parent, true);
				m_browserSelectableList->GrabControls(item);

				return;
			}
			else
			{
				if (item->tid == GetTypeID<EntityWorld>())
				{
					auto* wm = m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
					wm->InstallWorld(item->absolutePath);
					return;
				}
			}
		}
	}

	Widget* PanelResources::OnSelectableListBuildCustomTooltip(SelectableListLayout* list, void* userData)
	{
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
		img->GetProps().imageTexture						  = item->thumbnail;
		img->GetProps().fitImage							  = true;
		bg->AddChild(img);

		Text* txt = m_manager->Allocate<Text>();
		txt->GetFlags().Set(WF_POS_ALIGN_X);
		txt->SetAlignedPosX(0.5f);
		txt->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		txt->GetProps().text = FileSystem::RemoveExtensionFromPath(item->fileName);
		bg->AddChild(txt);
		bg->Initialize();

		return bg;
	}

	PayloadType PanelResources::OnSelectableListGetPayloadType(SelectableListLayout* list)
	{
		if (list == m_browserSelectableList)
			return PayloadType::BrowserSelectable;
		if (list == m_contentsSelectableList)
			return PayloadType::ContentsSelectable;
		return PayloadType::None;
	}

} // namespace Lina::Editor
