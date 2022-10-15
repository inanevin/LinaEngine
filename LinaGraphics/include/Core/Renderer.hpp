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
    constexpr uint32 FRAMES_IN_FLIGHT = 1;

    struct GPUSceneData
    {
        Matrix  view;
        Matrix  proj;
        Matrix  viewProj;
        Vector4 fogColor;     // w is for exponent
        Vector4 fogDistances; // x for min, y for max, zw unused.
        Vector4 ambientColor;
        Vector4 sunlightDirection; // w for sun power
        Vector4 sunlightColor;
    };

    struct Frame
    {
        Fence         renderFence;
        Semaphore     renderSemaphore;
        CommandPool   pool;
        CommandBuffer commandBuffer;
        Semaphore     presentSemaphore;
        Buffer        objDataBuffer;
        Buffer        indirectBuffer;
        DescriptorSet objDataDescriptor;
    };

    class Renderer
    {
    public:
        Renderer()  = default;
        ~Renderer() = default;

        inline RenderPass& GetRP()
        {
            return m_renderPass;
        }

        inline CameraSystem& GetCameraSystem()
        {
            return m_cameraSystem;
        }

        inline const Vector<View*>& GetViews()
        {
            return m_views;
        }

        inline DescriptorSet& GetGlobalSet()
        {
            return m_globalDescriptor;
        }

        inline DescriptorSet& GetObjectSet()
        {
            return GetCurrentFrame().objDataDescriptor;
        }

        inline DescriptorSet& GetTextureSet()
        {
            return m_textureDescriptor;
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
            return m_scenePropertiesBuffer;
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
        FeatureRendererManager       m_featureRendererManager;
        Vector<View*>                m_views;
        View                         m_playerView;
        CameraSystem                 m_cameraSystem;
        RenderPass                   m_renderPass;
        SubPass                      m_subpass;
        Vector<Framebuffer>          m_framebuffers;
        Vector<RenderableData>       m_extractedRenderables;
        Image                        m_depthImage;
        Buffer                       m_scenePropertiesBuffer;
        DescriptorSet                m_globalDescriptor;
        DescriptorSet                m_textureDescriptor;
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
