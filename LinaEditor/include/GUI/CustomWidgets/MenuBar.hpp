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

#pragma once

#ifndef MenuBar_HPP
#define MenuBar_HPP

#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "MenuPopup.hpp"

namespace Lina::Editor
{

    class MenuBar;

    class MenuBarItem
    {

    public:
        MenuBarItem(){};
        MenuBarItem(const String& name) : m_name(name){};
        MenuBarItem(const String& name, uint32 id) : m_name(name){};
        virtual ~MenuBarItem() = default;

    protected:
        virtual void Draw(const Vector2& size) = 0;

    private:
        friend class MenuBar;

    protected:
        String m_name = "";
    };

    class MenuBarItemPopup : public MenuBarItem
    {

    public:
        MenuBarItemPopup(){};
        MenuBarItemPopup(const String& name) : MenuBarItem(name){};
        virtual ~MenuBarItemPopup() = default;

        inline MenuPopup& GetPopup()
        {
            return m_menuPopup;
        }

    protected:
        virtual void Draw(const Vector2& size) override;

    private:
        void OnClicked();

    private:
        bool      m_isOpen = false;
        MenuPopup m_menuPopup;
    };

    class MenuBar
    {
    public:
        MenuBar() = default;
        ~MenuBar();

        void Draw();

        template <typename... Args> void AddItem(Args... args)
        {
            (m_items.push_back(std::forward<Args>(args)), ...);
        }

        inline void SetItemSize(const Vector2& size)
        {
            m_itemSize = size;
        }

        inline void SetStartPosition(const Vector2& pos)
        {
            m_startPosition = pos;
        }

        inline void SetExtraSpacing(float spacing)
        {
            m_extraSpacing = spacing;
        }

    private:
        Vector<MenuBarItem*> m_items;
        Vector2              m_itemSize      = Vector2::Zero;
        Vector2              m_startPosition = Vector2::Zero;
        float                m_extraSpacing  = 0.0f;
    };
} // namespace Lina::Editor

#endif
