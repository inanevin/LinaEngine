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

#ifndef GUINode_HPP
#define GUINode_HPP

#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Math/Vector.hpp"

namespace Lina::GUI
{
    enum class ParentConstraintType
    {
        SizeX,
        SizeY,
        PosX,
        PosY,
        SizeXToY,
        SizeYToX,
    };

    enum class AbsConstraintType
    {
        MinSizeX,
        MinSizeY,
        MaxSizeX,
        MaxSizeY,
        MinPosX,
        MinPosY,
        MaxPosX,
        MaxPosY,
    };

    struct ParentConstraint
    {
        ParentConstraintType type;
        float                val = 0.0f;
    };

    struct AbsConstraint
    {
        AbsConstraintType type;
        float             val = 0.0f;
    };

    class GUINode
    {
    public:
        GUINode() = default;
        virtual ~GUINode();

        void         AddChild(GUINode* child);
        void         RemoveChild(GUINode* child);
        GUINode*     AddParentConstraint(ParentConstraintType type, float value);
        GUINode*     AddAbsConstraint(AbsConstraintType type, float value);
        void         RemoveParentConstraint(ParentConstraintType type);
        void         RemoveAbsConstraint(AbsConstraintType type);
        virtual void Draw();
        void         Calculate();

        String  name        = "";
        uint32  drawOrder   = 0;
        bool    enableAA    = true;
        float   rotateAngle = 0.0f;
        Vector2 position    = Vector2();
        Vector2 size        = Vector2();

        // Debug
        bool _showBoundingBox = false;

    protected:
        void DebugDraw();

    protected:
        GUINode*                 m_parent = nullptr;
        Vector<GUINode*>         m_children;
        Vector<ParentConstraint> m_parentConstraints;
        Vector<AbsConstraint>    m_absConstraints;
    };
} // namespace Lina::GUI

#endif
