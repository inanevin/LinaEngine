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

#include "Core/GNode.hpp"
#include "Core/CommonEngine.hpp"

namespace Lina::GUI
{
    void GNode::AddChild(GNode* child)
    {
        child->parent = this;

        if (child->drawOrder <= drawOrder)
            child->drawOrder = drawOrder + 1;

        children.push_back(child);
    }

    GNode::~GNode()
    {
        for (auto c : children)
            delete c;

        children.clear();
    }

    void GNode::RemoveChild(GNode* child)
    {
        child->parent = nullptr;

        Vector<GNode*>::iterator it = children.begin();

        for (; it < children.end(); ++it)
        {
            if (*it == child)
            {
                children.erase(it);
                break;
            }
        }
    }

    Vector2 GNode::GetAbsPosition()
    {
        if (parent != nullptr)
            return RuntimeInfo::GetScreenSizeF() * position;

        return parent->_absPosition + parent->_absSize * position;
    }

    Vector2 GNode::GetAbsSize()
    {
        if (parent != nullptr)
            return RuntimeInfo::GetScreenSizeF() * size;

        return parent->_absSize * size;
    }

    void GNode::Calculate()
    {
        _absPosition = GetAbsPosition();
        _absSize     = GetAbsSize();
        _absHalfSize = _absSize * 0.5f;
        _min         = _absPosition - _absHalfSize;
        _max         = _absPosition + _absHalfSize;
    }

    void GNode::Draw()
    {
        for (auto& n : children)
            n->Draw();
    }

} // namespace Lina::GUI
