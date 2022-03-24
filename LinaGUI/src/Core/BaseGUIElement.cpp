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

#include "Core/BaseGUIElement.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/InputBackend.hpp"
#include "Math/Math.hpp"

#define ENABLE_DEBUG

namespace Lina::GUI
{
    BaseGUIElement* BaseGUIElement::s_elementOnTop = nullptr;

    BaseGUIElement::~BaseGUIElement()
    {
        for (auto* child : m_children)
            delete child;

        m_children.clear();
    }

    void BaseGUIElement::Initialize()
    {
    }

    void BaseGUIElement::AddChild(BaseGUIElement* child)
    {
        m_children.push_back(child);
        child->m_parent = this;
    }

    void BaseGUIElement::RemoveChild(BaseGUIElement* child, bool freeAsWell)
    {
        for (std::vector<BaseGUIElement*>::iterator it = m_children.begin(); it != m_children.end(); it++)
        {
            if (*it == child)
            {
                m_children.erase(it);

                if (freeAsWell)
                    delete child;

                break;
            }
        }
    }

    bool BaseGUIElement::IsMouseHovering()
    {
        const Vector2 mousePos = Input::InputEngine::Get()->GetMousePosition();
        if (mousePos.x > m_min.x && mousePos.x < m_min.x + m_sizeAbs.x && mousePos.y > m_min.y && mousePos.y < m_min.y + m_sizeAbs.y)
            return true;
        return false;
    }

    bool BaseGUIElement::IsOnTop()
    {
        return s_elementOnTop == this;
    }

    Vector2 BaseGUIElement::GetAbsPosition()
    {
        if (m_parent == nullptr)
            return m_position;

        const Vector2 parentAbs     = m_parent->GetAbsPosition();
        const Vector2 parentAbsSize = m_parent->GetAbsSize();
        return Vector2(parentAbs.x + parentAbsSize.x * m_position.x, parentAbs.y + parentAbsSize.y * m_position.y);
    }

    Vector2 BaseGUIElement::GetAbsSize()
    {
        if (m_parent == nullptr)
            return m_size;

        const Vector2 parentSize = m_parent->GetAbsSize();
        return Vector2(parentSize.x * m_size.x, parentSize.y * m_size.y);
    }

    Vector2 BaseGUIElement::GetPositionFromAbs(const Vector2& abs)
    {
        if (m_parent == nullptr)
            return abs;
        return Math::Remap(abs, m_parent->m_min, m_parent->m_max, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
    }

    void BaseGUIElement::CalculateFocus()
    {
        if (IsMouseHovering())
            s_elementOnTop = this;

        for (auto* child : m_children)
            child->CalculateFocus();
    }

    void BaseGUIElement::CalculateBounds()
    {
        m_min     = GetAbsPosition();
        m_max     = m_min + GetAbsSize();
        m_center  = (m_max + m_min) / 2.0f;
        m_sizeAbs = m_max - m_min;

        for (auto* child : m_children)
            child->CalculateBounds();
    }

    void BaseGUIElement::Render()
    {
        m_isFocused = IsMouseHovering() && IsOnTop();

#ifdef ENABLE_DEBUG
        const Color col = m_isFocused ? Color::Red : Color::White;
        ImGui::GetWindowDrawList()->AddRect(IMVEC2(GetAbsPosition()), IMVEC2((GetAbsPosition() + GetAbsSize())), IMCOL32(col), 0.0f, 0.0f, 2.0f);
#endif

        CalculateMouseDrag();

        for (auto* child : m_children)
            child->Render();
    }

    void BaseGUIElement::CalculateMouseDrag()
    {
        if (Input::InputEngine::Get()->GetMouseButtonDown(0))
            LINA_TRACE("MBD {0} {1}", m_isDraggable, m_isFocused);

            if (m_isDraggable)
        {
                LINA_TRACE("ya");
        }

        if (m_isFocused)
        {
            LINA_TRACE("jaaa");
        }

        if (Input::InputEngine::Get()->GetMouseButtonDown(0) || Input::InputEngine::Get()->GetKeyDown(LINA_KEY_SPACE))
        {
            LINA_TRACE("uuuuuuu");
        }
        if (m_isDraggable && m_isFocused && Input::InputEngine::Get()->GetMouseButtonDown(0))
        {
            const Vector2 mousePos = Input::InputEngine::Get()->GetMousePosition();
            m_dragBeginMouse       = mousePos;
            m_dragBegin            = m_min;
            m_isDragging           = true;
        }

        if (m_isDragging)
        {
            const Vector2 mousePos   = Input::InputEngine::Get()->GetMousePosition();
            const Vector2 delta      = mousePos - m_dragBeginMouse;
            Vector2       desiredAbs = m_dragBegin + delta;

            if (m_dragStyle == DragStyle::ConfinedInParent)
            {
                if (desiredAbs.x + m_sizeAbs.x > m_parent->m_max.x)
                    desiredAbs.x = m_parent->m_max.x - m_sizeAbs.x;
                else if (desiredAbs.x < m_parent->m_min.x)
                    desiredAbs.x = m_parent->m_min.x;
                if (desiredAbs.y + m_sizeAbs.y > m_parent->m_max.y)
                    desiredAbs.y = m_parent->m_max.y - m_sizeAbs.y;
                else if (desiredAbs.y < m_parent->m_min.y)
                    desiredAbs.y = m_parent->m_min.y;
            }

            m_position = GetPositionFromAbs(desiredAbs);
            LINA_TRACE("Pos: {0}", m_min.ToString());
        }

        if (m_isDragging && Input::InputEngine::Get()->GetMouseButtonUp(0))
        {
            m_isDragging = false;
        }
    }

} // namespace Lina::GUI