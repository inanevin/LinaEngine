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

#ifndef InputEngine_HPP
#define InputEngine_HPP

#include "InputCommon.hpp"
#include "InputAxis.hpp"
#include "InputMappings.hpp"
#include "Data/HashMap.hpp"
#include "Math/Rect.hpp"

namespace Lina
{
    class Engine;
    class Application;

    namespace Event
    {
        struct EWindowContextCreated;
        struct EMouseScrollCallback;
        struct EActiveAppChanged;
        struct EMouseButtonCallback;
        struct EMouseMovedRaw;
        struct EWindowFocused;
    } // namespace Event

} // namespace Lina

namespace Lina::Input
{
    class InputEngine
    {

    public:
        static InputEngine* Get()
        {
            return s_inputEngine;
        }

        bool GetKey(int button);
        bool GetKeyDown(int button);
        bool GetKeyUp(int button);
        bool GetMouseButton(int button);
        bool GetMouseButtonDown(int button);
        bool GetMouseButtonUp(int button);
        bool GetMouseButtonDoubleClick(int button);
        bool GetMouseButtonClicked(int button);
        bool IsPointInRect(const Vector2i point, const Recti& rect);
        void SetCursorMode(CursorMode mode);
        void SetMousePosition(const Vector2& v) const;

        /// <summary>
        /// 0,0 top-left, screenSizeX, screenSizeY bottom-right
        /// </summary>
        /// <returns></returns>
        inline Vector2 GetMousePosition()
        {
            return m_currentMousePosition;
        }

        inline Vector2 GetMousePositionAbs()
        {
            return m_currentMousePositionAbs;
        }

        inline Vector2 GetMouseDelta()
        {
            return m_mouseDelta;
        }

        inline Vector2 GetMouseDeltaRaw()
        {
            return m_mouseDeltaRaw;
        }

        inline Vector2 GetMouseScroll()
        {
            return m_currentMouseScroll;
        }

        inline CursorMode GetCursorMode()
        {
            return m_cursorMode;
        }
        inline float GetHorizontalAxisValue()
        {
            return m_horizontalAxis.GetValue();
        }
        inline float GetVerticalAxisValue()
        {
            return m_verticalAxis.GetValue();
        }

    private:
        friend class Engine;
        InputEngine()  = default;
        ~InputEngine() = default;
        void Initialize();
        void PrePoll();
        void Tick();
        void Shutdown();
        void OnWindowContextCreated(const Event::EWindowContextCreated& e);
        void OnMouseScrollCallback(const Event::EMouseScrollCallback& e);
        void OnMouseButtonCallback(const Event::EMouseButtonCallback& e);
        void OnMouseMovedRaw(const Event::EMouseMovedRaw& e);
        void OnActiveAppChanged(const Event::EActiveAppChanged& e);
        void OnWindowFocused(const Event::EWindowFocused& e);

    private:
        friend class Application;
        friend class Engine;
        static InputEngine* s_inputEngine;
        int                 m_keyStatesDown[NUM_KEY_STATES];
        int                 m_keyStatesUp[NUM_KEY_STATES]       = {0};
        int                 m_mouseStatesDown[NUM_MOUSE_STATES] = {0};
        int                 m_mouseStatesUp[NUM_MOUSE_STATES]   = {0};
        HashMap<int, int>   m_keyDownNewStateMap;
        HashMap<int, int>   m_keyUpNewStateMap;
        HashMap<int, int>   m_mouseDownNewStateMap;
        HashMap<int, int>   m_mouseUpNewStateMap;
        HashMap<int, bool>  m_doubleClicks;
        HashMap<int, int>   m_singleClickStates;
        InputAxis           m_horizontalAxis;
        InputAxis           m_verticalAxis;
        CursorMode          m_cursorMode              = CursorMode::Visible;
        Vector2             m_currentMouseScroll      = Vector2::Zero;
        Vector2             m_mousePosTrackingClick   = Vector2::Zero;
        Vector2             m_mouseDelta              = Vector2::Zero;
        Vector2             m_mouseDeltaRaw           = Vector2::Zero;
        Vector2             m_currentMousePosition    = Vector2::Zero;
        Vector2             m_previousMousePosition   = Vector2::Zero;
        Vector2             m_currentMousePositionAbs = Vector2::Zero;
        bool                m_windowActive            = false;
        void*               m_lastFocusedWindowHandle = nullptr;
    };
} // namespace Lina::Input

#endif
