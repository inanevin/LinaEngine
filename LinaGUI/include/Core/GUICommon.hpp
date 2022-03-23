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
Class: GUICommon



Timestamp: 3/22/2022 11:08:45 PM
*/

#pragma once

#ifndef GUICommon_HPP
#define GUICommon_HPP

// Headers here.
#include <imgui/imgui.h>
#include "Math/Vector.hpp"
#include "Math/Color.hpp"

namespace Lina::GUI
{
#define IMVEC2(V)   ImVec2(V.x, V.y)
#define IMVEC4(V)   ImVec4(V.x, V.y, V.z, V.w)
#define IMCOL(V)    ImVec4(V.r, V.g, V.b, V.a)
#define IMCOL32(V)  ImGui::ColorConvertFloat4ToU32(IMCOL(V))
#define LINAVEC2(V) Vector2(V.x, V.y)
#define LINAVEC4(V) Vector4(V.x, V.y)
#define LINACOL(V)  Color(V.x, V.y, V.z, V.w)

    enum class BackgroundStyle
    {
        SingleColor,
        GradientHorizontal,
        GradientVertical
    };

    enum class BorderStyle
    {
        None,
        Simple,
        Animated
    };

    enum class DragStyle
    {
        Free,
        ConfinedInParent,
    };

} // namespace Lina::GUI

#endif
