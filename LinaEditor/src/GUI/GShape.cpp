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

#include "GUI/GShape.hpp"
#include "Core/Screen.hpp"
#include "Utility/GUIParseUtility.hpp"

namespace Lina::Editor
{
    void GShape::ParseFromJSON(json& data)
    {
        const String shape = data.value("Shape", "Rect").c_str();

        if (shape.compare("Rect") == 0)
            m_shapeType = ShapeType::Rect;
        else if (shape.compare("Circle") == 0)
            m_shapeType = ShapeType::Circle;
        else if (shape.compare("Convex") == 0)
            m_shapeType = ShapeType::Convex;

        json convexPoints = data.value("Points", json());

        for (auto& e : convexPoints)
            m_convexPoints.push_back(Vector2(e.value("X", 0.0f), e.value("Y", 0.0f)));

        GNode::ParseFromJSON(data);
    }

    void GShape::Draw(float dt)
    {
        const bool aa            = LinaVG::Config.aaEnabled;
        LinaVG::Config.aaEnabled = aaEnabled;

        if (m_shapeType == ShapeType::Rect)
            LinaVG::DrawRect(LV2(_min), LV2((_max)), style, 0, _drawOrder);
        else
        {
            Vector<LinaVG::Vec2> points;

            for (auto v : m_convexPoints)
                points.push_back(LV2((parent->_absPosition + parent->_absSize * v)));

            LinaVG::DrawConvex(points.data(), points.size(), style, 0.0f, _drawOrder);
        }

        LinaVG::Config.aaEnabled = aa;

        GNode::Draw(dt);
    }

} // namespace Lina::Editor
