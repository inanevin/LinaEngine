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
#include "Editor/EditorLocale.hpp"
#include "Editor/IO/FileManager.hpp"
#include "Editor/Widgets/Compound/FoldingSelectable.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/LayoutBorder.hpp"

namespace Lina::Editor
{
	void PanelResources::Construct()
	{
		m_editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		DirectionalLayout* browser = m_manager->Allocate<DirectionalLayout>("Browser");
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
        searchField->GetProps().rounding = 0.0f;
        searchField->GetProps().usePlaceHolder = true;
        searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search) + "...";
        //searchField->GetProps().outlineThickness = 0.0f;
        browser->AddChild(searchField);
        
        DirectionalLayout* browserItems = m_manager->Allocate<DirectionalLayout>("BrowserItems");
        browserItems->GetProps().direction = DirectionOrientation::Vertical;
        browserItems->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X);
        browserItems->SetAlignedSize(Vector2(1.0f, 0.0f));
        browserItems->SetAlignedPosX(0.0f);
        browserItems->GetProps().backgroundStyle = DirectionalLayout::BackgroundStyle::Default;
        browserItems->GetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
        browserItems->GetProps().outlineThickness = 0.0f;
        browserItems->GetProps().rounding = 0.0f;
        browser->AddChild(browserItems);
        
        LayoutBorder* border = m_manager->Allocate<LayoutBorder>("Border");
        border->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
        border->SetAlignedPos(Vector2(0.25f, 0.0f));
        border->SetAlignedSizeY(1.0f);
        border->GetProps().orientation = DirectionOrientation::Vertical;
        AddChild(border);
        
        Widget* contents = m_manager->Allocate<Widget>("Contents");
        contents->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        contents->SetAlignedPos(Vector2(0.25f, 0.0f));
        contents->SetAlignedSize(Vector2(0.75f, 1.0f));
        contents->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
        contents->SetChildPadding(Theme::GetDef().baseIndent);
        AddChild(contents);
        
        border->AssignSides(browser, contents);
        m_border = border;
        m_browserItems = browserItems;

       // RefreshBrowserHierarchy();
	}

	void PanelResources::Draw(int32 threadIndex)
	{
        LinaVG::StyleOptions opts;
        opts.color           = Theme::GetDef().background1.AsLVG4();
        LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
        Widget::Draw(threadIndex);
	}

    PanelLayoutExtra PanelResources::GetExtraLayoutData()
    {
        return {.data0 = m_border->GetAlignedPosX()};
    }

    void PanelResources::SetExtraLayoutData(const PanelLayoutExtra &data)
    {
        m_border->SetAlignedPosX(data.data0);
        m_border->GetNegative()->SetAlignedSizeX(data.data0);
        m_border->GetPositive()->SetAlignedPosX(data.data0);
        m_border->GetPositive()->SetAlignedSizeX(1.0f - data.data0);
    }

    void PanelResources::AddSelectable(DirectoryItem* item, FoldingSelectable* parent, uint8& level)
    {
        FoldingSelectable* selectable = CreateSelectable(level, item->path);
        
        if(parent == nullptr)
            m_browserItems->AddChild(selectable);
        else
            parent->AddChild(selectable);
        
        if(!item->children.empty())
            level++;
        
        for(auto* c : item->children)
            AddSelectable(c, selectable, level);
    }

    void PanelResources::RefreshBrowserHierarchy()
    {
        for(auto* c : m_browserItems->GetChildren())
            m_manager->Deallocate(c);
        m_browserItems->RemoveAllChildren();
        
        
        uint8 level = 0;
        DirectoryItem* root = m_editor->GetFileManager().GetRoot();
        AddSelectable(root, nullptr, level);
    }

    FoldingSelectable* PanelResources::CreateSelectable(uint8 level, const String &text)
    {
        FoldingSelectable* folding = m_manager->Allocate<FoldingSelectable>("Folding");
        folding->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
        folding->SetAlignedPosX(0.0f);
        folding->SetAlignedSizeX(1.0f);
        folding->GetProps().level = level;
        folding->GetProps().height = Theme::GetDef().baseItemHeight;
        folding->GetChildMargins().left = Theme::GetDef().baseIndent;
        folding->SetChildPadding(Theme::GetDef().baseIndentInner);
       
        DirectionalLayout* layout = folding->GetLayout();
        layout->SetChildPadding(folding->GetChildPadding());
        
        Icon* dropdown = m_manager->Allocate<Icon>("Dropdown");
        dropdown->GetProps().icon = ICON_CHEVRON_RIGHT;
        dropdown->GetProps().textScale = 0.3f;
        dropdown->GetFlags().Set(WF_POS_ALIGN_Y);
        dropdown->SetAlignedPosY(0.5f);
        dropdown->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        dropdown->GetProps().onClicked = [folding, dropdown](){
            folding->ChangeFold(!folding->GetIsFolded());
            
            dropdown->GetProps().icon = folding->GetIsFolded() ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
        };
        layout->AddChild(dropdown);
        
        Icon* folder = m_manager->Allocate<Icon>("Folder");
        folder->GetProps().icon = ICON_FOLDER;
        folder->GetFlags().Set(WF_POS_ALIGN_Y);
        folder->SetAlignedPosY(0.5f);
        folder->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        layout->AddChild(folder);
        
        Text* title = m_manager->Allocate<Text>("Title");
        title->GetFlags().Set(WF_POS_ALIGN_Y);
        title->SetAlignedPosY(0.5f);
        title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        title->GetProps().text = text;
        layout->AddChild(title);
        
        folding->GetProps().onFoldChanged = [dropdown, folding](bool folded){
            dropdown->GetProps().icon = folding->GetIsFolded() ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
        };
        
        
        folding->GetProps().owner = m_browserItems;
        
        return folding;
    }
} // namespace Lina::Editor
