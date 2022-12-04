/*
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

#ifndef LinaGUI_HPP
#define LinaGUI_HPP

#include "Math/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Data/Deque.hpp"
#include "Math/Color.hpp"
#include "Widgets.hpp"
#include "Theme.hpp"

namespace Lina::Editor
{
    class ImmediateGUI;
    class ImmediateWindow;

    class ImmediateWidget
    {
    public:
        bool IsHovered();
        bool IsPressed();
        bool IsClicked();

        inline Vector2 GetAbsPos()
        {
            return m_absPos;
        }

    private:
        friend class ImmediateWindow;
        Vector2 m_size   = Vector2::Zero;
        Vector2 m_penPos = Vector2::Zero;
        Vector2 m_absPos = Vector2::Zero;
    };

    class ImmediateWindow
    {
    public:
        void Draw();
        bool IsWindowHovered();

        // Widgets
        void             BeginWidget(const Vector2& size);
        void             EndWidget();
        ImmediateWidget& GetCurrentWidget();

        inline void SetPenPos(const Vector2& pos)
        {
            m_penPos = pos;
        }

        inline void SetPenPosX(float x)
        {
            m_penPos.x = x;
        }

        inline void SetPenPosY(float y)
        {
            m_penPos.y = y;
        }

        inline Vector2 GetPenPos() const
        {
            return m_penPos;
        }

        inline Vector2 GetAbsPos() const
        {
            return m_absPos;
        }

        inline Vector2 GetRelPos() const
        {
            return m_relPos;
        }

        inline Vector2 GetSize() const
        {
            return m_size;
        }

        inline int GetDrawOrder() const
        {
            return m_drawOrder;
        }

        inline uint32 GetID()
        {
            return m_id;
        }

    private:
        friend class ImmediateGUI;

        StringID                m_id        = 0;
        Vector2                 m_penPos    = Vector2::Zero;
        Vector2                 m_absPos    = Vector2::Zero;
        Vector2                 m_relPos    = Vector2::Zero;
        Vector2                 m_size      = Vector2::Zero;
        int                     m_drawOrder = 0;
        Vector<ImmediateWidget> m_widgets;
    };

    class ImmediateGUI
    {
    public:
        static ImmediateGUI* Get()
        {
            return s_instance;
        }

        // Frame
        void StartFrame();
        void EndFrame();

        // Window
        bool             BeginWindow(StringID sid);
        void             EndWindow();
        void             SetWindowSize(StringID sid, const Vector2& size);
        void             SetWindowPosition(StringID sid, const Vector2& pos);
        ImmediateWindow& GetCurrentWindow();
        ImmediateWindow* GetHoveredWindow();
        ImmediateWindow* GetWindowByID(StringID sid);

        // Utility
        bool IsMouseHoveringRect(const Rect& rect);

        // Theme
        inline Theme& GetTheme()
        {
            return m_theme;
        }

        StringID GetIconTexture()
        {
            return m_iconTexture;
        }

    private:
        friend class Editor;

        static ImmediateGUI*       s_instance;
        Vector<ImmediateWindow>    m_windows;
        HashMap<StringID, Vector2> m_windowSizes;
        HashMap<StringID, Vector2> m_windowPositions;
        bool                       m_hasParentWindow = false;
        Theme                      m_theme;
        StringID                   m_hoveredWindow          = 0;
        StringID                   m_transientHoveredWindow = 0;
        StringID                   m_iconTexture;
    };

#define LGUI ImmediateGUI::Get()

} // namespace Lina::Editor

#endif
