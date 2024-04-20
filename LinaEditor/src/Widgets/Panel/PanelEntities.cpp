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

#include "Editor/Widgets/Panel/PanelEntities.hpp"
#include "Editor/Widgets/Testbed.hpp"
#include "Editor/EditorLocale.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Selectable.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"

namespace Lina::Editor
{
	void PanelEntities::Construct()
	{
        m_worldManager = m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
        m_editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);
        m_editor->AddPayloadListener(this);
        
        FileMenu* contextMenu = m_manager->Allocate<FileMenu>();
        contextMenu->SetListener(this);
        AddChild(contextMenu);
        
        DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
        layout->GetProps().direction = DirectionOrientation::Vertical;
        layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        layout->SetAlignedPos(Vector2::Zero);
        layout->SetAlignedSize(Vector2::One);
        layout->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
        layout->SetChildPadding(Theme::GetDef().baseIndent);
        AddChild(layout);
        
        InputField* search = m_manager->Allocate<InputField>("Search");
        search->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
        search->SetAlignedPosX(0.0f);
        search->SetAlignedSizeX(1.0f);
        search->SetFixedSizeY(Theme::GetDef().baseItemHeight);
        search->GetProps().usePlaceHolder = true;
        search->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
        search->GetProps().rounding = 0.0f;
        layout->AddChild(search);
        
        ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
        scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        scroll->SetAlignedPosX(0.0f);
        scroll->SetAlignedSize(Vector2(1.0f, 0.0f));
        scroll->GetProps().direction = DirectionOrientation::Vertical;
        layout->AddChild(scroll);
        
        DirectionalLayout* entitiesLayout = m_manager->Allocate<DirectionalLayout>("EntitiesLayout");
        entitiesLayout->GetProps().direction = DirectionOrientation::Vertical;
        entitiesLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        entitiesLayout->SetAlignedPos(Vector2::Zero);
        entitiesLayout->SetAlignedSize(Vector2::One);
        entitiesLayout->GetProps().backgroundStyle = DirectionalLayout::BackgroundStyle::Default;
        entitiesLayout->GetProps().colorBackgroundStart = entitiesLayout->GetProps().colorBackgroundEnd = Theme::GetDef().background0;
        entitiesLayout->GetProps().onRightClicked = [entitiesLayout, this](){ CreateContextMenu(entitiesLayout); };
        entitiesLayout->GetProps().colorOutline = Theme::GetDef().accentPrimary0;
        scroll->AddChild(entitiesLayout);
        
        m_entitiesLayout = entitiesLayout;
	}

    void PanelEntities::Destruct()
    {
        m_editor->RemovePayloadListener(this);
    }

    void PanelEntities::PreTick()
    {
        auto* world = m_worldManager->GetLoadedWorld();
        
        if(world != m_world)
        {
            m_world = world;
            m_world->AddListener(this);
            RefreshHierarchy();
        }
    }

    void PanelEntities::Tick(float dt)
    {
        m_entitiesLayout->GetProps().outlineThickness = m_entityPayloadActive ? Theme::GetDef().baseOutlineThickness : 0.0f;
        
        if(m_entityPayloadActive)
        {
            for(auto* s : m_entitySelectables)
                s->GetProps().colorStart = s->GetProps().colorEnd = s->GetIsHovered() ? Theme::GetDef().accentPrimary0 : Color(0,0,0,0);
        }
    }

    void PanelEntities::RefreshHierarchy()
    {
        m_entitiesLayout->DeallocAllChildren();
        m_entitiesLayout->RemoveAllChildren();
        m_entitySelectables.clear();
        
        if(m_world == nullptr)
            return;
        
        Vector<Entity*> entities;
        m_world->GetAllEntities(entities);
        
        for(auto* e : entities)
        {
            if(e->GetParent() != nullptr || e->GetEntityFlags().IsSet(EF_TRANSIENT))
                continue;
            
            Widget* selectable = CreateEntitySelectable(e, 0);
            m_entitiesLayout->AddChild(selectable);
        }
        
        m_entitiesLayout->Initialize();
    }

	void PanelEntities::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		Widget::Draw(threadIndex);
	}

    void PanelEntities::CreateContextMenu(Widget* w)
    {
        
    }

    Widget* PanelEntities::CreateEntitySelectable(Entity *e, uint8 level)
    {
        FoldLayout* fold = m_manager->Allocate<FoldLayout>("Fold");
        fold->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
        fold->SetAlignedPosX(0.0f);
        fold->SetAlignedSizeX(1.0f);
        fold->SetUserData(e);

        Selectable* selectable = m_manager->Allocate<Selectable>(e->GetName());
        selectable->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
        selectable->SetAlignedPosX(0.0f);
        selectable->SetAlignedSizeX(1.0f);
        selectable->SetFixedSizeY(Theme::GetDef().baseItemHeight);
        selectable->SetLocalControlsManager(m_entitiesLayout);
        selectable->SetUserData(e);
        selectable->GetProps().colorOutline = Theme::GetDef().accentPrimary0;

        selectable->GetProps().onRightClick = [this](Selectable* s){
            CreateContextMenu(s);
        };
        selectable->GetProps().onInteracted = [fold](Selectable*) { fold->SetIsUnfolded(!fold->GetIsUnfolded());};
        selectable->SetOnGrabbedControls([e, this]() {
            
        });
        
        selectable->GetProps().onDockedOut = [this, selectable, e, level](){
            DirectionalLayout* payload = m_manager->Allocate<DirectionalLayout>();
            payload->GetChildMargins().left = Theme::GetDef().baseIndent;
            payload->SetUserData(e);
            
            Text* text = m_manager->Allocate<Text>();
            text->GetFlags().Set(WF_POS_ALIGN_Y);
            text->SetAlignedPosY(0.5f);
            text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
            text->GetProps().text = e->GetName();
            payload->AddChild(text);
            payload->Initialize();
            
            m_editor->CreatePayload(payload, PayloadType::Entity, Vector2ui(text->GetSizeX() + Theme::GetDef().baseIndent * 2, Theme::GetDef().baseItemHeight));
        };
        m_entitySelectables.push_back(selectable);
        fold->AddChild(selectable);
        
        DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
        layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
        layout->SetAlignedPos(Vector2::Zero);
        layout->SetAlignedSizeX(1.0f);
        layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
        layout->SetChildPadding(Theme::GetDef().baseIndentInner);
        layout->GetChildMargins().left = Theme::GetDef().baseIndent + Theme::GetDef().baseIndent * level;
        selectable->AddChild(layout);

        Icon* dropdown                   = m_manager->Allocate<Icon>("Dropdown");
        dropdown->GetProps().icon       = !fold->GetIsUnfolded() ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
        dropdown->GetProps().textScale = 0.3f;
        dropdown->GetFlags().Set(WF_POS_ALIGN_Y);
        dropdown->SetAlignedPosY(0.5f);
        dropdown->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        dropdown->GetProps().onClicked = [fold]() { fold->SetIsUnfolded(!fold->GetIsUnfolded()); };
        layout->AddChild(dropdown);

        if (e->GetChildren().empty())
            dropdown->SetVisible(false);

        Text* title = WidgetUtility::BuildEditableText(this, true, []() {});
        title->GetFlags().Set(WF_POS_ALIGN_Y);
        title->SetAlignedPosY(0.5f);
        title->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        title->GetProps().text = e->GetName();
        layout->AddChild(title);

        fold->GetProps().onFoldChanged = [dropdown, fold, e, level, this](bool unfolded) {
            dropdown->GetProps().icon = !unfolded ? ICON_CHEVRON_RIGHT : ICON_CHEVRON_DOWN;
            dropdown->CalculateIconSize();
            m_foldStatus[e] = unfolded;
            
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
                for (auto* c : e->GetChildren())
                {
                    if(c->GetEntityFlags().IsSet(EF_TRANSIENT))
                        continue;
                    
                    auto* childSelectable = CreateEntitySelectable(c, level + 1);
                    fold->AddChild(childSelectable);
                    childSelectable->Initialize();
                }
            }
        };

        fold->SetIsUnfolded(m_foldStatus[e]);
        
        return fold;
    }

    bool PanelEntities::OnFileMenuItemClicked(StringID sid, void *userData)
    {
        return false;
    }

    void PanelEntities::OnGetFileMenuItems(StringID sid, Vector<FileMenuItem::Data> &outData, void *userData)
    {
            
    }

void PanelEntities::OnPayloadStarted(PayloadType type, Widget *payload) { 
    if(type != PayloadType::Entity)
        return;
    m_entityPayloadActive = true;
}

void PanelEntities::OnPayloadEnded(PayloadType type, Widget *payload) { 
    if(type != PayloadType::Entity)
        return;
    m_entityPayloadActive = false;
    
    for(auto* s : m_entitySelectables)
        s->GetProps().colorStart = s->GetProps().colorEnd = Color(0,0,0,0);
}

    bool PanelEntities::OnPayloadDropped(PayloadType type, Widget *payload) {
        
        if(type != PayloadType::Entity)
            return false;
        
        if(!GetIsHovered())
            return false;
        
        Entity* payloadEntity = static_cast<Entity*>(payload->GetUserData());

        for(auto* s : m_entitySelectables)
        {
            if(s->GetIsHovered())
            {
                Entity* hoveredEntity = static_cast<Entity*>(s->GetUserData());
                if(hoveredEntity != payloadEntity)
                {
                    hoveredEntity->AddChild(payloadEntity);
                    RefreshHierarchy();
                }
              
                m_manager->AddToKillList(payload);
                return true;
            }
        }
        
        m_manager->AddToKillList(payload);
        
        if(payloadEntity->GetParent())
        {
            payloadEntity->RemoveFromParent();
            RefreshHierarchy();
        }
        
        return true;
    }

} // namespace Lina::Editor
