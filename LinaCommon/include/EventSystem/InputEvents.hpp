/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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
Class: InputEvents



Timestamp: 12/25/2021 12:20:28 PM
*/

#pragma once

#ifndef InputEvents_HPP
#define InputEvents_HPP

// Headers here.
#include "Core/CommonInput.hpp"

namespace Lina::Event
{
    struct EKeyCallback
    {
        void*              m_window;
        int                m_key;
        int                m_scancode;
        Input::InputAction m_action;
        int                m_mods;
    };
    struct EMouseButtonCallback
    {
        void*              m_window;
        int                m_button;
        Input::InputAction m_action;
        int                m_mods;
    };
    struct EMouseScrollCallback
    {
        void*  m_window;
        double m_xoff;
        double m_yoff;
    };
    struct EMouseCursorCallback
    {
        void*  m_window;
        double m_xpos;
        double m_ypos;
    };
} // namespace Lina::Event

#endif
