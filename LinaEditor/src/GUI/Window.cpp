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

#include "GUI/Window.hpp"
#include "Core/Screen.hpp"

namespace Lina::Editor
{
    void Window::Draw(float dt)
    {
        const Vector2 screen = Graphics::Screen::SizeF();

        if (m_bitmask.IsSet(WindowMask::Fullscreen))
        {
            m_min      = Vector2(0, 0);
            m_max      = screen;
            m_position = (m_max + m_min) / 2.0f;
        }

        LinaVG::DrawRect(LinaVG::Vec2(0,0), LinaVG::Vec2(500,500), m_style, m_drawOrder);

        // Children
        Node::Draw(dt);
    }
} // namespace Lina::Editor
