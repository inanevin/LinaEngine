/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Panels/EntitiesPanel.hpp"
#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/GUILayer.hpp"
#include "Core/InputBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Log/Log.hpp"
#include "Panels/LevelPanel.hpp"
#include "Rendering/Material.hpp"
#include "Panels/HeaderPanel.hpp"
#include "Rendering/Model.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Widgets/MenuButton.hpp"
#include "IconsFontAwesome5.h"
namespace Lina::Editor
{
    using namespace ECS;
    using namespace Lina;

    std::map<Entity, bool> m_visibilityMouseMap;
    bool                   m_isMouseDragging;

    EntitiesPanel::~EntitiesPanel()
    {
        delete m_rightClickMenu;
    }

    void EntitiesPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &EntitiesPanel::OnLevelInstall>(this);
        Event::EventSystem::Get()->Connect<Event::EKeyCallback, &EntitiesPanel::OnKeyCallback>(this);
        Event::EventSystem::Get()->Connect<EShortcut, &EntitiesPanel::OnShortcut>(this);

        m_rightClickMenu  = new Menu("RC");
        MenuBarElement* m = new MenuBarElement("", "Create", nullptr, 0, MenuBarElementType::None, false);
        m_rightClickMenu->AddElement(m);

        auto& createElements = HeaderPanel::GetCreateEntityElements();
        for (auto* elem : createElements)
            m->AddChild(elem);
    }

    void EntitiesPanel::DrawEntityNode(int id, ECS::Entity entity)
    {
        ECS::EntityDataComponent& data        = ECS::Registry::Get()->get<ECS::EntityDataComponent>(entity);
        const bool                hasChildren = data.m_children.size() > 0;
        const ImGuiTreeNodeFlags  parent      = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        const ImGuiTreeNodeFlags  leaf        = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
        ImGuiTreeNodeFlags        flags       = hasChildren ? parent : leaf;

        // Label as treenode.
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        if (entity == m_selectedEntity)
            flags |= ImGuiTreeNodeFlags_Selected;

        const std::string treeLabel = data.m_name;
        bool              open      = WidgetsUtility::TreeNode((void*)(intptr_t)entity, flags, treeLabel.c_str(), hasChildren);

        // Select entity.
        if (ImGui::IsItemClicked())
        {
            m_selectedEntity = entity;
            Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{m_selectedEntity});
        }

        // Drag and drop.
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload(ECS_MOVEENTITY, &entity, sizeof(Entity));

            // Display preview
            ImGui::Text(data.m_name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ECS_MOVEENTITY))
            {
                IM_ASSERT(payload->DataSize == sizeof(Entity));
                ECS::Registry::Get()->AddChildToEntity(entity, *(Entity*)payload->Data);
            }
            ImGui::EndDragDropTarget();
        }

        // Visibility icon.
        ImGui::TableNextColumn();
        const char* visibilityIcon      = data.GetIsEnabled() ? ICON_FA_EYE : ICON_FA_EYE_SLASH;
        const float visibilityIconWidth = ImGui::CalcTextSize("AB").x;

        ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        textColor.w -= data.GetIsEnabled() ? 0.2f : 0.5f;
        WidgetsUtility::TableAlignCenter(visibilityIconWidth);
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertFloat4ToU32(textColor));

        if (WidgetsUtility::IconButton(visibilityIcon))
        {
            ECS::Registry::Get()->SetEntityEnabled(entity, !data.GetIsEnabled());
        }

        // If the middle mouse is being dragged switch visibility for once, resetted when drag ends.
        if (ImGui::IsItemHovered() && !m_visibilityMouseMap[entity] && m_isMouseDragging)
        {
            m_visibilityMouseMap[entity] = true;
            ECS::Registry::Get()->SetEntityEnabled(entity, !data.GetIsEnabled());
        }

        ImGui::PopStyleColor();

        // Draw children.
        if (open && hasChildren)
        {
            for (auto child : data.m_children)
                DrawEntityNode(0, child);
            ImGui::TreePop();
        }
    }

    void EntitiesPanel::OnShortcut(const EShortcut& event)
    {
        if (event.m_name.compare("duplicate") == 0)
        {
            if (m_selectedEntity != entt::null)
            {
                m_selectedEntity = ECS::Registry::Get()->CreateEntity(m_selectedEntity);
                Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{m_selectedEntity});
            }
        }
        else if (event.m_name.compare("create_empty") == 0)
        {
            auto ent         = ECS::Registry::Get()->CreateEntity("Empty");
            m_selectedEntity = ent;
            Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{m_selectedEntity});
        }
    }

    void EntitiesPanel::OnKeyCallback(const Event::EKeyCallback& ev)
    {
        if (ev.m_action == Input::InputAction::Pressed && ev.m_key == LINA_KEY_DELETE)
        {
            if (m_selectedEntity != entt::null)
            {
                Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
                ECS::Registry::Get()->DestroyEntity(m_selectedEntity);
                m_selectedEntity = entt::null;
            }
        }
    }
    void EntitiesPanel::OnLevelInstall(const Event::ELevelInstalled& ev)
    {
        Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
        m_selectedEntity = entt::null;
    }

    void EntitiesPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
                ImGui::BeginChild("ecs_child", ImVec2(0, -30), true);

                static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;

                if (ImGui::BeginTable("entitiesTable", 2, flags))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableHeadersRow();

                    auto singleView = ECS::Registry::Get()->view<ECS::EntityDataComponent>();

                    for (auto entity : singleView)
                    {
                        ECS::EntityDataComponent& data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(entity);

                        if (data.m_parent == entt::null && data.m_name.compare(EDITOR_CAMERA_NAME) != 0)
                            DrawEntityNode(0, entity);
                    }

                    ImGui::EndTable();
                }

                // Switch visibility using mouse drag.
                if (Input::InputEngineBackend::Get()->GetMouseButton(LINA_MOUSE_3))
                    m_isMouseDragging = true;
                if (m_isMouseDragging && Input::InputEngineBackend::Get()->GetMouseButtonUp(LINA_MOUSE_3))
                {
                    m_isMouseDragging = false;
                    m_visibilityMouseMap.clear();
                }

                if (!ImGui::IsItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_selectedEntity = entt::null;
                    Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected());
                }

                WidgetsUtility::PushPopupStyle();

                // Handle Right Click.
                // if (Application::Get()->GetActiveLevelExists() && ImGui::BeginPopupContextWindow())
                // {
                //     m_rightClickMenu->Draw();
                //     ImGui::EndPopup();
                // }

                WidgetsUtility::PopPopupStyle();

                ImGui::EndChild();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                WidgetsUtility::HorizontalDivider(-ImGui::GetStyle().ItemSpacing.y, 4);
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                End();
            }
        }
    }

} // namespace Lina::Editor
