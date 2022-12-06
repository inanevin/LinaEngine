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

#include "GUI/CustomWidgets/MenuBar.hpp"
#include "GUI/GUI.hpp"
#include "Core/InputEngine.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
    void MenuBarItemPopup::Draw(const Vector2& size)
    {
        const Vector2& penPos = LGUI->GetCurrentWindow().GetPenPos();

        if (Input::InputEngine::Get()->GetMouseButtonDown(LINA_MOUSE_0))
        {
            if (m_isOpen && !LGUI->IsMouseHoveringRect(Rect(penPos, size)))
            {
                m_isOpen = !m_isOpen;
            }
        }

        if (Widgets::Button(m_name, size))
        {
            OnClicked();
        }

        if (m_isOpen)
        {
            const Vector2 menuPopupStartPos = penPos + Vector2(0, size.y);
            m_menuPopup.Draw(menuPopupStartPos);
        }
    }

    void MenuBarItemPopup::OnClicked()
    {
        m_isOpen = !m_isOpen;
    }

    MenuBar::~MenuBar()
    {
        for (auto& i : m_items)
            delete i;

        m_items.clear();
    }

    void MenuBar::Draw()
    {
        auto& window = LGUI->GetCurrentWindow();
        Vector2 pos = m_startPosition;
        for (auto i : m_items)
        {
            window.SetPenPos(pos);
            i->Draw(m_itemSize);
            pos.x += m_itemSize.x + m_extraSpacing;
        }
        window.SetPenPos(pos);
    }
} // namespace Lina::Editor
