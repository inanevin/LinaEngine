/*
Class: EntityDrawer

Responsible for drawing the selected entity properties, name, layers etc. as well as initiating
component draw for that entity.

Timestamp: 10/12/2020 1:02:29 AM
*/

#pragma once

#ifndef EntityDrawer_HPP
#define EntityDrawer_HPP

#include "ComponentDrawer.hpp"
#include "Core/CommonECS.hpp"

namespace Lina::Editor
{
    class EntityDrawer
    {
    public:
        EntityDrawer()  = default;
        ~EntityDrawer() = default;

        void Initialize();
        void SetSelectedEntity(ECS::Entity entity);
        void DrawSelectedEntity();
        void AddComponentPopup();

    private:
        ComponentDrawer m_componentDrawer;
        ECS::Entity     m_selectedEntity = entt::null;
        bool            m_shouldCopyEntityName      = true;
    };
} // namespace Lina::Editor

#endif
