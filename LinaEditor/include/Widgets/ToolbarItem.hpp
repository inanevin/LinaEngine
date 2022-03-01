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
Class: ToolbarItem



Timestamp: 12/27/2021 12:05:24 AM
*/

#pragma once

#ifndef ToolbarItem_HPP
#define ToolbarItem_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"
#include "Math/Vector.hpp"
#include <vector>

namespace Lina::Editor
{

    enum class ToolbarItemType
    {
        None = 0,

        MoveHandle   = 1,
        RotateHandle = 2,
        ScaleHandle  = 3,

        GlobalLocalHandle = 10,
        PivotHandle       = 11,

        PlayButton = 20,
        StopButton = 21,
        SkipButton = 22,
    };

    class ToolbarItem;

    class ToolbarGroup
    {
    public:
        ToolbarGroup(const Vector2& buttonSize)
            : m_buttonSize(buttonSize)
        {
        }

        virtual ~ToolbarGroup();

        void AddItem(ToolbarItem* item);
        void Draw();

    private:
        Vector2                     m_buttonSize = Vector2::Zero;
        std::vector<ToolbarItem*> m_items;
    };

    class ToolbarItem
    {

    public:
        ToolbarItem(const char* icon, const char* tooltip, bool useToggle = false, bool startToggled = false, ToolbarItemType type = ToolbarItemType::None)
            : m_icon(icon), m_tooltip(tooltip), m_useToggle(useToggle), m_toggled(startToggled), m_type(type){};

        ToolbarItem() = default;
        ~ToolbarItem() = default;

        void Draw(const Vector2& size);
        bool m_toggled = false;

    private:
        const char*     m_tooltip   = nullptr;
        const char*     m_icon      = nullptr;
        bool            m_useToggle = false;
        ToolbarItemType m_type      = ToolbarItemType::None;
    };
} // namespace Lina::Editor

#endif
