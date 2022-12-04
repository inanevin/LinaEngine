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

#ifndef MenuPopup_HPP
#define MenuPopup_HPP

#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Math/Vector.hpp"

namespace Lina::Editor
{
    class MenuPopup;

    class MenuPopupElement
    {

    public:
        MenuPopupElement() = default;
        MenuPopupElement(const String& name, uint32 id, const String& tooltip) : m_name(name), m_id(id), m_tooltip(tooltip){};
        virtual ~MenuPopupElement() = default;

    private:
        friend class MenuPopup;

    protected:
        virtual void  OnClicked() = 0;
        virtual void  Draw()      = 0;
        virtual float GetWidth()  = 0;

    protected:
        String m_name    = "";
        uint32 m_id      = 0;
        String m_tooltip = "";
        float  m_width   = 0.0f;
    };

    class MenuPopupToggleElement : public MenuPopupElement
    {
    public:
        MenuPopupToggleElement() = default;
        MenuPopupToggleElement(const String& name, uint32 id, String tooltip = "") : MenuPopupElement(name, id, tooltip){};
        virtual ~MenuPopupToggleElement() = default;

    protected:
        virtual void  OnClicked() override;
        virtual void  Draw() override;
        virtual float GetWidth() override;
    };

    class MenuPopupExpandableElement : public MenuPopupElement
    {
    public:
        MenuPopupExpandableElement() = default;
        MenuPopupExpandableElement(const String& name, uint32 id, String tooltip = "") : MenuPopupElement(name, id, tooltip){};
        virtual ~MenuPopupExpandableElement() = default;

    protected:
        virtual void  OnClicked() override;
        virtual void  Draw() override;
        virtual float GetWidth() override;
    };

    class MenuPopupActionElement : public MenuPopupElement
    {
    public:
        MenuPopupActionElement() = default;
        MenuPopupActionElement(const String& name, uint32 id, String tooltip = "") : MenuPopupElement(name, id, tooltip){};
        virtual ~MenuPopupActionElement() = default;

    protected:
        virtual void  OnClicked() override;
        virtual void  Draw() override;
        virtual float GetWidth() override;
    };

    class MenuPopupDividerElement : public MenuPopupElement
    {
    public:
        MenuPopupDividerElement() = default;
        MenuPopupDividerElement(const String& name, uint32 id, String tooltip = "") : MenuPopupElement(name, id, tooltip){};
        virtual ~MenuPopupDividerElement() = default;

    protected:
        virtual void  OnClicked() override;
        virtual void  Draw() override;
        virtual float GetWidth() override;
    };

    class MenuPopup
    {
    public:
        ~MenuPopup();
        void AddElement(MenuPopupElement* element);
        void Draw(const Vector2& startPosition);

    private:
        Vector<MenuPopupElement*> m_elements;
        float                     m_maxWidth = 0.0f;
    };
} // namespace Lina::Editor

#endif
