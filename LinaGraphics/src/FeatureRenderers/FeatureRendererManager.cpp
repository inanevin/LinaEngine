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

#include "FeatureRenderers/FeatureRendererManager.hpp"
#include "Core/FramePacket.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/Renderer.hpp"

namespace Lina::Graphics
{

    void FeatureRendererManager::ExtractGameState(World::EntityWorld* w)
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        Taskflow tf;

        // Extract
        tf.for_each(onExtract.begin(), onExtract.end(), [&](auto&& f) { f(w); });
        JobSystem::Get()->GetMainExecutor().Run(tf).wait();
        tf.clear();

        // Per view
        const Vector<View*>& views = m_renderer->GetViews();
        for (auto v : views)
        {
            // Extract per view
            tf.for_each(onExtractPerView.begin(), onExtractPerView.end(), [w, v](auto&& f) { f(w, v); });
            JobSystem::Get()->GetMainExecutor().Run(tf).wait();
            tf.clear();

            // Extract per view end.
            tf.for_each(onExtractPerViewEnd.begin(), onExtractPerViewEnd.end(), [w, v](auto&& f) { f(w, v); });
            JobSystem::Get()->GetMainExecutor().Run(tf).wait();
            tf.clear();
        }

        // Extract end.
        tf.for_each(onExtractEnd.begin(), onExtractEnd.end(), [&](auto&& f) { f(w); });
        JobSystem::Get()->GetMainExecutor().Run(tf).wait();
        tf.clear();
    }

    void FeatureRendererManager::PrepareRenderData()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        Taskflow tf;

        // Prepare
        tf.for_each(onPrepare.begin(), onPrepare.end(), [](auto&& f) { f(); });
        JobSystem::Get()->GetMainExecutor().Run(tf).wait();
        tf.clear();

        // Per view
        const Vector<View*>& views = m_renderer->GetViews();
        for (auto v : views)
        {
            // Prepare per view
            tf.for_each(onPreparePerView.begin(), onPreparePerView.end(), [v](auto&& f) { f(v); });
            JobSystem::Get()->GetMainExecutor().Run(tf).wait();
            tf.clear();

            // Prepare per view end.
            tf.for_each(onPreparePerViewEnd.begin(), onPreparePerViewEnd.end(), [v](auto&& f) { f(v); });
            JobSystem::Get()->GetMainExecutor().Run(tf).wait();
            tf.clear();
        }

        // Prepare end.
        tf.for_each(onPrepareEnd.begin(), onPrepareEnd.end(), [](auto&& f) { f(); });
        JobSystem::Get()->GetMainExecutor().Run(tf).wait();
        tf.clear();
    }

    void FeatureRendererManager::Submit(CommandBuffer& cb)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        Taskflow tf;

        // Submit per view.
        const Vector<View*>& views = m_renderer->GetViews();
        for (auto v : views)
            linatl::for_each(onSubmit.begin(), onSubmit.end(), [&](auto&& f) { f(cb, v); });
    }

} // namespace Lina::Graphics
