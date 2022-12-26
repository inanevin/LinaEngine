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

#ifndef LINA_PLATFORM_WINDOWS

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
        Event::EventSystem::Get()->Connect<Event::EWindowFocusChanged, &InputEngine::OnWindowFocusChanged>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseButtonCallback, &InputEngine::OnMouseButtonCallback>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseMovedRaw, &InputEngine::OnMouseMovedRaw>(this);
        m_horizontalAxis.BindAxis(LINA_KEY_D, LINA_KEY_A);
        m_verticalAxis.BindAxis(LINA_KEY_W, LINA_KEY_S);
    }

    void InputEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Input Engine GLFW ({0})", typeid(*this).name());
        Event::EventSystem::Get()->Disconnect<Event::EWindowContextCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EMouseScrollCallback>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowFocusChanged>(this);
        Event::EventSystem::Get()->Disconnect<Event::EMouseButtonCallback>(this);
        Event::EventSystem::Get()->Disconnect<Event::EMouseMovedRaw>(this);
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
    }

    void InputEngine::OnMouseScrollCallback(const Event::EMouseScrollCallback& e)
    {
        m_currentMouseScroll.x = (float)e.xoff;
        m_currentMouseScroll.y = (float)e.yoff;
    }

    void InputEngine::OnMouseButtonCallback(const Event::EMouseButtonCallback& e)
    {
        if (e.action == static_cast<int>(InputAction::Repeated))
            m_doubleClicks[e.button] = true;
    }

    void InputEngine::OnMouseMovedRaw(const Event::EMouseMovedRaw& e)
    {
        m_mouseDeltaRaw.x = static_cast<float>(e.xDelta);
        m_mouseDeltaRaw.y = static_cast<float>(e.yDelta);
    }

    void InputEngine::OnWindowFocusChanged(const Event::EWindowFocusChanged& e)
    {
        m_windowFocused = e.isFocused;
    }

    bool InputEngine::GetKey(int keycode)
    {
        if (!m_windowFocused)
            return false;

#ifdef LINA_PLATFORM_WINDOWS
        return GetKeyState(keycode) & 0x8000;
#else
        int state  = glfwGetKey(glfwWindow, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
#endif
    }

    bool InputEngine::GetKeyDown(int keyCode)
    {
        if (!m_windowFocused)
            return false;

#ifdef LINA_PLATFORM_WINDOWS
        int  newState                 = GetKey(keyCode);
        bool flag                     = (newState == 1 && m_keyStatesDown[keyCode] == 0) ? true : false;
        m_keyDownNewStateMap[keyCode] = newState;
        return flag;
#else
        int  newState                 = glfwGetKey(glfwWindow, keyCode);
        bool flag                     = (newState == GLFW_PRESS && m_keyStatesDown[keyCode] == GLFW_RELEASE) ? true : false;
        m_keyDownNewStateMap[keyCode] = newState;
        return flag;
#endif
    }
    bool InputEngine::GetKeyUp(int keyCode)
    {
        if (!m_windowFocused)
            return false;

#ifdef LINA_PLATFORM_WINDOWS
        int  newState                 = GetKey(keyCode);
        bool flag                     = (newState == 0 && m_keyStatesDown[keyCode] == 1) ? true : false;
        m_keyDownNewStateMap[keyCode] = newState;
        return flag;
#else
        int  newState               = glfwGetKey(glfwWindow, keyCode);
        bool flag                   = (newState == GLFW_RELEASE && m_keyStatesUp[keyCode] == GLFW_PRESS) ? true : false;
        m_keyUpNewStateMap[keyCode] = newState;
        return flag;
#endif
    }
    bool InputEngine::GetMouseButton(int button)
    {
        if (!m_windowFocused)
            return false;

#ifdef LINA_PLATFORM_WINDOWS
        int state = GetKeyState(button) & 0x8000;
        return state;
#else
        int state = glfwGetMouseButton(glfwWindow, button);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
#endif
    }

    bool InputEngine::GetMouseButtonDown(int button)
    {
        if (!m_windowFocused)
            return false;

#ifdef LINA_PLATFORM_WINDOWS
        int  newState                  = GetMouseButton(button);
        bool flag                      = (newState == 1 && m_mouseStatesDown[button] == 0) ? true : false;
        m_mouseDownNewStateMap[button] = newState;
        return flag;
#else
        int  newState                  = glfwGetMouseButton(glfwWindow, button);
        bool flag                      = (newState == GLFW_PRESS && m_mouseStatesDown[button] == GLFW_RELEASE) ? true : false;
        m_mouseDownNewStateMap[button] = newState;
        return flag;
#endif
    }
    bool InputEngine::GetMouseButtonUp(int button)
    {
        if (!m_windowFocused)
            return false;

#ifdef LINA_PLATFORM_WINDOWS
        int  newState                = GetMouseButton(button);
        bool flag                    = (newState == 0 && m_mouseStatesUp[button] == 1) ? true : false;
        m_mouseUpNewStateMap[button] = newState;
        return flag;
#else
        int  newState                = glfwGetMouseButton(glfwWindow, button);
        bool flag                    = (newState == GLFW_RELEASE && m_mouseStatesUp[button] == GLFW_PRESS) ? true : false;
        m_mouseUpNewStateMap[button] = newState;
        return flag;
#endif
    }

    bool InputEngine::GetMouseButtonDoubleClick(int button)
    {
        return m_doubleClicks[button];
    }

    bool InputEngine::GetMouseButtonClicked(int button)
    {
        bool result = false;

        if (GetMouseButtonDown(button))
            m_mousePosTrackingClick = GetMousePosition();

        if (GetMouseButtonUp(button) && GetMousePosition().Equals(m_mousePosTrackingClick, 5))
            result = true;

        return result;
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

    void InputEngine::PrePoll()
    {
        m_mouseDeltaRaw      = Vector2::Zero;
        m_currentMouseScroll = Vector2::Zero;
        m_doubleClicks.clear();
        m_singleClickStates.clear();

        double xpos, ypos;
#ifdef LINA_PLATFORM_WINDOWS
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(win32Window, &point);
        xpos = point.x;
        ypos = point.y;
#else
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);
#endif
        m_previousMousePosition = m_currentMousePosition;
        m_currentMousePosition  = Vector2((float)xpos, (float)ypos);
        m_mouseDelta            = m_currentMousePosition - m_previousMousePosition;
    }

    void InputEngine::Tick()
    {
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
