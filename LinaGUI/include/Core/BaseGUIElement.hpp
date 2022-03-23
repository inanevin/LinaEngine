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
Class: BaseGUIElement



Timestamp: 3/22/2022 10:41:11 PM
*/

#pragma once

#ifndef BaseGUIElement_HPP
#define BaseGUIElement_HPP

// Headers here.
#include "Math/Vector.hpp"
#include "Core/GUICommon.hpp"
#include <vector>

namespace Lina::GUI
{
    class BaseGUIElement
    {

    public:
        BaseGUIElement(){};
        virtual ~BaseGUIElement();

        void Initialize();
        void AddChild(BaseGUIElement* child);
        void RemoveChild(BaseGUIElement* child, bool freeAsWell);

        bool         IsMouseHovering();
        bool         IsOnTop();
        Vector2      GetAbsPosition();
        Vector2      GetAbsSize();
        Vector2      GetPositionFromAbs(const Vector2& abs);
        void         CalculateFocus();
        virtual void CalculateBounds();
        virtual void Render();

        static BaseGUIElement* s_elementOnTop;

        std::vector<BaseGUIElement*> m_children;
        BaseGUIElement*              m_parent    = nullptr;
        Vector2                      m_position  = Vector2(0, 0); // range: 0.0f - 1.0f if child.
        Vector2                      m_size      = Vector2(0, 0); // range: 0.0f - 1.0f if child.
        Vector2                      m_min       = Vector2(0, 0); // abs
        Vector2                      m_max       = Vector2(0, 0); // abs
        Vector2                      m_center    = Vector2(0, 0);
        Vector2                      m_sizeAbs   = Vector2(0, 0);
        bool                         m_isFocused = false;

        // Drag func.
        DragStyle m_dragStyle      = DragStyle::Free;
        bool      m_isDraggable    = false;
        bool      m_isDragging     = false;
        Vector2   m_dragBegin      = Vector2(0, 0);
        Vector2   m_dragBeginMouse = Vector2(0, 0);

    private:
        void CalculateMouseDrag();
    };
} // namespace Lina::GUI

#endif
