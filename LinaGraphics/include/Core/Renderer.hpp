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

#ifndef RenderDataManager_HPP
#define RenderDataManager_HPP

#include "Math/Matrix.hpp"
#include "Data/Vector.hpp"
#include "Core/GraphicsCommon.hpp"
#include "PipelineObjects/RQueue.hpp"
#include "PipelineObjects/CommandBuffer.hpp"
#include "PipelineObjects/CommandPool.hpp"
#include "PipelineObjects/RenderPass.hpp"
#include "PipelineObjects/Framebuffer.hpp"
#include "PipelineObjects/Semaphore.hpp"
#include "PipelineObjects/Fence.hpp"
#include "PipelineObjects/Pipeline.hpp"
#include "PipelineObjects/PipelineLayout.hpp"
#include "PipelineObjects/Image.hpp"

#include "RenderWorld.hpp"

#include <functional>

namespace Lina::Graphics
{
    class Backend;
    class View;

    typedef std::function<void()> FeatureRendererFunction;
    typedef std::function<void()> FeatureRendererViewFunction;

    enum class FeatureRendererStage
    {
        OnExtract,
        OnExtractView,
        OnExtractViewFinalize,
        OnExtractEnd,
        OnPrepare,
        OnPrepareView,
        OnPrepareViewFinalize,
        OnPrepareEnd,
        OnSubmit
    };

    class Renderer
    {
    public:
        Renderer()  = default;
        ~Renderer() = default;

        void RegisterToStageFunction(FeatureRendererStage stage, FeatureRendererFunction&& func);
        void RegisterToStageViewFunction(FeatureRendererStage stage, FeatureRendererViewFunction&& func);

    private:
        friend class RenderEngine;

        void Initialize();
        void Render();
        void Join();
        void Shutdown();
        void FetchVisibilityState();
        void ExtractGameState(Vector<View*>& views);
        void PrepareRenderData(Vector<View*>& views);

    private:
        Vector<FeatureRendererFunction>     m_onExtract;
        Vector<FeatureRendererViewFunction> m_onExtractView;
        Vector<FeatureRendererViewFunction> m_onExtractViewFinalize;
        Vector<FeatureRendererFunction>     m_onExtractEnd;
        Vector<FeatureRendererFunction>     m_onPrepare;
        Vector<FeatureRendererViewFunction> m_onPrepareView;
        Vector<FeatureRendererViewFunction> m_onPrepareViewFinalize;
        Vector<FeatureRendererFunction>     m_onPrepareEnd;
        Vector<FeatureRendererFunction>     m_onSubmit;

        RenderWorld         m_world;
        Backend*            m_backend = nullptr;
        RQueue              m_graphicsQueue;
        CommandPool         m_pool;
        CommandBuffer       m_commandBuffer;
        RenderPass          m_renderPass;
        SubPass             m_subpass;
        Vector<Framebuffer> m_framebuffers;
        Fence               m_renderFence;
        Semaphore           m_renderSemaphore;
        Semaphore           m_presentSemaphore;
        Image               m_depthImage;
    };

} // namespace Lina::Graphics

#endif
