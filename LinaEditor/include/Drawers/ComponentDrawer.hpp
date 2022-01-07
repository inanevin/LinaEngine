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
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>
#include <functional>
#include <map>
#include <tuple>

namespace Lina::Editor
{
    struct ETransformPivotChanged;
    struct EComponentOrderSwapped;

    typedef std::map<std::string, std::vector<std::pair<std::string, TypeID>>> AddComponentMap;

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
        std::vector<TypeID> m_componentDrawList;
        bool                m_isTransformPivotGlobal = true;
    };

} // namespace Lina::Editor

#endif
