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

#include "Drawers/EntityDrawer.hpp"

#include "Core/Application.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Drawers/ClassDrawer.hpp"
#include "IconsFontAwesome5.h"

namespace Lina::Editor
{
    using namespace entt::literals;

    void EntityDrawer::Initialize()
    {
        m_componentDrawer.Initialize();
    }
    void EntityDrawer::SetSelectedEntity(ECS::Entity entity)
    {
        m_shouldCopyEntityName = true;
        m_selectedEntity       = entity;

        m_componentDrawer.ClearDrawList();
    }

    void EntityDrawer::DrawSelectedEntity()
    {
        ECS::Registry*            ecs            = ECS::Registry::Get();
        ECS::EntityDataComponent& data           = ecs->get<ECS::EntityDataComponent>(m_selectedEntity);
        ImVec2                    windowPos      = ImGui::GetWindowPos();
        ImVec2                    windowSize     = ImGui::GetWindowSize();
        bool                      isEditorCamera = m_selectedEntity == ecs->GetEntity("Editor Camera");

        if (isEditorCamera)
            ImGui::BeginDisabled();

        ImGui::SetCursorPosX(12.0f);

        if (WidgetsUtility::Button(ICON_FA_PLUS, ImVec2(22, 22), 0.6f, 4.0f, ImVec2(0.5f, 0.0f)))
            AddComponentPopup();

        ImGui::SameLine();

        // Setup char.
        static char entityName[64] = "";
        if (m_shouldCopyEntityName)
        {
            m_shouldCopyEntityName = false;
            memset(entityName, 0, sizeof entityName);
            std::string str = ecs->get<ECS::EntityDataComponent>(m_selectedEntity).m_name;
            std::copy(str.begin(), str.end(), entityName);
        }

        // Entity name input text.
        WidgetsUtility::FramePaddingX(5);
        WidgetsUtility::IncrementCursorPosY(0.9f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
        ImGui::InputText("##ename", entityName, IM_ARRAYSIZE(entityName));
        ecs->get<ECS::EntityDataComponent>(m_selectedEntity).m_name = entityName;
        WidgetsUtility::PopStyleVar();

        // Entity enabled toggle button.
        ImGui::SameLine();
        WidgetsUtility::IncrementCursorPosY(3.0f);
        ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);

        const bool prev          = data.GetIsEnabled();
        bool       enabledToggle = prev;
        WidgetsUtility::ToggleButton("##eactive", &enabledToggle);

        if (enabledToggle != prev)
            ECS::Registry::Get()->SetEntityEnabled(m_selectedEntity, enabledToggle);

        // Add Component Popup
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 4));
        if (ImGui::BeginPopup("addComponentPopup", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            AddComponentMap map            = m_componentDrawer.GetCurrentAddComponentMap(m_selectedEntity);
            static char     filterChr[128] = "";
            const char*     addCompText    = "Add Component";
            WidgetsUtility::CenteredText(addCompText);
            ImGui::SetNextItemWidth(250);
            ImGui::InputText("##filter", filterChr, IM_ARRAYSIZE(filterChr));

            for (auto& category : map)
            {
                std::string filterLowercase = Utility::ToLower(std::string(filterChr));

                if (ImGui::TreeNode(category.first.c_str()))
                {
                    for (auto& pair : category.second)
                    {
                        std::string pairLowercase = Utility::ToLower(pair.first);
                        if (pairLowercase.find(std::string(filterLowercase)) != std::string::npos)
                        {
                            bool selected = false;
                            ImGui::Selectable(pair.first.c_str(), &selected);
                            if (selected)
                            {
                                entt::resolve(pair.second).func("add"_hs).invoke({}, m_selectedEntity);
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }

                    ImGui::TreePop();
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        if (isEditorCamera)
            ImGui::EndDisabled();

        WidgetsUtility::IncrementCursorPosY(6.0f);
        m_componentDrawer.DrawEntityData(m_selectedEntity);

        // Visit each component an entity has and add the component to the draw list if its registered as a drawable component.
        for (auto& pool : ECS::Registry::Get()->storage())
        {
            if (pool.second.contains(m_selectedEntity))
            {
                if (pool.first != GetTypeID<ECS::EntityDataComponent>() && pool.first != GetTypeID<ECS::PhysicsComponent>())
                    m_componentDrawer.PushComponentToDraw(pool.first, m_selectedEntity);
            }
        }

        m_componentDrawer.DrawAllComponents(m_selectedEntity);
    }

    void EntityDrawer::AddComponentPopup()
    {
        ImGui::OpenPopup("addComponentPopup");
    }

    void EntityDrawer::DrawEntityData()
    {
    }

} // namespace Lina::Editor
