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

#ifndef GNode_HPP
#define GNode_HPP

#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Math/Vector.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::GUI
{
    class GNode
    {
    public:
        GNode() = default;
        virtual ~GNode();

        void         AddChild(GNode* child);
        void         RemoveChild(GNode* child);
        Vector2      GetAbsPosition();
        Vector2      GetAbsSize();
        virtual void Calculate();
        virtual void Draw();

        GNode*         parent = nullptr;
        Vector<GNode*> children;
        String         name         = "";
        uint32         drawOrder    = 0;
        bool           enableAA     = true;
        float          rotateAngle  = 0.0f;
        Vector2        position     = Vector2();
        Vector2        size         = Vector2();
        Vector2        _absPosition = Vector2();
        Vector2        _absSize     = Vector2();
        Vector2        _absHalfSize = Vector2();
        Vector2        _min         = Vector2();
        Vector2        _max         = Vector2();
    };
} // namespace Lina::GUI

#endif
