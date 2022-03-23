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
Class: Container



Timestamp: 3/22/2022 10:56:29 PM
*/

#pragma once

#ifndef Container_HPP
#define Container_HPP

// Headers here.
#include "Core/BaseGUIElement.hpp"
#include "Math/Color.hpp"

namespace Lina::GUI
{
    class Container : public BaseGUIElement
    {

    public:
        Container(){};

        virtual void Render() override;

        float           m_borderThickness  = 0.0f;
        float           m_rounding         = 0.0f;
        Color           m_backgroundColor  = Color(0, 0, 0, 1);
        Color           m_backgroundColor2 = Color(0, 0, 0, 1);
        Color           m_borderColor      = Color(0, 0, 0, 1);
        BackgroundStyle m_backgroundStyle  = BackgroundStyle::SingleColor;
        BorderStyle     m_borderStyle      = BorderStyle::None;
    };
} // namespace Lina::GUI

#endif
