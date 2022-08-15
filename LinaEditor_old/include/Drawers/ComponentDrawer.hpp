/*
Class: ComponentDrawer

Responsible for drawing entity components by overriding Draw methods of the components.

Timestamp: 10/13/2020 2:34:21 PM
*/
#pragma once

#ifndef ComponentDrawer_HPP
#define ComponentDrawer_HPP

#include "Core/CommonECS.hpp"
#include "Utility/StringId.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Data/Map.hpp"
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>
#include <functional>
#include <tuple>

namespace Lina::Editor
{
    struct ETransformPivotChanged;
    struct EComponentOrderSwapped;

    typedef Map<String, Vector<std::pair<String, TypeID>>> AddComponentMap;

    class ComponentDrawer
    {

    public:
        ComponentDrawer() = default;
        ~ComponentDrawer() = default;

        void Initialize();
        void ClearDrawList();

        /// <summary>
        /// Stores the given component in draw list.
        /// </summary>
        void PushComponentToDraw(TypeID tid, ECS::Entity ent);

        /// <summary>
        /// Flushes all stored components in draw list.
        /// </summary>
        void DrawAllComponents(ECS::Entity ent);

        /// <summary>
        /// Draws entity data & physics components for given entity.
        /// </summary>
        void DrawEntityData(ECS::Entity ent);

        /// <summary>
        /// Returns the components who are eligible to be added to the given entity.
        /// </summary>
        AddComponentMap GetCurrentAddComponentMap(ECS::Entity entity);

    private:

        void DrawComponent(TypeID tid, ECS::Entity ent);

    private:
        void OnTransformPivotChanged(const ETransformPivotChanged& ev);
        void OnComponentOrderSwapped(const EComponentOrderSwapped& ev);

    private:
        AddComponentMap     m_addComponentMap; // Category - vector of pairs - pair.first = component title, pair.second component id.
        Vector<TypeID> m_componentDrawList;
        bool                m_isTransformPivotGlobal = true;
    };

} // namespace Lina::Editor

#endif
