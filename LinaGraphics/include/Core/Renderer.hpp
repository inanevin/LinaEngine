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
#include "PipelineObjects/DescriptorPool.hpp"
#include "FeatureRenderers/FeatureRendererManager.hpp"
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
            Semaphore     presentSemaphore;
            Buffer        objDataBuffer;
            Buffer        indirectBuffer;
            Buffer        scenePropertiesBuffer;

            DescriptorSet passDescriptor;
            DescriptorSet globalDescriptor;
        };

        struct Pass
        {
            RenderPass  renderPass;
            SubPass     subPass;
            Framebuffer frameBuffer;
            Image       m_colImg;
            Image       m_depthImg;
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

        inline DescriptorSet& GetMaterialDescriptorSet()
        {
            return m_materialDescriptor;
        }
        inline Buffer& GetMaterialBuffer()
        {
            return m_materialBuffer;
        }

        inline uint32 GetFrameIndex()
        {
            return m_frameNumber % FRAMES_IN_FLIGHT;
        }

        inline Buffer& GetObjectDataBuffer()
        {
            return GetCurrentFrame().objDataBuffer;
        }

        inline Buffer& GetScenePropertiesBuffer()
        {
            return GetCurrentFrame().scenePropertiesBuffer;
        }

        inline Buffer& GetIndirectBuffer()
        {
            return GetCurrentFrame().indirectBuffer;
        }

        inline GPUSceneData& GetSceneData()
        {
            return m_sceneData;
        }

        Frame& GetCurrentFrame()
        {
            return m_frames[m_frameNumber % FRAMES_IN_FLIGHT];
        }

        inline RenderPass& GetRenderPass(RenderPassType type)
        {
            return m_passes[type].renderPass;
        }

        DescriptorSet& GetDescriptorSet(DescriptorSetType type);

    private:
        friend class RenderEngine;

        void Initialize();
        void Shutdown();
        void Tick();
        void Render();
        void Join();
        void SyncData();
        void OnLevelInstalled(const Event::ELevelInstalled& ev);
        void OnLevelUninstalled(const Event::ELevelUninstalled& ev);
        void OnComponentCreated(const Event::EComponentCreated& ev);
        void OnComponentDestroyed(const Event::EComponentDestroyed& ev);

    private:
        FeatureRendererManager        m_featureRendererManager;
        Vector<View*>                 m_views;
        View                          m_playerView;
        CameraSystem                  m_cameraSystem;
        Vector<Framebuffer>           m_framebuffers;
        Vector<RenderableData>        m_extractedRenderables;
        Image                         m_depthImage;
        HashMap<RenderPassType, Pass> m_passes;

        DescriptorSet m_materialDescriptor;
        Buffer        m_materialBuffer;

        IDList<RenderableComponent*> m_allRenderables;
        Vector<GPUObjectData>        m_gpuObjectData;
        DrawPass                     m_opaquePass;
        GPUSceneData                 m_sceneData;

        uint32   m_frameNumber = 0;
        Frame    m_frames[FRAMES_IN_FLIGHT];
        Backend* m_backend           = nullptr;
        bool     m_hasLevelInstalled = false;
    };

} // namespace Lina::Graphics

#endif
