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

#include "Layout/Container.hpp"

namespace Lina::GUI
{
    void Container::Render()
    {
        if (m_backgroundStyle == BackgroundStyle::SingleColor)
            ImGui::GetWindowDrawList()->AddRectFilled(IMVEC2(m_min), IMVEC2(m_max), IMCOL32(m_backgroundColor), m_rounding);
        else if (m_backgroundStyle == BackgroundStyle::GradientHorizontal)
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(IMVEC2(m_min), IMVEC2(m_max), IMCOL32(m_backgroundColor), IMCOL32(m_backgroundColor2), IMCOL32(m_backgroundColor2), IMCOL32(m_backgroundColor));
        else if (m_backgroundStyle == BackgroundStyle::GradientVertical)
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(IMVEC2(m_min), IMVEC2(m_max), IMCOL32(m_backgroundColor), IMCOL32(m_backgroundColor), IMCOL32(m_backgroundColor2), IMCOL32(m_backgroundColor2));

        if (m_borderStyle == BorderStyle::Simple)
        {
            ImGui::GetWindowDrawList()->AddRect(IMVEC2(m_min), IMVEC2(m_max), IMCOL32(m_borderColor), m_rounding, 0, m_borderThickness);
        }
        else if (m_borderStyle == BorderStyle::Animated)
        {
            // TODO
        }
        BaseGUIElement::Render();
    }
} // namespace Lina::GUI