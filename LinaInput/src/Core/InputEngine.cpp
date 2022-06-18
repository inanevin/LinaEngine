/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Core/InputEngine.hpp"
#include "EventSystem/InputEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Profiling/Profiler.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Lina::Input
{
#define AXIS_SENSITIVITY  0.1f
#define MOUSE_SENSITIVITY 5.0f
    GLFWwindow*      glfwWindow                     = nullptr;
    InputEngine* InputEngine::s_inputEngine = nullptr;

    void InputEngine::Initialize()
    {
        LINA_TRACE("[Initialization] -> Input Engine GLFW ({0})", typeid(*this).name());
        Event::EventSystem::Get()->Connect<Event::EWindowContextCreated, &InputEngine::OnWindowContextCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EMouseScrollCallback, &InputEngine::OnMouseScrollCallback>(this);
        m_horizontalAxis.BindAxis(LINA_KEY_D, LINA_KEY_A);
        m_verticalAxis.BindAxis(LINA_KEY_W, LINA_KEY_S);
        m_freeLookSystem.Initialize("Free Look");
        m_inputPipeline.AddSystem(m_freeLookSystem);
    }

    void InputEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Input Engine GLFW ({0})", typeid(*this).name());
        m_keyDownNewStateMap.clear();
        m_keyUpNewStateMap.clear();
        m_mouseDownNewStateMap.clear();
        m_mouseUpNewStateMap.clear();
    }

    void InputEngine::OnWindowContextCreated(const Event::EWindowContextCreated& e)
    {
        glfwWindow = static_cast<GLFWwindow*>(e.m_window);
    }

    void InputEngine::OnMouseScrollCallback(const Event::EMouseScrollCallback& e)
    {
        m_currentMouseScroll.x = (float)e.m_xoff;
        m_currentMouseScroll.y = (float)e.m_yoff;
    }

    bool InputEngine::GetKey(int keycode)
    {
        int state = glfwGetKey(glfwWindow, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool InputEngine::GetKeyDown(int keyCode)
    {
        int  newState                 = glfwGetKey(glfwWindow, keyCode);
        bool flag                     = (newState == GLFW_PRESS && m_keyStatesDown[keyCode] == GLFW_RELEASE) ? true : false;
        m_keyDownNewStateMap[keyCode] = newState;
        return flag;
    }
    bool InputEngine::GetKeyUp(int keyCode)
    {
        int  newState               = glfwGetKey(glfwWindow, keyCode);
        bool flag                   = (newState == GLFW_RELEASE && m_keyStatesUp[keyCode] == GLFW_PRESS) ? true : false;
        m_keyUpNewStateMap[keyCode] = newState;
        return flag;
    }
    bool InputEngine::GetMouseButton(int button)
    {
        int state = glfwGetMouseButton(glfwWindow, button);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    bool InputEngine::GetMouseButtonDown(int button)
    {
        int  newState                  = glfwGetMouseButton(glfwWindow, button);
        bool flag                      = (newState == GLFW_PRESS && m_mouseStatesDown[button] == GLFW_RELEASE) ? true : false;
        m_mouseDownNewStateMap[button] = newState;
        return flag;
    }
    bool InputEngine::GetMouseButtonUp(int button)
    {
        int  newState                = glfwGetMouseButton(glfwWindow, button);
        bool flag                    = (newState == GLFW_RELEASE && m_mouseStatesUp[button] == GLFW_PRESS) ? true : false;
        m_mouseUpNewStateMap[button] = newState;
        return flag;
    }

    Vector2 InputEngine::GetRawMouseAxis()
    {
        // Get the cursor position.
        double posX, posY;
        glfwGetCursorPos(glfwWindow, &posX, &posY);

        // Get the delta mouse position.
        Vector2 currentMousePosition = Vector2((float)posX, (float)posY);
        Vector2 diff                 = currentMousePosition - m_axisMousePos;
        Vector2 raw                  = Vector2::Zero;

        // Set raw axis values depending on the direction of the axis.
        if (diff.x > 0.0f)
            raw.x = 1.0f;
        else if (diff.x < 0.0f)
            raw.x = -1.0f;
        if (diff.y > 0)
            raw.y = 1.0f;
        else if (diff.y < 0)
            raw.y = -1.0f;

        // Set previous position.
        m_axisMousePos = currentMousePosition;

        // Return raw.
        return raw;
    }

    Vector2 InputEngine::GetMouseAxis()
    {
        double posX, posY;
        glfwGetCursorPos(glfwWindow, &posX, &posY);

        // Delta
        Vector2 diff = Vector2((float)(posX - m_axisMousePos.x), (float)(posY - m_axisMousePos.y));

        // Clamp and remap delta mouse position.
        diff.x = Math::Clamp(diff.x, -MOUSE_SENSITIVITY, MOUSE_SENSITIVITY);
        diff.y = Math::Clamp(diff.y, -MOUSE_SENSITIVITY, MOUSE_SENSITIVITY);
        diff.x = Math::Remap(diff.x, -MOUSE_SENSITIVITY, MOUSE_SENSITIVITY, -1.0f, 1.0f);
        diff.y = Math::Remap(diff.y, -MOUSE_SENSITIVITY, MOUSE_SENSITIVITY, -1.0f, 1.0f);

        // Set the previous position.
        m_axisMousePos = Vector2((float)posX, (float)posY);

        return diff;
    }

    void InputEngine::AddToInputPipeline(ECS::System& system)
    {
        m_inputPipeline.AddSystem(system);
    }

    Vector2 InputEngine::GetMousePosition()
    {
        double xpos, ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);
        return Vector2((float)xpos, (float)ypos);
    }

    void InputEngine::SetCursorMode(CursorMode mode)
    {
        m_cursorMode = mode;

        switch (mode)
        {
        case CursorMode::Visible:
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;

        case CursorMode::Hidden:
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;

        case CursorMode::Disabled:
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        }
    }

    void InputEngine::SetMousePosition(const Vector2& v) const
    {
        glfwSetCursorPos(glfwWindow, v.x, v.y);
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

        m_currentMouseScroll = Vector2::Zero;
        glfwPollEvents();
    }
} // namespace Lina::Input
