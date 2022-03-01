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

/*
Class: CommonWindow



Timestamp: 12/25/2021 12:21:41 PM
*/

#pragma once

#ifndef CommonWindow_HPP
#define CommonWindow_HPP

// Headers here.
#include <string>

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
        std::string  m_title;
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
        WindowState  m_windowState;

        WindowProperties()
        {
            m_title  = "Lina Engine";
            m_width  = 1440;
            m_height = 900;
        }

        WindowProperties(const std::string& title, unsigned int width, unsigned int height)
        {
            m_title  = title;
            m_width  = width;
            m_height = height;
        }
    };
} // namespace Lina

#endif
