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

#include "GUI/GNode.hpp"
#include "Core/Screen.hpp"
#include "Utility/GUIParseUtility.hpp"

namespace Lina::Editor
{
    GNode::~GNode()
    {
        for (auto& n : children)
            delete n;

        children.clear();
    }

    void GNode::Draw(float dt)
    {
        LinaVG::StyleOptions opts;
        opts.color = LinaVG::Vec4(0, 0, 0, 1);
        LinaVG::DrawRect(LV2(_min), LV2((_max)), opts);
        for (auto& c : children)
            c->Draw(dt);
    }

    void GNode::ParseFromJSON(json& data)
    {
       // for (auto& n : children)
       //     delete n;
       //
       // children.clear();
       //
       // json pos      = data.value("Pos", json());
       // json size     = data.value("Size", json());
       // json minSize  = data.value("MinSize", json());
       // json maxSize  = data.value("MaxSize", json());
       // json anchorJs = data.value("Anchor", json());
       //
       // name       = data.value("Name", "noname").c_str();
       // posPerc    = pos.empty() ? Vector2() : Vector2(pos.value("X", 0.0f), pos.value("Y", 0.0f));
       // sizePerc   = size.empty() ? Vector2() : Vector2(size.value("X", 0.0f), size.value("Y", 0.0f));
       // minAbsSize = minSize.empty() ? Vector2() : Vector2(minSize.value("X", 0.0f), minSize.value("Y", 0.0f));
       // maxAbsSize = maxSize.empty() ? Vector2() : Vector2(maxSize.value("X", 0.0f), maxSize.value("Y", 0.0f));
       // anchor     = anchorJs.empty() ? Vector2() : Vector2(anchorJs.value("X", 0.0f), anchorJs.value("Y", 0.0f));
       // aaEnabled  = data.value("AA", false);
       //
       // // Style
       // json jstyle = data.value("Style", json());
       // style       = jstyle.empty() ? LinaVG::StyleOptions() : jstyle.get<LinaVG::StyleOptions>();
       //
       // json children = data.value("Children", json());
       //
       // for (auto& e : children)
       // {
       //     GNode* child = nullptr;
       //     if (e.is_string())
       //         child = GUIParseUtility::LoadFromFile(e.get<std::string>().c_str());
       //     else
       //     {
       //         const String type = e.value("Node", "").c_str();
       //         child             = GUIParseUtility::CreateFromTypeStr(type);
       //         child->ParseFromJSON(e);
       //     }
       //
       //     if (child != nullptr)
       //         AddChild(child);
       // }
    }

    void GNode::AddChild(GNode* node)
    {
        node->parent = this;
        node->Setup();
        children.push_back(node);
    }

    void GNode::RemoveChild(GNode* node)
    {
        Vector<GNode*>::iterator it = children.begin();

        for (; it < children.end(); it++)
        {
            if (*it == node)
            {
                node->parent = nullptr;
                children.erase(it);
            }
        }
    }

    Vector2 GNode::GetAbsPosition()
    {
        if (parent == nullptr)
        {
            const Vector2 screen = Graphics::Screen::SizeF();
            return screen * posPerc;
        }

        return parent->_absPosition + parent->_absSize * posPerc;
    }

    Vector2 GNode::GetAbsSize()
    {
        if (parent == nullptr)
        {
            const Vector2 screen = Graphics::Screen::SizeF();
            return minAbsSize.Max(screen * sizePerc);
        }

        Vector2 size = minAbsSize.Max(parent->_absSize * sizePerc);

        if (maxAbsSize != Vector2::Zero)
            size = maxAbsSize.Min(size);
        return size;
    }

    uint32 GNode::GetDrawOrder()
    {
        if (parent == nullptr)
            return 0;

        return parent->GetDrawOrder() + 1;
    }

    void GNode::Calc()
    {
        _drawOrder   = GetDrawOrder();
        _absPosition = GetAbsPosition();
        _absSize     = GetAbsSize();
        _absHalfSize = _absSize * 0.5f;
        _min         = _absPosition;
        _max         = _absPosition + _absSize;
        _mid         = (_max + _min) / 2.0f;
        _topMid      = (_min + Vector2(_max.x, _min.y)) / 2.0f;
        _botMid      = (Vector2(_min.x, _max.y) + _max) / 2.0f;

        for (auto& c : children)
            c->Calc();
    }
} // namespace Lina::Editor
