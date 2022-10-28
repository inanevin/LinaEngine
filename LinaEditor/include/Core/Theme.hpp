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

#pragma once

#ifndef Theme_HPP
#define Theme_HPP

#include "Math/Color.hpp"
#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Core/SizeDefinitions.hpp"

namespace Lina::Editor
{

#define TC_DARK     Color(20.0f, 20.0f, 20.0f, 255.0f, true)
#define TC_VERYDARK Color(14.0f, 14.0f, 14.0f, 255.0f, true)
#define TC_OUTLINE1 Color(14.0f, 14.0f, 14.0f, 255.0f, true)

    enum class ThemeFont
    {
        Default,
        LinaStyle,
    };

    class Theme
    {
    public:
        inline static uint32 GetFont(ThemeFont font)
        {
            return s_fonts[font];
        }

    private:
        friend class EditorRenderer;

        static HashMap<ThemeFont, uint32> s_fonts;
    };
} // namespace Lina::Editor

#endif
