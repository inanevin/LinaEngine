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

#ifndef View_HPP
#define View_HPP

#include "Math/Frustum.hpp"
#include "Math/Vector.hpp"
#include "Data/HashSet.hpp"
#include "Core/RenderData.hpp"

namespace Lina::Graphics
{
    enum class ViewType
    {
        Player,
        SunShadow,
    };

    class FramePacket;
    class RenderableComponent;
    class View
    {
    public:
        void CalculateVisibility(RenderableComponent** renderables, uint32 size);

        inline ViewType GetType()
        {
            return m_viewType;
        }

        inline Vector<RenderableComponent*>& GetVisibleObjects()
        {
            return m_visibleRenderables;
        }

        inline const Vector3& GetPos() const
        {
            return m_pos;
        }
        inline const Matrix& GetView() const
        {
            return m_view;
        }

        inline const Matrix& GetProj() const
        {
            return m_proj;
        }

    private:
        friend class RenderEngine;
        friend class Renderer;

        void CalculateFrustum(const Vector3& pos, const Matrix& view, const Matrix& proj);

    private:
        Vector<RenderableComponent*> m_visibleRenderables;
        ViewType                     m_viewType = ViewType::Player;
        Matrix                       m_view;
        Matrix                       m_proj;
        Vector3                      m_pos     = Vector3::Zero;
        Frustum                      m_frustum = Frustum();
    };
} // namespace Lina::Graphics

#endif
