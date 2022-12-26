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

#include "Graphics/Core/Renderer.hpp"
#include "Graphics/PipelineObjects/Swapchain.hpp"
#include "Graphics/PipelineObjects/CommandPool.hpp"
#include "Graphics/Core/RenderData.hpp"
#include "Profiling/Profiler.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

namespace Lina::Graphics
{
    void Renderer::UpdateViewport(const Vector2i& size)
    {
        m_viewport.width             = static_cast<float>(size.x);
        m_viewport.height            = static_cast<float>(size.y);
        m_scissors.size              = size;
        LinaVG::Config.displayWidth  = static_cast<unsigned int>(m_viewport.width);
        LinaVG::Config.displayHeight = static_cast<unsigned int>(m_viewport.height);
    }
} // namespace Lina::Graphics
