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
