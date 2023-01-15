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

#include "Input/Core/InputEngine.hpp"
#include "EventSystem/InputEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Profiling/Profiler.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Graphics/Platform/Win32/Win32Window.hpp"

#else
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

namespace Lina::Input
{
#define MOUSE_SENSITIVITY 15.0f

#ifdef LINA_PLATFORM_WINDOWS
    HWND__* win32Window = nullptr;
#else
    GLFWwindow* glfwWindow = nullptr;
#endif

    InputEngine* InputEngine::s_inputEngine = nullptr;

    void InputEngine::Initialize()
    {
        LINA_TRACE("[Initialization] -> Input Engine GLFW ({0})", typeid(*this).name());
        Event::EventSystem::Get()->Connect<Event::EWindowContextCreated, &InputEngine::OnWindowContextCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseScrollCallback, &InputEngine::OnMouseScrollCallback>(this);
        Event::EventSystem::Get()->Connect<Event::EActiveAppChanged, &InputEngine::OnActiveAppChanged>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseButtonCallback, &InputEngine::OnMouseButtonCallback>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseMovedRaw, &InputEngine::OnMouseMovedRaw>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowFocused, &InputEngine::OnWindowFocused>(this);
        Event::EventSystem::Get()->Connect<Event::EKeyCallback, &InputEngine::OnKeyCallback>(this);
        m_horizontalAxis.BindAxis(LINA_KEY_D, LINA_KEY_A);
        m_verticalAxis.BindAxis(LINA_KEY_W, LINA_KEY_S);
    }

    void InputEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Input Engine GLFW ({0})", typeid(*this).name());
        Event::EventSystem::Get()->Disconnect<Event::EWindowContextCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EMouseScrollCallback>(this);
        Event::EventSystem::Get()->Disconnect<Event::EActiveAppChanged>(this);
        Event::EventSystem::Get()->Disconnect<Event::EMouseButtonCallback>(this);
        Event::EventSystem::Get()->Disconnect<Event::EMouseMovedRaw>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowFocused>(this);
        Event::EventSystem::Get()->Disconnect<Event::EKeyCallback>(this);
        m_keyDownNewStateMap.clear();
        m_keyUpNewStateMap.clear();
        m_mouseDownNewStateMap.clear();
        m_mouseUpNewStateMap.clear();
    }

    void InputEngine::OnWindowContextCreated(const Event::EWindowContextCreated& e)
    {
#ifdef LINA_PLATFORM_WINDOWS
        win32Window = static_cast<HWND__*>(e.window);
#else
        glfwWindow = static_cast<GLFWwindow*>(e.window);
#endif

        m_lastFocusedWindowHandle = static_cast<void*>(win32Window);
    }

    void InputEngine::OnMouseScrollCallback(const Event::EMouseScrollCallback& e)
    {
        m_mouseScrollPrev.x = e.xoff;
        m_mouseScrollPrev.y = e.yoff;
    }

    void InputEngine::OnMouseButtonCallback(const Event::EMouseButtonCallback& e)
    {
        m_currentStates[e.button] = static_cast<InputAction>(e.action) == InputAction::Pressed ? 1 : 0;

        if (e.action == static_cast<int>(InputAction::Repeated))
            m_doubleClicks[e.button] = true;
    }

    void InputEngine::OnKeyCallback(const Event::EKeyCallback& e)
    {
        m_currentStates[e.key] = static_cast<InputAction>(e.action) == InputAction::Pressed ? 1 : 0;
    }

    void InputEngine::OnMouseMovedRaw(const Event::EMouseMovedRaw& e)
    {
        m_mouseDeltaRawPrev.x = e.xDelta;
        m_mouseDeltaRawPrev.y = e.yDelta;
    }

    void InputEngine::OnActiveAppChanged(const Event::EActiveAppChanged& e)
    {
        m_windowActive = e.isThisApp;
    }

    void InputEngine::OnWindowFocused(const Event::EWindowFocused& e)
    {
        m_lastFocusedWindowHandle = e.window;
    }

    bool InputEngine::GetKey(int keycode)
    {
        if (!m_windowActive)
            return false;
        return m_currentStates[keycode] == 1;
    }

    bool InputEngine::GetKeyDown(int keyCode)
    {
        if (!m_windowActive)
            return false;

        return m_currentStates[keyCode] == 1 && m_previousStates[keyCode] == 0;
    }
    bool InputEngine::GetKeyUp(int keyCode)
    {
        if (!m_windowActive)
            return false;

        return m_currentStates[keyCode] == 0 && m_previousStates[keyCode] == 1;
    }
    bool InputEngine::GetMouseButton(int button)
    {
        if (!m_windowActive)
            return false;

        return m_currentStates[button] == 1;
    }

    bool InputEngine::GetMouseButtonDown(int button)
    {
        if (!m_windowActive)
            return false;

        return m_currentStates[button] == 1 && m_previousStates[button] == 0;
    }
    bool InputEngine::GetMouseButtonUp(int button)
    {
        if (!m_windowActive)
            return false;

        return m_currentStates[button] == 0 && m_previousStates[button] == 1;
    }

    bool InputEngine::GetMouseButtonDoubleClick(int button)
    {
        return m_doubleClicks[button];
    }

    bool InputEngine::GetMouseButtonClicked(int button)
    {
        bool result = false;

        if (GetMouseButtonDown(button))
            m_mousePosTrackingClick = GetMousePositionAbs();

        if (GetMouseButtonUp(button) && GetMousePositionAbs().Equals(m_mousePosTrackingClick, 5))
            result = true;

        return result;
    }

    bool InputEngine::IsPointInRect(const Vector2i point, const Recti& rect)
    {
#ifdef LINA_PLATFORM_WINDOWS

        RECT r;
        r.left   = rect.pos.x;
        r.top    = rect.pos.y;
        r.right  = rect.pos.x + rect.size.x;
        r.bottom = rect.pos.y + rect.size.y;
        POINT pt;
        pt.x = point.x;
        pt.y = point.y;
        return PtInRect(&r, pt);
#else
        return false;
#endif
    }

    void InputEngine::SetCursorMode(CursorMode mode)
    {
        if (mode == m_cursorMode)
            return;

        m_cursorMode = mode;

        switch (mode)
        {
        case CursorMode::Visible:
#ifdef LINA_PLATFORM_WINDOWS
            ShowCursor(true);
#else
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
            break;

        case CursorMode::Hidden:
#ifdef LINA_PLATFORM_WINDOWS
            ShowCursor(false);
#else
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#endif
            break;

        case CursorMode::Disabled:
#ifdef LINA_PLATFORM_WINDOWS
            ShowCursor(false);
#else
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
            break;
        }
    }

    void InputEngine::SetMousePosition(const Vector2& v) const
    {
#ifdef LINA_PLATFORM_WINDOWS
        SetCursorPos(static_cast<int>(v.x), static_cast<int>(v.y));
#else
        glfwSetCursorPos(glfwWindow, v.x, v.y);
#endif
    }

    void InputEngine::PreTick()
    {
        for (int i = 0; i < 256; i++)
            m_previousStates[i] = m_currentStates[i];
    }

    void InputEngine::Tick()
    {
        m_mouseDeltaRaw     = m_mouseDeltaRawPrev;
        m_mouseScroll       = m_mouseScrollPrev;
        m_mouseScrollPrev   = Vector2i::Zero;
        m_mouseDeltaRawPrev = Vector2i::Zero;
        m_doubleClicks.clear();
        m_singleClickStates.clear();

#ifdef LINA_PLATFORM_WINDOWS
        POINT point;
        GetCursorPos(&point);
#else
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);
#endif
        m_previousMousePosition   = m_currentMousePositionAbs;
        m_currentMousePositionAbs = Vector2i(point.x, point.y);
        m_mouseDelta              = m_currentMousePositionAbs - m_previousMousePosition;

        // Refresh the key states from previous frame.
        for (auto& pair : m_keyDownNewStateMap)
            m_keyStatesDown[pair.first] = m_keyDownNewStateMap[pair.first];

        for (auto& pair : m_keyUpNewStateMap)
            m_keyStatesUp[pair.first] = m_keyUpNewStateMap[pair.first];

        for (auto& pair : m_mouseDownNewStateMap)
            m_mouseStatesDown[pair.first] = m_mouseDownNewStateMap[pair.first];

        for (auto& pair : m_mouseUpNewStateMap)
            m_mouseStatesUp[pair.first] = m_mouseUpNewStateMap[pair.first];

        m_keyDownNewStateMap.clear();
        m_keyUpNewStateMap.clear();
        m_mouseDownNewStateMap.clear();
        m_mouseUpNewStateMap.clear();

#ifdef LINA_PLATFORM_WINDOWS

#else
        glfwPollEvents();
#endif
    }
} // namespace Lina::Input
