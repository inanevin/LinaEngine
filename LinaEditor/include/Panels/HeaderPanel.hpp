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
Class: HeaderPanel

Draws the top header and its components like title, lina icon, window buttons, menubar etc.

Timestamp: 10/8/2020 1:39:03 PM
*/

#pragma once

#ifndef HeaderPanel_HPP
#define HeaderPanel_HPP

#include "Panels/EditorPanel.hpp"

#include <vector>

namespace Lina::Editor
{
    class Menu;
    enum class MenuBarElementType;

    class HeaderPanel : public EditorPanel
    {

    public:
        HeaderPanel() = default;
        virtual ~HeaderPanel();

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

        static std::vector<MenuBarElement*>& GetCreateEntityElements()
        {
            return s_createEntityElements;
        }

    private:
        void DrawMenuBarChild();

    private:
        static std::vector<MenuBarElement*> s_createEntityElements;
        std::string                         m_title = "";
        std::vector<Menu*>            m_menuButtons;
    };
} // namespace Lina::Editor

#endif
