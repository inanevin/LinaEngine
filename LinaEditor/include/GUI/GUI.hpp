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

#include "Core/SizeDefinitions.hpp"
#include "Utility/StringId.hpp"
#include "Math/Vector.hpp"
#include "Math/Rect.hpp"
#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Data/Deque.hpp"
#include "Math/Color.hpp"
#include "Widgets.hpp"
#include "Theme.hpp"

namespace Lina::Editor
{
    class ImmediateGUI;
    class ImmediateWindow;

    enum ImmediateWindowMask
    {
        IMW_UseAbsolutePosition = 0,
        IMW_UseAbsoluteDrawOrder,
    };

    struct PersistentWindowData
    {
        Vector2 position = Vector2::Zero;
        Vector2 size     = Vector2::Zero;
    };

    struct PerFrameWindowData
    {
        int  drawOrder = 0;
        Rect rect      = Rect();
    };

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

        // Layout
        void BeginHorizontal();
        void EndHorizontal();

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
            return m_sid;
        }

        inline const Deque<int>& GetHorizontalRequests()
        {
            return m_horizontalRequests;
        }

    private:
        friend class ImmediateGUI;

        String                 m_name                 = "";
        StringID               m_sid                  = 0;
        Vector2                m_penPos               = Vector2::Zero;
        Vector2                m_absPos               = Vector2::Zero;
        Vector2                m_relPos               = Vector2::Zero;
        Vector2                m_size                 = Vector2::Zero;
        float                  m_maxPenPosX           = 0.0f;
        float                  m_maxYDuringHorizontal = 0.0f;
        int                    m_drawOrder            = 0;
        Deque<ImmediateWidget> m_widgets;
        Deque<int>             m_horizontalRequests;
        Color                  m_color = Color::White;
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
        bool             BeginWindow(const String& name, Bitmask16 mask = 0);
        bool             BeginPopup(const String& name);
        void             EndWindow();
        void             EndPopup();
        void             SetWindowSize(const String& str, const Vector2& size);
        void             SetWindowPosition(const String& str, const Vector2& pos);
        ImmediateWindow& GetCurrentWindow();
        ImmediateWindow* GetWindowBySID(StringID sid);
        StringID         GetSIDFromStr(const String& str);
        String           GetNameFromSID(StringID sid);

        // Utility
        bool    IsMouseHoveringRect(const Rect& rect);
        Vector2 GetMousePosition();
        bool    GetMouseButtonDown(int button);
        bool    GetMouseButtonUp(int button);
        bool    GetMouseButton(int button);
        bool    GetMouseButtonClicked(int button);
        bool    GetMouseButtonDoubleClicked(int button);

        inline void SetAbsoluteDrawOrder(int drawOrder)
        {
            m_absoluteDrawOrder = drawOrder;
        }

        // Theme
        inline Theme& GetTheme()
        {
            return m_theme;
        }

        inline StringID GetIconTexture()
        {
            return m_iconTexture;
        }

        inline StringID GetHoveredWindowSID()
        {
            return m_hoveredWindow;
        }

    private:
        friend class Editor;

        static ImmediateGUI*                    s_instance;
        Deque<ImmediateWindow>                  m_windows;
        HashMap<String, StringID>               m_windowSIDs;
        HashMap<StringID, PersistentWindowData> m_windowDataPersistent;
        HashMap<StringID, PerFrameWindowData>   m_windowDataPerFrame;
        Theme                                   m_theme;
        StringID                                m_iconTexture;
        StringID                                m_hoveredWindow     = 0;
        int                                     m_absoluteDrawOrder = 0;
    };

#define LGUI ImmediateGUI::Get()

} // namespace Lina::Editor

#endif
