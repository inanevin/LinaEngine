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

#include "Core/CommonInput.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/InputEvents.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "Log/Log.hpp"
#include "glad/glad.h"

// After GLAD
#include "GLFW/glfw3.h"

namespace Lina::Graphics
{
    Window* Window::s_Window = nullptr;

    void Window::Tick()
    {
        if (!glfwWindowShouldClose(m_glfwWindow))
            glfwSwapBuffers(m_glfwWindow);
    }

    static void GLFWErrorCallback(int error, const char* desc)
    {
        LINA_ERR("GLFW Error: {0} Description: {1} ", error, desc);
    }

    bool Window::CreateContext(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Window ({0})", typeid(*this).name());

        // Set props.
        m_windowProperties = appInfo.m_windowProperties;

        // Initialize glfw & set window hints
        int init = glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, m_windowProperties.m_msaaSamples);
        glfwWindowHint(GLFW_DECORATED, m_windowProperties.m_decorated);
        glfwWindowHint(GLFW_RESIZABLE, m_windowProperties.m_resizable);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // Build window
        auto*              primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

        m_windowProperties.m_monitorWidth  = mode->width;
        m_windowProperties.m_monitorHeight = mode->height;

        if (m_windowProperties.m_fullscreen)
        {
            m_windowProperties.m_width  = m_windowProperties.m_monitorWidth;
            m_windowProperties.m_height = m_windowProperties.m_monitorHeight;
        }

        m_glfwWindow = (glfwCreateWindow(m_windowProperties.m_width, m_windowProperties.m_height, m_windowProperties.m_title.c_str(), m_windowProperties.m_fullscreen ? primaryMonitor : NULL, NULL));
        glfwGetMonitorContentScale(primaryMonitor, &(m_windowProperties.m_contentScaleWidth), &(m_windowProperties.m_contentScaleHeight));

        if (!m_glfwWindow)
        {
            // Assert window creation.
            LINA_ERR("GLFW could not initialize!");
            return false;
        }
        // Set error callback
        glfwSetErrorCallback(GLFWErrorCallback);

        // Set context.
        glfwMakeContextCurrent(m_glfwWindow);
        // Load glad
        bool loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (!loaded)
        {
            LINA_ERR("GLAD Loader failed!");
            return false;
        }

        SetPos(Vector2i(0, 0));

        if (appInfo.m_appMode == ApplicationMode::Editor)
        {
            int xpos, ypos, width, height;
            glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &xpos, &ypos, &width, &height);
            glfwSetWindowSizeLimits(m_glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, height);
            m_windowProperties.m_workingAreaWidth  = width;
            m_windowProperties.m_workingAreaHeight = height;
            SetSize(Vector2i(width, height));
        }
        else
        {
            m_windowProperties.m_workingAreaWidth  = m_windowProperties.m_width;
            m_windowProperties.m_workingAreaHeight = m_windowProperties.m_height;
        }

        // Update OpenGL about the window data.
        glViewport(0, 0, m_windowProperties.m_width, m_windowProperties.m_height);

        SetVsync(m_windowProperties.m_vsync);

        // set user pointer for callbacks.
        glfwSetWindowUserPointer(m_glfwWindow, this);

        auto windowResizeFunc = [](GLFWwindow* w, int wi, int he) {
            auto* window                        = static_cast<Window*>(glfwGetWindowUserPointer(w));
            window->m_windowProperties.m_width  = wi;
            window->m_windowProperties.m_height = he;
            Event::EventSystem::Get()->Trigger<Event::EWindowResized>(Event::EWindowResized{window->m_window, window->m_windowProperties});
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

            Event::EventSystem::Get()->Trigger<Event::EKeyCallback>(Event::EKeyCallback{window->m_window, key, scancode, inputAction, modes});
        };

        auto windowButtonFunc = [](GLFWwindow* w, int button, int action, int modes) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));

            Input::InputAction inputAction = Input::InputAction::Pressed;
            if (action == GLFW_RELEASE)
                inputAction = Input::InputAction::Released;
            else if (action == GLFW_REPEAT)
                inputAction = Input::InputAction::Repeated;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{window->m_window, button, inputAction, modes});
        };

        auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            Event::EventSystem::Get()->Trigger<Event::EMouseScrollCallback>(Event::EMouseScrollCallback{window->m_window, xOff, yOff});
        };

        auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            Event::EventSystem::Get()->Trigger<Event::EMouseCursorCallback>(Event::EMouseCursorCallback{window->m_window, xPos, yPos});
        };

        auto windowFocusFunc = [](GLFWwindow* w, int f) {
            auto* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
            Event::EventSystem::Get()->Trigger<Event::EWindowFocused>(Event::EWindowFocused{window->m_window, f});
        };

        // Register window callbacks.
        glfwSetFramebufferSizeCallback(m_glfwWindow, windowResizeFunc);
        glfwSetWindowCloseCallback(m_glfwWindow, windowCloseFunc);
        glfwSetKeyCallback(m_glfwWindow, windowKeyFunc);
        glfwSetMouseButtonCallback(m_glfwWindow, windowButtonFunc);
        glfwSetScrollCallback(m_glfwWindow, windowMouseScrollFunc);
        glfwSetCursorPosCallback(m_glfwWindow, windowCursorPosFunc);
        glfwSetWindowFocusCallback(m_glfwWindow, windowFocusFunc);

        m_window = static_cast<void*>(m_glfwWindow);
        Event::EventSystem::Get()->Trigger<Event::EWindowContextCreated>(Event::EWindowContextCreated{m_window});
        appInfo.m_windowProperties = m_windowProperties;
        return true;
    }

    void Window::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Window ({0})", typeid(*this).name());

        glfwTerminate();
    };

    void Window::SetVsync(int interval)
    {
        m_windowProperties.m_vsync = interval;
        glfwSwapInterval(interval);
    }

    double Window::GetTime()
    {
        return glfwGetTime();
    }

    void Window::SetSize(const Vector2i& newSize)
    {
        glfwSetWindowSize(m_glfwWindow, newSize.x, newSize.y);
        m_windowProperties.m_width  = newSize.x;
        m_windowProperties.m_height = newSize.y;
    }

    void Window::SetPos(const Vector2i& newPos)
    {
        m_windowProperties.m_xPos = newPos.x;
        m_windowProperties.m_yPos = newPos.y;
        glfwSetWindowPos(m_glfwWindow, newPos.x, newPos.y);
    }

    void Window::SetPosCentered(const Vector2 newPos)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        SetPos(Vector2i((int)((float)mode->width / 2.0f + (float)newPos.x), (int)((float)mode->height / 2.0f + (float)newPos.y)));
    }

    void Window::Iconify()
    {
        m_windowProperties.m_windowState = WindowState::Iconified;
        glfwIconifyWindow(m_glfwWindow);
    }

    void Window::Maximize()
    {
        if (m_windowProperties.m_windowState != WindowState::Maximized)
        {
            m_windowProperties.m_widthBeforeMaximize  = m_windowProperties.m_width;
            m_windowProperties.m_heightBeforeMaximize = m_windowProperties.m_height;
            m_windowProperties.m_xPosBeforeMaximize   = m_windowProperties.m_xPos;
            m_windowProperties.m_yPosBeforeMaximize   = m_windowProperties.m_yPos;
            SetPos(Vector2i(0, 0));
            SetSize(Vector2i(m_windowProperties.m_workingAreaWidth, m_windowProperties.m_workingAreaHeight));
            m_windowProperties.m_windowState = WindowState::Maximized;
            // glfwMaximizeWindow(m_glfwWindow);
        }
        else
        {
            SetPos(Vector2i(m_windowProperties.m_xPosBeforeMaximize, m_windowProperties.m_yPosBeforeMaximize));
            SetSize(Vector2i(m_windowProperties.m_widthBeforeMaximize, m_windowProperties.m_heightBeforeMaximize));
            m_windowProperties.m_windowState = WindowState::Normal;
            // glfwRestoreWindow(m_glfwWindow);
        }
    }

    void Window::Close()
    {
        Event::EventSystem::Get()->Trigger<Event::EWindowClosed>(Event::EWindowClosed{m_window});
    }

    void Window::Sleep(int milliseconds)
    {
        glfwWaitEventsTimeout((float)milliseconds / 1000.0f);
    }

} // namespace Lina::Graphics
