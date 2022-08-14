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
#include "ECS/Components/EntityDataComponent.hpp"
#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{
    void FeatureRendererManager::RegisterDefaultStage(FeatureRendererDefaultStage stage, FeatureRendererEmptyFunction&& func)
    {
        m_defaultStages[stage].push_back(func);
    }

    void FeatureRendererManager::RegisterViewStage(FeatureRendererViewStage stage, FeatureRendererViewFunction&& func)
    {
        m_viewStages[stage].push_back(func);
    }

    void FeatureRendererManager::RegisterVisibility(FeatureRendererVisibilityStage stage, FeatureRendererEmptyFunction&& func)
    {
        m_visibilityStages[stage].push_back(func);
    }

    void FeatureRendererManager::RegisterSubmit(FeatureRendererSubmitStage stage, FeatureRendererCBFunction&& func)
    {
        m_submitStages[stage].push_back(func);
    }

    void FeatureRendererManager::FetchVisibility()
    {
        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);
        Taskflow tf;

        // Fetch visibility, each feature renderer will check for their own components & acquire a list of visible entities.
        const auto& fetchVisibility = m_visibilityStages[FeatureRendererVisibilityStage::FetchVisibility];
        tf.for_each(fetchVisibility.begin(), fetchVisibility.end(), [](auto&& f) { f(); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();

        // Single thread, each feature renderer registered will add their own visibility data to this combined list.
        const auto& vec = m_visibilityStages[FeatureRendererVisibilityStage::AssignVisibility];
        for (auto&& f : vec)
            f();

        // Calculate visibility for each view.
        const Vector<View*>& views = RenderEngine::Get()->GetViews();
        tf.for_each(views.begin(), views.end(), [](View* v) { v->CalculateVisibility(); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();

    }

    void FeatureRendererManager::ExtractGameState()
    {
        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);
        Taskflow tf;

        // Extract
        const auto& onExtract = m_defaultStages[FeatureRendererDefaultStage::OnExtract];
        tf.for_each(onExtract.begin(), onExtract.end(), [](auto&& f) { f(); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();

        // Per view
        const Vector<View*>& views = RenderEngine::Get()->GetViews();
        for (auto v : views)
        {
            // Extract per view
            const auto& onExtractPerView = m_viewStages[FeatureRendererViewStage::OnExtractView];
            tf.for_each(onExtractPerView.begin(), onExtractPerView.end(), [v](auto&& f) { f(v); });
            JobSystem::Get()->Run(tf).wait();
            tf.clear();

            // Extract per view end.
            const auto& onExtractPerViewFin = m_viewStages[FeatureRendererViewStage::OnExtractViewFinalize];
            tf.for_each(onExtractPerViewFin.begin(), onExtractPerViewFin.end(), [v](auto&& f) { f(v); });
            JobSystem::Get()->Run(tf).wait();
            tf.clear();
        }

        // Extract end.
        const auto& onExtractEnd = m_defaultStages[FeatureRendererDefaultStage::OnExtractEnd];
        tf.for_each(onExtractEnd.begin(), onExtractEnd.end(), [](auto&& f) { f(); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();
    }

    void FeatureRendererManager::PrepareRenderData()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        Taskflow tf;

        // Prepare
        const auto& onPrepare = m_defaultStages[FeatureRendererDefaultStage::OnPrepare];
        tf.for_each(onPrepare.begin(), onPrepare.end(), [](auto&& f) { f(); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();

        // Per view
        const Vector<View*>& views = RenderEngine::Get()->GetViews();
        for (auto v : views)
        {
            // Prepare per view
            const auto& onPreparePerView = m_viewStages[FeatureRendererViewStage::OnPrepareView];
            tf.for_each(onPreparePerView.begin(), onPreparePerView.end(), [v](auto&& f) { f(v); });
            JobSystem::Get()->Run(tf).wait();
            tf.clear();

            // Prepare per view end.
            const auto& onPreparePerViewFin = m_viewStages[FeatureRendererViewStage::OnPrepareViewFinalize];
            tf.for_each(onPreparePerViewFin.begin(), onPreparePerViewFin.end(), [v](auto&& f) { f(v); });
            JobSystem::Get()->Run(tf).wait();
            tf.clear();
        }

        // Prepare end.
        const auto& onPrepareEnd = m_defaultStages[FeatureRendererDefaultStage::OnPrepareEnd];
        tf.for_each(onPrepareEnd.begin(), onPrepareEnd.end(), [](auto&& f) { f(); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();
    }

    void FeatureRendererManager::Submit(CommandBuffer& cb)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        Taskflow tf;

        // Submit.
        const auto& onSubmit = m_submitStages[FeatureRendererSubmitStage::OnSubmit];
        tf.for_each(onSubmit.begin(), onSubmit.end(), [&cb](auto&& f) { f(cb); });
        JobSystem::Get()->Run(tf).wait();
        tf.clear();
    }

} // namespace Lina::Graphics
