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
#include "PipelineObjects/CommandBuffer.hpp"
#include "PipelineObjects/CommandPool.hpp"
#include "PipelineObjects/RenderPass.hpp"
#include "PipelineObjects/Framebuffer.hpp"
#include "PipelineObjects/Semaphore.hpp"
#include "PipelineObjects/Fence.hpp"
#include "PipelineObjects/Pipeline.hpp"
#include "PipelineObjects/PipelineLayout.hpp"
#include "PipelineObjects/Image.hpp"
#include "PipelineObjects/Buffer.hpp"
#include "PipelineObjects/DescriptorSet.hpp"
#include "PipelineObjects/Sampler.hpp"
#include "PipelineObjects/DescriptorPool.hpp"
#include "Data/IDList.hpp"
#include "CameraSystem.hpp"
#include "View.hpp"
#include "DrawPass.hpp"

#include <functional>

namespace Lina
{
    namespace World
    {
        class EntityWorld;
    }

    namespace Event
    {
        struct ELevelUninstalled;
        struct ELevelInstalled;
        struct EComponentCreated;
        struct EComponentDestroyed;
        struct EPreMainLoop;
        struct EWindowResized;
        struct EWindowPositioned;
    } // namespace Event

} // namespace Lina
namespace Lina::Graphics
{
    class Backend;
    class ModelNodeComponent;

    class Renderer
    {

    private:
        struct Frame
        {
            Fence         renderFence;
            Semaphore     renderSemaphore;
            CommandPool   pool;
            CommandBuffer commandBuffer;
            CommandBuffer finalPassCmd;
            Semaphore     presentSemaphore;
            Buffer        objDataBuffer;
            Buffer        indirectBuffer;
            Buffer        sceneDataBuffer;
            Buffer        viewDataBuffer;
            Buffer        lightDataBuffer;

            DescriptorSet passDescriptor;
            DescriptorSet globalDescriptor;
        };

    public:
        Renderer()  = default;
        ~Renderer() = default;

        inline CameraSystem& GetCameraSystem()
        {
            return m_cameraSystem;
        }

        inline const Vector<View*>& GetViews()
        {
            return m_views;
        }

        inline DescriptorSet& GetGlobalDescriptorSet()
        {
            return GetCurrentFrame().globalDescriptor;
        }

        inline DescriptorSet& GetPassDescriptorSet()
        {
            return GetCurrentFrame().passDescriptor;
        }
        inline uint32 GetFrameIndex()
        {
            return m_frameNumber % FRAMES_IN_FLIGHT;
        }

        inline Buffer& GetObjectDataBuffer()
        {
            return GetCurrentFrame().objDataBuffer;
        }

        inline Buffer& GetSceneDataBuffer()
        {
            return GetCurrentFrame().sceneDataBuffer;
        }

        inline Buffer& GetViewDataBuffer()
        {
            return GetCurrentFrame().viewDataBuffer;
        }

        inline Buffer& GetLightDataBuffer()
        {
            return GetCurrentFrame().lightDataBuffer;
        }

        inline Buffer& GetIndirectBuffer()
        {
            return GetCurrentFrame().indirectBuffer;
        }

        inline GPUViewData& GetViewData()
        {
            return m_viewData;
        }

        Frame& GetCurrentFrame()
        {
            return m_frames[m_frameNumber % FRAMES_IN_FLIGHT];
        }

        inline RenderPass& GetRenderPass(RenderPassType type)
        {
            return m_renderPasses[type];
        }

    private:
        friend class RenderEngine;

        void Initialize();
        void Shutdown();
        void Tick();
        void Render();
        void Join();
        void SyncData();
        void Stop();
        void OnLevelUninstalled(const Event::ELevelUninstalled& ev);
        void OnComponentCreated(const Event::EComponentCreated& ev);
        void OnComponentDestroyed(const Event::EComponentDestroyed& ev);
        void OnPreMainLoop(const Event::EPreMainLoop& ev);
        void OnWindowResized(const Event::EWindowResized& ev);
        void OnWindowPositioned(const Event::EWindowPositioned& newPos);
        void HandleOutOfDateImage();

    private:
        Vector<View*>                       m_views;
        View                                m_playerView;
        CameraSystem                        m_cameraSystem;
        Vector<Framebuffer>                 m_framebuffers;
        Vector<RenderableData>              m_extractedRenderables;
        Image                               m_depthImage;
        HashMap<RenderPassType, RenderPass> m_renderPasses;

        IDList<RenderableComponent*> m_allRenderables;
        Vector<GPUObjectData>        m_gpuObjectData;
        DrawPass                     m_opaquePass;
        GPUSceneData                 m_sceneData;
        GPUViewData                  m_viewData;
        GPULightData                 m_lightData;

        uint32   m_frameNumber = 0;
        Frame    m_frames[FRAMES_IN_FLIGHT];
        Backend* m_backend           = nullptr;
        bool     m_recreateSwapchain = false;

        Atomic<bool> m_stopped = false;
    };

} // namespace Lina::Graphics

#endif
