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

		FileMenu* createMenu = m_manager->Allocate<FileMenu>("CreateMenu");
		createMenu->SetListener(this);
		AddChild(createMenu);

		Widget* browser = BuildBrowser();
		AddChild(browser);

		LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
		border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		border->SetAlignedPos(Vector2(0.25f, 0.0f));
		border->SetAlignedSizeY(1.0f);
		border->GetProps().orientation = DirectionOrientation::Vertical;
		AddChild(border);

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

		ScrollArea* scroller = m_manager->Allocate<ScrollArea>("ScrollArea");
		scroller->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroller->SetAlignedPosX(0.0f);
		scroller->SetAlignedSize(Vector2(1.0f, 0.0f));
		scroller->GetProps().direction = DirectionOrientation::Vertical;
		contents->AddChild(scroller);

		GridLayout* grid = m_manager->Allocate<GridLayout>("Grid");
		grid->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		grid->SetAlignedPos(Vector2::Zero);
		grid->SetAlignedSize(Vector2::One);
		grid->GetProps().clipChildren = true;
		grid->GetProps().background	  = GridLayout::BackgroundStyle::Default;
		grid->GetChildMargins()		  = TBLR::Eq(Theme::GetDef().baseIndent);

		scroller->AddChild(grid);
		contents->AddChild(BuildBottomContents());
		border->AssignSides(browser, contents);

		m_border		 = border;
		m_contentsGrid	 = grid;
		m_contentsScroll = scroller;
		m_contextMenu	 = createMenu;
		RefreshBrowserHierarchy();
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
		m_contentsSize	   = data.f[1];
		m_showListContents = m_contentsSize < MIN_CONTENTS_SIZE + 0.5f;
	}

	void PanelResources::RefreshBrowserHierarchy()
	{
		for (auto* c : m_browserItems->GetChildren())
			m_manager->Deallocate(c);

		m_browserItems->RemoveAllChildren();

		uint8		   level = 0;
		DirectoryItem* root	 = m_editor->GetFileManager().GetRoot();
		m_browserItems->AddChild(BuildBrowserSelectable(root, 0));
	}

	void PanelResources::RefreshContents()
	{
		if (m_currentBrowserSelection.size() != 1)
		{
			return;
		}

		m_currentContentsSelection.clear();

		auto* targetDirectory = m_currentBrowserSelection[0];

		if (m_showListContents)
		{
			m_contentsGrid->GetProps().horizontalPadding = Theme::GetDef().baseIndentInner / 4;
			m_contentsGrid->SetChildPadding(0.0f);
		}
		else
		{
			m_contentsGrid->GetProps().horizontalPadding = Theme::GetDef().baseIndent;
			m_contentsGrid->SetChildPadding(Theme::GetDef().baseIndent);
		}

		m_path->GetProps().text = targetDirectory->relativePath;
		m_path->CalculateTextSize();

		m_itemCount->GetProps().text = TO_STRING(targetDirectory->children.size()) + " " + Locale::GetStr(LocaleStr::Items);
		m_itemCount->CalculateTextSize();

		for (auto* c : m_contentsGrid->GetChildren())
			m_manager->Deallocate(c);

		m_contentsGrid->RemoveAllChildren();

		if (targetDirectory->children.empty())
		{
			Text* nothingToSee = m_manager->Allocate<Text>("NothingToSee");
			nothingToSee->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			nothingToSee->SetAlignedPos(Vector2::One * 0.5f);
			nothingToSee->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			nothingToSee->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			nothingToSee->GetProps().color = Theme::GetDef().foreground1;
			nothingToSee->GetProps().text  = Locale::GetStrUnicode(LocaleStr::NothingInDirectory);
			nothingToSee->Initialize();
			m_contentsGrid->AddChild(nothingToSee);
			return;
		}

		for (auto* item : targetDirectory->children)
		{
			if (m_showListContents)
			{
				Widget* widget = BuildContentSelectableList(item);
				widget->Initialize();
				m_contentsGrid->AddChild(widget);
				continue;
			}

			Widget* widget = BuildContentSelectableBig(item);
			widget->Initialize();
			m_contentsGrid->AddChild(widget);
		}
	}

	void PanelResources::UpdateWidgetSizeFromContentsSize(Widget* w)
	{
		w->SetChildPadding(Theme::GetDef().baseIndentInner);

		if (m_showListContents)
		{
			w->SetAlignedSizeX(1.0f);
			w->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			return;
		}

		const float x = Theme::GetDef().baseItemHeight * m_contentsSize;
		w->SetFixedSizeX(x);
		w->SetFixedSizeY(x + Theme::GetDef().baseItemHeight + Theme::GetDef().baseIndentInner);
	}

	Widget* PanelResources::BuildThumbnailForItem(DirectoryItem* item)
	{
		if ((item->tid == GetTypeID<Texture>() || item->tid == GetTypeID<Font>()) && item->thumbnail)
		{
			ShapeRect* img = m_manager->Allocate<ShapeRect>("Thumb");
			img->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			img->SetAlignedPos(Vector2::Zero);
			img->SetAlignedSize(Vector2::One);
			img->GetProps().imageTexture = item->thumbnail;
			img->GetProps().fitImage	 = true;
			img->GetProps().colorStart	 = Color(1, 1, 1, 1);
			return img;
		}

		return nullptr;
	}

	Widget* PanelResources::BuildTooltipForItem(void* userData)
	{
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

	Widget* PanelResources::BuildTitleForItem(DirectoryItem* item)
	{
		Text* resName = WidgetUtility::BuildEditableText(this, m_showListContents, []() {});

		if (m_showListContents)
		{
			resName->GetFlags().Set(WF_POS_ALIGN_Y);
			resName->SetAlignedPosY(0.5f);
			resName->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		}
		else
		{
			resName->GetFlags().Set(WF_POS_ALIGN_X);
			resName->SetAlignedPosX(0.5f);
			resName->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		}
		resName->GetProps().text				= item->isDirectory ? item->folderName : FileSystem::RemoveExtensionFromPath(item->fileName);
		resName->GetProps().fetchWrapFromParent = true;
		resName->GetProps().wordWrap			= false;

		return resName;
	}

	Widget* PanelResources::BuildFolderIconForItem(DirectoryItem* item, float dynSize)
	{
		Icon* folder			= m_manager->Allocate<Icon>("Folder");
		folder->GetProps().icon = ICON_FOLDER;

		if (m_showListContents)
		{
			folder->GetFlags().Set(WF_POS_ALIGN_Y);
			folder->SetAlignedPosY(0.5f);
			folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		}
		else
		{
			folder->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			folder->SetAlignedPos(Vector2::One * 0.5f);
			folder->SetPosAlignmentSourceX(PosAlignmentSource::Center);
			folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		}

		folder->GetProps().dynamicSizeToParent = true;
		folder->GetProps().dynamicSizeScale	   = dynSize;

		return folder;
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
			{
				m_showListContents = true;
				RefreshContents();
			}
			else if (m_showListContents && val > MIN_CONTENTS_SIZE + 0.25f)
			{
				m_showListContents = false;
				RefreshContents();
			}

			if (!m_showListContents)
			{
				for (auto* c : m_contentsGrid->GetChildren())
					UpdateWidgetSizeFromContentsSize(c);
			}
		};

		bottom->AddChild(sizeSlider);

		m_itemCount			= itemCount;
		m_selectedItemCount = itemSelected;

		return bottom;
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

		InputField* searchField = m_manager->Allocate<InputField>("SearchField");
		searchField->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		searchField->SetAlignedPosX(0.0f);
		searchField->SetAlignedSizeX(1.0f);
		searchField->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		searchField->GetProps().rounding		= 0.0f;
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
		browser->AddChild(searchField);

		ScrollArea* scroller = m_manager->Allocate<ScrollArea>("ScrollArea");
		scroller->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroller->SetAlignedPosX(0.0f);
		scroller->SetAlignedSize(Vector2(1.0f, 0.0f));
		scroller->GetProps().direction = DirectionOrientation::Vertical;
		browser->AddChild(scroller);

		DirectionalLayout* browserItems	   = m_manager->Allocate<DirectionalLayout>("BrowserItems");
		browserItems->GetProps().direction = DirectionOrientation::Vertical;
		browserItems->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		browserItems->SetAlignedSize(Vector2::One);
		browserItems->SetAlignedPos(Vector2::Zero);
		browserItems->GetProps().backgroundStyle  = DirectionalLayout::BackgroundStyle::Default;
		browserItems->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		browserItems->GetProps().outlineThickness = 0.0f;
		browserItems->GetProps().rounding		  = 0.0f;
		browserItems->GetProps().clipChildren	  = true;
		scroller->AddChild(browserItems);

		m_browserItems	= browserItems;
		m_browserScroll = scroller;

		return browser;
	}

	Widget* PanelResources::BuildBrowserSelectable(DirectoryItem* item, uint8 level)
	{
		bool containsFolderChild = false;
		for (auto* c : item->children)
		{
			if (c->isDirectory)
			{
				containsFolderChild = true;
				break;
			}
		}

		FoldLayout* fold = m_manager->Allocate<FoldLayout>("Fold");
		fold->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		fold->SetAlignedPosX(0.0f);
		fold->SetAlignedSizeX(1.0f);
		fold->SetDebugName(item->folderName);

		Selectable* selectable = m_manager->Allocate<Selectable>(item->folderName);
		selectable->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		selectable->SetAlignedPosX(0.0f);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		selectable->SetLocalControlsManager(m_browserItems);
		selectable->SetUserData(item);
		selectable->GetProps().onRightClick = BIND(&PanelResources::OnSelectableRightClick, this, std::placeholders::_1);
		selectable->GetProps().onInteracted = [fold](Selectable*) { fold->SetIsUnfolded(!fold->GetIsUnfolded()); };
		selectable->SetOnGrabbedControls([item, this]() {
			m_currentBrowserSelection.clear();
			m_currentBrowserSelection.push_back(item);
			RefreshContents();
		});

		fold->AddChild(selectable);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
		layout->GetChildMargins().left = Theme::GetDef().baseIndent + Theme::GetDef().baseIndent * level;
		selectable->AddChild(layout);

		Icon* dropdown				   = m_manager->Allocate<Icon>("Dropdown");
		dropdown->GetProps().icon	   = ICON_CHEVRON_RIGHT;
		dropdown->GetProps().textScale = 0.3f;
		dropdown->GetFlags().Set(WF_POS_ALIGN_Y);
		dropdown->SetAlignedPosY(0.5f);
		dropdown->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		dropdown->GetProps().onClicked = [fold]() { fold->SetIsUnfolded(!fold->GetIsUnfolded()); };
		layout->AddChild(dropdown);

		if (!containsFolderChild)
			dropdown->SetVisible(false);

		Icon* folder			= m_manager->Allocate<Icon>("Folder");
		folder->GetProps().icon = ICON_FOLDER;
		folder->GetFlags().Set(WF_POS_ALIGN_Y);
		folder->SetAlignedPosY(0.5f);
		folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		layout->AddChild(folder);

		Text* title = WidgetUtility::BuildEditableText(this, true, []() {});
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		title->GetProps().text = item->folderName;
		layout->AddChild(title);

		fold->GetProps().onFoldChanged = [dropdown, fold, item, level, this](bool unfolded) {
			dropdown->GetProps().icon = !unfolded ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
			dropdown->CalculateIconSize();

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
				for (auto* c : item->children)
				{
					if (!c->isDirectory)
						continue;
					auto* childSelectable = BuildBrowserSelectable(c, level + 1);
					fold->AddChild(childSelectable);
					childSelectable->Initialize();
				}
			}
		};

		return fold;
	}

	Widget* PanelResources::BuildContentSelectableList(DirectoryItem* item)
	{
		Selectable* selectable = m_manager->Allocate<Selectable>("Item");
		selectable->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		selectable->SetAlignedPosX(0.0f);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		selectable->SetUserData(item);
		selectable->GetProps().onInteracted		  = BIND(&PanelResources::OnSelectableInteracted, this, std::placeholders::_1);
		selectable->GetProps().onSelectionChanged = BIND(&PanelResources::OnSelectionChanged, this, std::placeholders::_1, std::placeholders::_2);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
		selectable->AddChild(layout);

		if (item->isDirectory)
			layout->AddChild(BuildFolderIconForItem(item, 0.9f));
		else
		{
			Widget* wrap = m_manager->Allocate<Widget>("Wrap");
			wrap->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_POS_ALIGN_Y);
			wrap->SetAlignedSizeY(0.9f);
			wrap->SetAlignedPosY(0.5f);
			wrap->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(wrap);

			Widget* thumbnail = BuildThumbnailForItem(item);
			if (thumbnail)
			{
				thumbnail->SetCustomTooltipUserData(item);
				thumbnail->SetBuildCustomTooltip(BIND(&PanelResources::BuildTooltipForItem, this, std::placeholders::_1));
				wrap->AddChild(thumbnail);
			}
		}

		layout->AddChild(BuildTitleForItem(item));
		return selectable;
	}

	Widget* PanelResources::BuildContentSelectableBig(DirectoryItem* item)
	{
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Item");
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		UpdateWidgetSizeFromContentsSize(layout);

		Selectable* selectable = m_manager->Allocate<Selectable>("Selectable");
		selectable->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_Y_COPY_X);
		selectable->SetAlignedPosX(0.0f);
		selectable->SetAlignedSizeX(1.0f);
		selectable->SetUserData(item);
		selectable->GetChildMargins()			  = TBLR::Eq(Theme::GetDef().baseIndentInner);
		selectable->GetProps().rounding			  = Theme::GetDef().baseRounding;
		selectable->GetProps().onInteracted		  = BIND(&PanelResources::OnSelectableInteracted, this, std::placeholders::_1);
		selectable->GetProps().onSelectionChanged = BIND(&PanelResources::OnSelectionChanged, this, std::placeholders::_1, std::placeholders::_2);
		layout->AddChild(selectable);

		if (item->isDirectory)
			selectable->AddChild(BuildFolderIconForItem(item, 0.75f));
		else
		{
			selectable->SetCustomTooltipUserData(item);
			selectable->SetBuildCustomTooltip(BIND(&PanelResources::BuildTooltipForItem, this, std::placeholders::_1));
			selectable->GetProps().colorEnd = selectable->GetProps().colorStart = Theme::GetDef().background0;
			auto* thumbnail														= BuildThumbnailForItem(item);
			if (thumbnail)
				selectable->AddChild(thumbnail);
		}

		layout->AddChild(BuildTitleForItem(item));
		return layout;
	}

	Widget* PanelResources::FindBrowserSelectable(DirectoryItem* item)
	{
		Vector<Selectable*> selectables;
		Widget::GetWidgetsOfType(selectables, m_browserItems);

		for (auto* s : selectables)
		{
			if (s->GetUserData() == static_cast<void*>(item))
				return s;
		}

		return nullptr;
	}

	void PanelResources::OpenFile(DirectoryItem* item)
	{
		if (item->tid == GetTypeID<EntityWorld>())
		{
			auto* wm = m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
			wm->LoadWorld(item->absolutePath);
			return;
		}
	}

	bool PanelResources::OnFileMenuItemClicked(StringID sid, void* userData)
	{
		if (sid == TO_SID(Locale::GetStr(LocaleStr::NewWorld)))
		{
			DirectoryItem* item = static_cast<DirectoryItem*>(userData);
			m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->CreateAndSaveNewWorld(item->absolutePath + "/NewWorld.linaworld");
			return true;
		}
		return false;
	}

	void PanelResources::OnGetFileMenuItems(StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		if (sid == "FolderContext"_hs)
		{
			outData = {

				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Create), .dropdownIcon = ICON_CHEVRON_RIGHT, .hasDropdown = true, .userData = userData},
				// FileMenuItem::Data{.isDivider = true},
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

	void PanelResources::OnSelectableInteracted(Selectable* selectable)
	{
		DirectoryItem* item = static_cast<DirectoryItem*>(selectable->GetUserData());

		if (item->isDirectory)
		{
			Widget* parent = FindBrowserSelectable(item->parent);
			static_cast<FoldLayout*>(parent->GetParent())->SetIsUnfolded(true);

			Widget* selectable = FindBrowserSelectable(item);

			if (selectable)
				m_manager->GrabControls(selectable);
			return;
		}
		else
		{
			OpenFile(item);
		}
	}

	void PanelResources::OnSelectionChanged(Selectable* selectable, bool selected)
	{
		DirectoryItem* item = static_cast<DirectoryItem*>(selectable->GetUserData());

		if (selected)
			m_currentContentsSelection.push_back(item);
		else
		{
			auto it = linatl::find_if(m_currentContentsSelection.begin(), m_currentContentsSelection.end(), [item](DirectoryItem* it) -> bool { return item == it; });
			if (it != m_currentContentsSelection.end())
				m_currentContentsSelection.erase(it);
		}

		m_selectedItemCount->GetProps().text = TO_STRING(m_currentContentsSelection.size()) + " " + Locale::GetStr(LocaleStr::Selected);
		m_selectedItemCount->CalculateTextSize();
	}

	void PanelResources::OnSelectableRightClick(Selectable* selectable)
	{
		DirectoryItem* item = static_cast<DirectoryItem*>(selectable->GetUserData());
		m_contextMenu->CreateItems("FolderContext"_hs, m_lgxWindow->GetMousePosition(), static_cast<void*>(item));
	}
} // namespace Lina::Editor
