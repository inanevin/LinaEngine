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

#include "GUI/Theme.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
    float Theme::GetProperty(ThemeProperty p)
    {
        bool shouldScale = p == ThemeProperty::WindowItemSpacingX || p == ThemeProperty::WindowItemSpacingY || p == ThemeProperty::WindowItemPaddingX || p == ThemeProperty::WindowItemPaddingY;
        return shouldScale ? m_properties[p] * LinaVG::Config.framebufferScale.x : m_properties[p];
    }

    void Theme::PushSetColor(ThemeColor tc, Color c)
    {
        StoredColor sc;
        sc.tc  = tc;
        sc.col = m_colors[tc];
        m_storedColors.push_back(sc);
        m_colors[tc] = c;
    }

    void Theme::PushSetColor(ThemeColor tc, ThemeColor fromTc)
    {
        StoredColor sc;
        sc.tc  = tc;
        sc.col = m_colors[tc];
        m_storedColors.push_back(sc);
        m_colors[tc] = m_colors[fromTc];
    }

    void Theme::PopStoredColor()
    {
        StoredColor restore  = m_storedColors.back();
        m_colors[restore.tc] = restore.col;
        m_storedColors.pop_back();
    }

    void Theme::PushSetProperty(ThemeProperty tp, float val)
    {
        StoredProperty sp;
        sp.tp  = tp;
        sp.val = m_properties[tp];
        m_storedProperties.push_back(sp);
        m_properties[tp] = val;

        if (tp == ThemeProperty::AAEnabled)
            LinaVG::Config.aaEnabled = val == 1.0f;
    }

    void Theme::PopStoredProperty()
    {
        StoredProperty restored   = m_storedProperties.back();
        m_properties[restored.tp] = restored.val;
        m_storedProperties.pop_back();

        if (restored.tp == ThemeProperty::AAEnabled)
            LinaVG::Config.aaEnabled = restored.val == 1.0f;
    }

} // namespace Lina::Editor
