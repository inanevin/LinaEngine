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

#include "Primitives/GConvex.hpp"

namespace Lina::GUI
{
    void GConvex::Draw()
    {
        const bool aa            = LinaVG::Config.aaEnabled;
        LinaVG::Config.aaEnabled = enableAA;
        Vector<LinaVG::Vec2> _points;

        for (auto& pp : pointPositions)
        {
            if (parent != nullptr)
                _points.push_back(LV2((parent->_absPosition + parent->_absSize * pp)));
            else
                _points.push_back(LV2((RuntimeInfo::GetScreenSizeF() * pp)));
        }

        LinaVG::DrawConvex(_points.data(), _points.size(), style, rotateAngle, drawOrder);
        _points.clear();
        LinaVG::Config.aaEnabled = aa;
    }
} // namespace Lina::GUI
