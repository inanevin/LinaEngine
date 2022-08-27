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

#include "Core/View.hpp"
#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{
    void View::CalculateVisibility(RenderableComponent** renderables, uint32 size)
    {
        PROFILER_FUNC("Render View");
        m_visibleRenderables.clear();
        m_visibleRenderables.resize(size, nullptr);

        Taskflow tf;
        tf.for_each_index(0, static_cast<int>(size), 1, [&](int i) {
            RenderableComponent* rend = renderables[i];
            if (rend == nullptr)
                return;
            FrustumTest test = m_frustum.TestIntersection(rend->GetAABB());

            // if visible for this view.
            // if (test != FrustumTest::Outside)
            m_visibleRenderables[i] = rend;
        });

        JobSystem::Get()->GetMainExecutor().Run(tf).wait();
    }

    void View::CalculateFrustum(const Vector3& pos, const Matrix& view, const Matrix& proj)
    {
        m_pos  = pos;
        m_view = view;
        m_proj = proj;
        m_frustum.Calculate(proj * view, false);
    }
} // namespace Lina::Graphics
