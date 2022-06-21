/*
Class: CommonWindow



Timestamp: 12/25/2021 12:21:41 PM
*/

#pragma once

#ifndef CommonWindow_HPP
#define CommonWindow_HPP

// Headers here.
#include "Data/String.hpp"

namespace Lina
{
    enum class WindowState
    {
        Normal    = 0,
        Maximized = 1,
        Iconified = 2
    };

    struct WindowProperties
    {
        String  m_title;
        int          m_width                = 1440;
        int          m_height               = 900;
        int          m_monitorWidth         = 0;
        int          m_monitorHeight        = 0;
        unsigned int m_xPos                 = 0;
        unsigned int m_yPos                 = 0;
        unsigned int m_xPosBeforeMaximize   = 0;
        unsigned int m_yPosBeforeMaximize   = 0;
        unsigned int m_widthBeforeMaximize  = 768;
        unsigned int m_heightBeforeMaximize = 768;
        unsigned int m_workingAreaWidth     = 768;
        unsigned int m_workingAreaHeight    = 768;
        int          m_vsync                = 0;
        bool         m_decorated            = true;
        bool         m_resizable            = true;
        bool         m_fullscreen           = false;
        int          m_msaaSamples          = 4;
        float        m_contentScaleWidth    = 1.0f;
        float        m_contentScaleHeight   = 1.0f;
        WindowState  m_windowState;

        WindowProperties()
        {
            m_title  = "Lina Engine";
            m_width  = 1440;
            m_height = 900;
        }

        WindowProperties(const String& title, unsigned int width, unsigned int height)
        {
            m_title  = title;
            m_width  = width;
            m_height = height;
        }
    };
} // namespace Lina

#endif
