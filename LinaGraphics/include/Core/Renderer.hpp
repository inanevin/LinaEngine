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
#include "FeatureRenderers/StaticMeshRenderer.hpp"
#include "CameraSystem.hpp"
#include "Core/View.hpp"

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
    }
} // namespace Lina
namespace Lina::Graphics
{
    class Backend;
    class RenderableComponent;
    constexpr unsigned int FRAME_OVERLAP = 2;

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
    struct GPUObjectData
    {
        Matrix modelMatrix = Matrix::Identity();
    };

    struct Frame
    {
        Fence         renderFence;
        Semaphore     renderSemaphore;
        CommandPool   pool;
        CommandBuffer commandBuffer;
        Semaphore     presentSemaphore;
        Buffer        objDataBuffer;
        DescriptorSet objDataDescriptor;
    };

    class Renderer
    {
    public:
        Renderer()  = default;
        ~Renderer() = default;

        static Renderer* Get()
        {
            return s_instance;
        }

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

        inline uint32 GetFrameIndex()
        {
            return m_frameNumber % FRAME_OVERLAP;
        }

        void AddRenderable(RenderableComponent* comp);
        void RemoveRenderable(RenderableComponent* comp);

    private:
        friend class RenderEngine;

        void   Initialize();
        void   Tick();
        void   Render();
        void   Join();
        void   Shutdown();
        void   FetchAndExtract(World::EntityWorld* world);
        void   OnLevelUninstalled(const Event::ELevelUninstalled& ev);
        Frame& GetCurrentFrame();

    private:
        static Renderer*             s_instance;
        FeatureRendererManager       m_featureRendererManager;
        StaticMeshRenderer           m_meshRenderer;
        Vector<View*>                m_views;
        View                         m_playerView;
        Vector<RenderableComponent*> m_renderables;
        Queue<uint32>                m_availableRenderableIDs;
        uint32                       m_nextRenderableID = 0;
        CameraSystem                 m_cameraSystem;
        RenderPass                   m_renderPass;
        SubPass                      m_subpass;
        Vector<Framebuffer>          m_framebuffers;
        Image                        m_depthImage;
        Buffer                       m_scenePropertiesBuffer;
        DescriptorSet                m_globalDescriptor;

        uint32   m_frameNumber = 0;
        Frame    m_frames[FRAME_OVERLAP];
        Backend* m_backend = nullptr;
    };

} // namespace Lina::Graphics

#endif
