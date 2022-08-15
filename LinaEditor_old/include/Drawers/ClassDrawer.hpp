/*
Class: ClassDrawer



Timestamp: 1/5/2022 3:43:30 PM
*/

#pragma once

#ifndef ClassDrawer_HPP
#define ClassDrawer_HPP

// Headers here.
#include "Core/CommonECS.hpp"
#include "Utility/StringId.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
#include <entt/meta/meta.hpp>

namespace Lina::Editor
{
    class ClassDrawer
    {

    public:
        ClassDrawer()  = default;
        ~ClassDrawer() = default;

        /// <summary>
        /// Draws all reflected properties in the given class, returns whether any of the properties is changed.
        /// </summary>
        static bool DrawClass(TypeID tid, entt::meta_any& instance, bool drawHeader = false);

        /// <summary>
        /// Pushes the given property to the draw list which is organized based on property categories.
        /// </summary>
        static void AddPropertyToDrawList(const String& category, entt::meta_data& data);

        /// <summary>
        /// Draws all the properties in the draw list & clears the list.
        /// </summary>
        /// <param name="id"></param>
        static bool FlushDrawList(const String& id, entt::meta_type& owningType, entt::meta_any& instance);

        /// <summary>
        /// Draws a class field given meta_data property, returns if the property is changed.
        /// </summary>
        static bool DrawProperty(entt::meta_type& owningType, entt::meta_data& data, entt::meta_any& instance);

        static HashMap<String, Vector<entt::meta_data>> m_propertyList; // Category- property pair

    };
} // namespace Lina::Editor

#endif
