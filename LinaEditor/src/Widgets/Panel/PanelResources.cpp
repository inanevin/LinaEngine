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
#include "Core/GUI/Widgets/Compound/FoldingSelectable.hpp"
#include "Core/GUI/Widgets/Primitives/Selectable.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/GUI/Widgets/Layout/LayoutBorder.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina::Editor
{
	void PanelResources::Construct()
	{
		m_editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		DirectionalLayout* browser	  = m_manager->Allocate<DirectionalLayout>("Browser");
		browser->GetProps().direction = DirectionOrientation::Vertical;
		browser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		browser->SetAlignedPos(Vector2::Zero);
		browser->SetAlignedSize(Vector2(0.25f, 1.0f));
		browser->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
		browser->SetChildPadding(Theme::GetDef().baseIndent);
        browser->GetFlags().Set(WF_CONTROLS_MANAGER);
		AddChild(browser);

		InputField* searchField = m_manager->Allocate<InputField>("SearchField");
		searchField->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		searchField->SetAlignedPosX(0.0f);
		searchField->SetAlignedSizeX(1.0f);
		searchField->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		searchField->GetProps().rounding		= 0.0f;
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
		// searchField->GetProps().outlineThickness = 0.0f;
		browser->AddChild(searchField);

		DirectionalLayout* browserItems	   = m_manager->Allocate<DirectionalLayout>("BrowserItems");
		browserItems->GetProps().direction = DirectionOrientation::Vertical;
		browserItems->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X);
		browserItems->SetAlignedSize(Vector2(1.0f, 0.0f));
		browserItems->SetAlignedPosX(0.0f);
		browserItems->GetProps().backgroundStyle  = DirectionalLayout::BackgroundStyle::Default;
		browserItems->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		browserItems->GetProps().outlineThickness = 0.0f;
		browserItems->GetProps().rounding		  = 0.0f;
        browserItems->GetProps().clipChildren = true;
		browser->AddChild(browserItems);

		LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
		border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		border->SetAlignedPos(Vector2(0.25f, 0.0f));
		border->SetAlignedSizeY(1.0f);
		border->GetProps().orientation = DirectionOrientation::Vertical;
		AddChild(border);

		DirectionalLayout* contents = m_manager->Allocate<DirectionalLayout>("Contents");
        contents->GetProps().direction = DirectionOrientation::Vertical;
		contents->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		contents->SetAlignedPos(Vector2(0.25f, 0.0f));
		contents->SetAlignedSize(Vector2(0.75f, 1.0f));
        contents->GetChildMargins() = {.top = Theme::GetDef().baseIndent};
        contents->GetFlags().Set(WF_CONTROLS_MANAGER);
		AddChild(contents);
        
        DirectionalLayout* topContents = m_manager->Allocate<DirectionalLayout>("TopContents");
        topContents->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
        topContents->SetAlignedPos(0.0f);
        topContents->SetAlignedSizeX(1.0f);
        topContents->SetFixedSizeY(Theme::GetDef().baseItemHeight);
        topContents->SetChildPadding(Theme::GetDef().baseIndentInner);
        topContents->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
        contents->AddChild(topContents);
        
        Icon* folder = m_manager->Allocate<Icon>("PathIcon");
        folder->GetProps().icon = ICON_FOLDER;
        folder->GetFlags().Set(WF_POS_ALIGN_Y);
        folder->SetAlignedPosY(0.5f);
        folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        topContents->AddChild(folder);
        
        Icon* chev = m_manager->Allocate<Icon>("Path>");
        chev->GetProps().icon = ICON_CHEVRON_RIGHT;
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
        path->GetProps().isDynamic = true;
        topContents->AddChild(path);
        
        GridLayout* grid = m_manager->Allocate<GridLayout>("Grid");
        grid->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        grid->SetAlignedPosX(0.0f);
        grid->SetAlignedSize(Vector2(1.0f, 0.0f));
        grid->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
        grid->GetProps().horizontalPadding = Theme::GetDef().baseIndent;
        grid->GetProps().verticalPadding = Theme::GetDef().baseIndent;
        contents->AddChild(grid);
        
        DirectionalLayout* bottom = m_manager->Allocate<DirectionalLayout>("BotContents");
        bottom->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
        bottom->SetAlignedPos(0.0f);
        bottom->SetAlignedSizeX(1.0f);
        bottom->SetFixedSizeY(Theme::GetDef().baseItemHeight);
        bottom->SetChildPadding(Theme::GetDef().baseIndentInner);
        bottom->GetChildMargins() = {.left = Theme::GetDef().baseIndentInner, .right = Theme::GetDef().baseIndent};
        bottom->GetProps().backgroundStyle = DirectionalLayout::BackgroundStyle::Default;
        bottom->GetProps().colorBackgroundStart = bottom->GetProps().colorBackgroundEnd = Theme::GetDef().background3;
        contents->AddChild(bottom);
        
        Text* itemCount = m_manager->Allocate<Text>("ItemCount");
        itemCount->GetFlags().Set(WF_POS_ALIGN_Y);
        itemCount->SetAlignedPosY(0.5f);
        itemCount->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        itemCount->GetProps().fetchCustomClipFromSelf = true;
        itemCount->GetProps().isDynamic = true;
        itemCount->GetProps().text = "0 " + Locale::GetStr(LocaleStr::Items);
        bottom->AddChild(itemCount);
        
        ShapeRect* divider = m_manager->Allocate<ShapeRect>("Divider");
        divider->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
        divider->SetAlignedPosY(0.0f);
        divider->SetAlignedSizeY(1.0f);
        divider->SetFixedSizeX(2.0f);
        divider->GetProps().colorStart = divider->GetProps().colorEnd = Theme::GetDef().background1;
        bottom->AddChild(divider);
        
        Text* itemSelected = m_manager->Allocate<Text>("ItemSelected");
        itemSelected->GetFlags().Set(WF_POS_ALIGN_Y);
        itemSelected->SetAlignedPosY(0.5f);
        itemSelected->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        itemSelected->GetProps().fetchCustomClipFromSelf = true;
        itemSelected->GetProps().isDynamic = true;
        itemSelected->GetProps().text = "0 " + Locale::GetStr(LocaleStr::Selected);
        bottom->AddChild(itemSelected);
        
        ShapeRect* divider2 = m_manager->Allocate<ShapeRect>("Divider");
        divider2->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
        divider2->SetAlignedPosY(0.0f);
        divider2->SetAlignedSizeY(1.0f);
        divider2->SetFixedSizeX(2.0f);
        divider2->GetProps().colorStart = divider2->GetProps().colorEnd = Theme::GetDef().background1;
        bottom->AddChild(divider2);
        
        border->AssignSides(browser, contents);
		m_border	   = border;
		m_browserItems = browserItems;
        m_path = path;
        m_itemCount = itemCount;
        m_selectedItemCount = itemSelected;
        m_contentsGrid = grid;
        
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
		return {.data0 = m_border->GetAlignedPosX()};
	}

	void PanelResources::SetExtraLayoutData(const PanelLayoutExtra& data)
	{
		m_border->SetAlignedPosX(data.data0);
		m_border->GetNegative()->SetAlignedSizeX(data.data0);
		m_border->GetPositive()->SetAlignedPosX(data.data0);
		m_border->GetPositive()->SetAlignedSizeX(1.0f - data.data0);
	}

	void PanelResources::RefreshBrowserHierarchy()
	{
		for (auto* c : m_browserItems->GetChildren())
			m_manager->Deallocate(c);
        
		m_browserItems->RemoveAllChildren();

		uint8		   level = 0;
		DirectoryItem* root	 = m_editor->GetFileManager().GetRoot();
        m_browserItems->AddChild(CreateSelectable(root,0));
	}

    void PanelResources::RefreshContents()
    {
        auto* rm = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
        
        m_path->GetProps().text = m_currentSelectedDirectory->relativePath;
        m_path->CalculateTextSize();
        
        m_itemCount->GetProps().text = TO_STRING(m_currentSelectedDirectory->children.size()) + " " + Locale::GetStr(LocaleStr::Items);
        m_itemCount->CalculateTextSize();
        
        linatl::sort(m_currentSelectedDirectory->children.begin(), m_currentSelectedDirectory->children.end(), [](DirectoryItem* it, DirectoryItem* it2) -> bool { return it->tid < it2->tid;});
        
        for(auto* c : m_contentsGrid->GetChildren())
            m_manager->Deallocate(c);
        
        m_contentsGrid->RemoveAllChildren();
        
        for(auto* item : m_currentSelectedDirectory->children)
        {
            DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Item");
            layout->GetProps().direction = DirectionOrientation::Vertical;
            layout->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
            layout->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
            layout->SetChildPadding(Theme::GetDef().baseIndentInner);
            layout->SetFixedSizeY(layout->GetFixedSizeX() + Theme::GetDef().baseItemHeight + Theme::GetDef().baseIndentInner);

            Selectable* base = m_manager->Allocate<Selectable>("Base");
            base->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
            base->SetAlignedPosX(0.0f);
            base->SetAlignedSizeX(1.0f);
            base->SetFixedSizeY(layout->GetFixedSizeX());
            base->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndentInner);
            layout->AddChild(base);
            
            if(base->GetProps().colorStart.x > 0.1f)
            {
                
            }
            
            if(item->isDirectory)
            {
                Icon* folder = m_manager->Allocate<Icon>("Folder");
                folder->GetProps().icon = ICON_FOLDER;
                folder->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
                folder->SetAlignedPos(Vector2::One * 0.5f);
                folder->SetPosAlignmentSourceX(PosAlignmentSource::Center);
                folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
                folder->GetProps().dynamicSizeToParent = true;
                folder->GetProps().dynamicSizeScale = 0.9f;
                base->AddChild(folder);
            }
            else
            {
                base->SetBuildCustomTooltip([this, item]() -> Widget* {
                    const Vector2 size = Vector2(RESOURCE_THUMBNAIL_SIZE * 1.2f, RESOURCE_THUMBNAIL_SIZE * 1.2f);
                    DirectionalLayout* bg = m_manager->Allocate<DirectionalLayout>();
                    bg->GetProps().direction = DirectionOrientation::Vertical;
                    bg->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
                    bg->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
                    bg->GetProps().backgroundStyle = DirectionalLayout::BackgroundStyle::Default;
                    bg->GetProps().colorBackgroundEnd = bg->GetProps().colorBackgroundStart = Theme::GetDef().background0;
                    bg->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
                    bg->GetProps().colorOutline = Theme::GetDef().background0;
                    bg->SetFixedSize(size);
                    ShapeRect* img = m_manager->Allocate<ShapeRect>();
                    img->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
                    img->SetAlignedPosX(0.0f);
                    img->SetAlignedSize(Vector2(1.0f, 1.0f));
                    img->GetProps().colorStart = img->GetProps().colorEnd = Color::White;
                    img->GetProps().imageTexture = item->thumbnail;
                    img->GetProps().fitImage = true;
                    bg->AddChild(img);
                    return bg;
                });
            
                base->GetProps().colorEnd = base->GetProps().colorStart = Theme::GetDef().background0;
                base->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
                base->GetProps().rounding = Theme::GetDef().baseRounding;
                base->GetProps().colorOutline = Theme::GetDef().black;
                
                if((item->tid == GetTypeID<Texture>() || item->tid == GetTypeID<Font>()) && item->thumbnail)
                {
                    ShapeRect* img = m_manager->Allocate<ShapeRect>("Thumb");
                    img->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
                    img->SetAlignedPos(Vector2::Zero);
                    img->SetAlignedSize(Vector2::One);
                    img->GetProps().imageTexture = item->thumbnail;
                    img->GetProps().fitImage = true;
                    img->GetProps().colorStart = Color(1,1,1,1);
                    base->AddChild(img);
                }
                else if(item->tid == GetTypeID<Font>())
                {
                   
                }
            }
            
            
            InputField* inpField = m_manager->Allocate<InputField>("TitleField");
            inpField->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
            inpField->SetAlignedPosX(0.0f);
            inpField->SetAlignedSizeX(1.0f);
            inpField->SetFixedSizeY(Theme::GetDef().baseItemHeight);
            inpField->GetText()->GetProps().text = item->isDirectory ? item->folderName : FileSystem::RemoveExtensionFromPath(item->fileName);
            inpField->GetProps().rounding = inpField->GetProps().outlineThickness = 0.0f;
            inpField->GetProps().centerText = true;
            inpField->GetProps().colorBackground = Color(0.0f, 0.0f, 0.0f, 0.0f);
            layout->AddChild(inpField);
            
            layout->Initialize();
            m_contentsGrid->AddChild(layout);
        }
    }
    
	FoldingSelectable* PanelResources::CreateSelectable(DirectoryItem* item, uint8 level)
	{
		FoldingSelectable* folding = m_manager->Allocate<FoldingSelectable>("Folding");
		folding->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		folding->SetAlignedPosX(0.0f);
		folding->SetAlignedSizeX(1.0f);
		folding->GetProps().level		= level;
		folding->GetProps().height		= Theme::GetDef().baseItemHeight;

		DirectionalLayout* layout = folding->GetLayout();
		layout->SetChildPadding(Theme::GetDef().baseIndentInner);
        layout->GetChildMargins().left = Theme::GetDef().baseIndent * (level + 1);

		Icon* dropdown				   = m_manager->Allocate<Icon>("Dropdown");
		dropdown->GetProps().icon	   = ICON_CHEVRON_RIGHT;
		dropdown->GetProps().textScale = 0.3f;
		dropdown->GetFlags().Set(WF_POS_ALIGN_Y);
		dropdown->SetAlignedPosY(0.5f);
		dropdown->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		dropdown->GetProps().onClicked = [folding, dropdown]() {
			folding->ChangeFold(!folding->GetIsFolded());
			dropdown->GetProps().icon = folding->GetIsFolded() ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
		};
		layout->AddChild(dropdown);
        
        bool containsFolderChild = false;
        for(auto* c : item->children)
        {
            if(c->isDirectory)
            {
                containsFolderChild = true;
                break;
            }
        }
        
        if(!containsFolderChild)
            dropdown->SetVisible(false);
                
        Icon* folder            = m_manager->Allocate<Icon>("Folder");
        folder->GetProps().icon = ICON_FOLDER;
        folder->GetFlags().Set(WF_POS_ALIGN_Y);
        folder->SetAlignedPosY(0.5f);
        folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        layout->AddChild(folder);
	
		Text* title = m_manager->Allocate<Text>("Title");
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        title->GetProps().text = item->folderName;
		layout->AddChild(title);

        folding->GetProps().onFoldChanged = [this, item, dropdown, folding](bool folded) {
            dropdown->GetProps().icon = folded ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
            
            if(folded)
            {
                DirectionalLayout* layout = folding->GetLayout();
                
                for(auto* c : folding->GetChildren())
                {
                    if(c != layout)
                        m_manager->Deallocate(c);
                }
                
                folding->RemoveAllChildren();
                folding->AddChild(layout);
            }
            else
            {
                for(auto* c : item->children)
                {
                    if(!c->isDirectory)
                        continue;
                    
                    auto* selectable = CreateSelectable(c, folding->GetProps().level + 1);
                    folding->AddChild(selectable);
                    selectable->Initialize();
                }
            }
            
        };

		folding->GetProps().owner = m_browserItems;
        folding->GetProps().userData = static_cast<void*>(item);
        
        folding->GetProps().onSelectedChanged = [folding, this](bool selected){
            if(selected)
            {
                m_currentSelectedDirectory = static_cast<DirectoryItem*>(folding->GetProps().userData);
                RefreshContents();
            }
        };

		return folding;
	}
} // namespace Lina::Editor
