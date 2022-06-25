/*
Class: GLWindow

Responsible for all window operations of an Open GL window context.

Timestamp: 4/14/2019 5:12:19 PM
*/

#pragma once

#ifndef Window_HPP
#define Window_HPP

#include "Core/CommonApplication.hpp"
#include "Core/CommonWindow.hpp"
#include "Math/Vector.hpp"

struct GLFWwindow;

namespace Lina
{
    class Engine;
}

namespace Lina::Graphics
{

    class Window
    {
    public:
        static Window* Get()
        {
            return s_Window;
        }
        virtual void* GetNativeWindow() const
        {
            return m_window;
        }

        Vector2i GetSize()
        {
            return Vector2i(m_windowProperties.m_width, m_windowProperties.m_height);
        }
        Vector2i GetPos()
        {
            return Vector2i(m_windowProperties.m_xPos, m_windowProperties.m_yPos);
        }
        Vector2i GetWorkSize()
        {
            return Vector2i(m_windowProperties.m_workingAreaWidth, m_windowProperties.m_workingAreaHeight);
        }
        int GetWidth()
        {
            return m_windowProperties.m_width;
        }
        int GetHeight()
        {
            return m_windowProperties.m_height;
        }
        const WindowProperties& GetProperties() const
        {
            return m_windowProperties;
        }

        void   SetVsync(int interval);
        void   SetSize(const Vector2i& newSize);
        void   SetPos(const Vector2i& newPos);
        void   SetPosCentered(const Vector2 newPos);
        void   Iconify();
        void   Maximize();
        void   Close();
        void   Tick();
        double GetTime();


    private:
        friend class Engine;
        Window()  = default;
        ~Window() = default;
        bool CreateContext(ApplicationInfo& appInfo);
        void Shutdown();
        void Sleep(int milliseconds);

    private:
        static Window* s_Window;
        WindowProperties     m_windowProperties = WindowProperties();
        void*                m_window           = nullptr;
        GLFWwindow*          m_glfwWindow       = nullptr;
    };

} // namespace Lina::Graphics

#endif
