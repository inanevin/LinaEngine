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

#include "Core/Window.hpp"
#include "Log/Log.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/InputEvents.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Lina::Graphics
{
    Window* Window::s_instance = nullptr;

    static void GLFWErrorCallback(int error, const char* desc)
    {
        LINA_ERR("GLFW Error: {0} Description: {1} ", error, desc);
    }

    bool Window::Initialize(const WindowProperties& props)
    {
        LINA_TRACE("[Initialization] -> Window ({0})", typeid(*this).name());

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_DECORATED, props.decorated);
        glfwWindowHint(GLFW_RESIZABLE, props.resizable);

        auto*              primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

        int width  = props.width;
        int height = props.height;
        if (props.fullscreen)
        {
            width  = mode->width;
            height = mode->height;
        }

        m_glfwWindow = (glfwCreateWindow(width, height, props.title.c_str(), props.fullscreen ? primaryMonitor : NULL, NULL));
        glfwGetMonitorContentScale(primaryMonitor, &(g_appInfo.m_contentScaleWidth), &(g_appInfo.m_contentScaleHeight));

        if (!m_glfwWindow)
        {
            // Assert window creation.
            LINA_ERR("[Window] -> GLFW could not initialize!");
            return false;
        }

        m_userPtr = static_cast<void*>(m_glfwWindow);

        // Set error callback
        glfwSetErrorCallback(GLFWErrorCallback);

        // Set working area width, exclude taskbar.
        if (g_appInfo.GetAppMode() == ApplicationMode::Editor)
        {
            int xpos, ypos, width, height;
            glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &xpos, &ypos, &width, &height);
            glfwSetWindowSizeLimits(m_glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, height);
            // SetSize(Vector2i(width, height));
        }

        SetVsync(props.vsync);
        glfwSetWindowUserPointer(m_glfwWindow, this);

        auto windowResizeFunc = [](GLFWwindow* w, int wi, int he) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            window->SetSize(Vector2i(wi, he));
        };

        auto windowCloseFunc = [](GLFWwindow* w) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            window->Close();
        };

        auto windowKeyFunc = [](GLFWwindow* w, int key, int scancode, int action, int modes) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));

            Input::InputAction inputAction = Input::InputAction::Pressed;
            if (action == GLFW_RELEASE)
                inputAction = Input::InputAction::Released;
            else if (action == GLFW_REPEAT)
                inputAction = Input::InputAction::Repeated;

            Event::EventSystem::Get()->Trigger<Event::EKeyCallback>(Event::EKeyCallback{window->m_userPtr, key, scancode, inputAction, modes});
        };

        auto windowButtonFunc = [](GLFWwindow* w, int button, int action, int modes) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));

            Input::InputAction inputAction = Input::InputAction::Pressed;
            if (action == GLFW_RELEASE)
                inputAction = Input::InputAction::Released;
            else if (action == GLFW_REPEAT)
                inputAction = Input::InputAction::Repeated;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{window->m_userPtr, button, inputAction, modes});
        };

        auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            Event::EventSystem::Get()->Trigger<Event::EMouseScrollCallback>(Event::EMouseScrollCallback{window->m_userPtr, xOff, yOff});
        };

        auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            Event::EventSystem::Get()->Trigger<Event::EMouseCursorCallback>(Event::EMouseCursorCallback{window->m_userPtr, xPos, yPos});
        };

        auto windowFocusFunc = [](GLFWwindow* w, int f) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            Event::EventSystem::Get()->Trigger<Event::EWindowFocused>(Event::EWindowFocused{window->m_userPtr, f});
        };

        // Register window callbacks.
        glfwSetFramebufferSizeCallback(m_glfwWindow, windowResizeFunc);
        glfwSetWindowCloseCallback(m_glfwWindow, windowCloseFunc);
        glfwSetKeyCallback(m_glfwWindow, windowKeyFunc);
        glfwSetMouseButtonCallback(m_glfwWindow, windowButtonFunc);
        glfwSetScrollCallback(m_glfwWindow, windowMouseScrollFunc);
        glfwSetCursorPosCallback(m_glfwWindow, windowCursorPosFunc);
        glfwSetWindowFocusCallback(m_glfwWindow, windowFocusFunc);

        Event::EventSystem::Get()->Trigger<Event::EWindowContextCreated>(Event::EWindowContextCreated{.window = m_userPtr});
        m_size  = Vector2i(props.width, props.height);
        m_pos   = Vector2(0, 0);
        m_vsync = props.vsync;
        return true;
    }

    void Window::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Window ({0})", typeid(*this).name());

        glfwDestroyWindow(m_glfwWindow);
        glfwTerminate();
    }

    void Window::SetSize(const Vector2i& size)
    {
        glfwSetWindowSize(m_glfwWindow, size.x, size.y);
        Event::EventSystem::Get()->Trigger<Event::EWindowResized>(Event::EWindowResized{.window = m_userPtr, .newSize = size});
        m_size      = size;
        m_minimized = size.x == 0 || size.y == 0;
    }

    void Window::SetPos(const Vector2i& pos)
    {
        glfwSetWindowPos(m_glfwWindow, pos.x, pos.y);
        Event::EventSystem::Get()->Trigger<Event::EWindowPositioned>(Event::EWindowPositioned{.window = m_userPtr, .newPos = pos});
        m_pos = pos;
    }

    void Window::SetPosCentered(const Vector2i& newPos)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        SetPos(Vector2i((int)((float)mode->width / 2.0f + (float)newPos.x), (int)((float)mode->height / 2.0f + (float)newPos.y)));
    }

    void Window::SetVsync(VsyncMode mode)
    {
        m_vsync = mode;
        Event::EventSystem::Get()->Trigger<Event::EVsyncModeChanged>(Event::EVsyncModeChanged{.newMode = mode});
    }

    void Window::Close()
    {
        Event::EventSystem::Get()->Trigger<Event::EWindowClosed>();
    }

} // namespace Lina::Graphics
