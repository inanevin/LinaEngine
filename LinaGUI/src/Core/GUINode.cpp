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

#include "Core/GUINode.hpp"
#include "Core/CommonEngine.hpp"

namespace Lina::GUI
{
    void GUINode::AddChild(GUINode* child)
    {
        child->m_parent = this;

        if (child->drawOrder <= drawOrder)
            child->drawOrder = drawOrder + 1;

        m_children.push_back(child);
    }

    GUINode::~GUINode()
    {
        for (auto c : m_children)
            delete c;

        m_children.clear();
    }

    void GUINode::RemoveChild(GUINode* child)
    {
        child->m_parent = nullptr;

        Vector<GUINode*>::iterator it = m_children.begin();

        for (; it < m_children.end(); ++it)
        {
            if (*it == child)
            {
                m_children.erase(it);
                break;
            }
        }
    }

    void GUINode::Calculate()
    {
        const Vector2 screen  = RuntimeInfo::GetScreenSizeF();
        const Vector2 refSize = m_parent ? m_parent->size : screen;

        for (auto& c : m_parentConstraints)
        {
            if (c.type == ParentConstraintType::SizeX)
                size.x = refSize.x * c.val;
            else if (c.type == ParentConstraintType::SizeY)
                size.y = refSize.y * c.val;
            else if (c.type == ParentConstraintType::PosX)
            {
                if (m_parent)
                    position.x = m_parent->position.x;

                position.x += refSize.x * c.val;
            }
            else if (c.type == ParentConstraintType::PosY)
            {
                if (m_parent)
                    position.y = m_parent->position.y;

                position.y += refSize.y * c.val;
            }
        }
    }

    void GUINode::AddParentConstraint(ParentConstraintType type, float value)
    {
        m_parentConstraints.push_back(ParentConstraint{.type = type, .val = value});
    }

    void GUINode::RemoveParentConstraint(ParentConstraintType type)
    {
        auto it = m_parentConstraints.begin();

        for (; it < m_parentConstraints.end(); ++it)
        {
            if (it->type == type)
            {
                m_parentConstraints.erase(it);
                break;
            }
        }
    }

    void GUINode::Draw()
    {
        for (auto& n : m_children)
        {
            n->Calculate();
            n->Draw();
        }
    }

} // namespace Lina::GUI
