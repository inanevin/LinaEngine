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

#ifndef DefaultRenderer_HPP
#define DefaultRenderer_HPP

#include "Renderer.hpp"
#include "DrawPass.hpp"
#include "Data/IDList.hpp"
#include "PipelineObjects/RenderPass.hpp"

namespace Lina
{
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
    class View;

    class DefaultRenderer : public Renderer
    {

    public:
        DefaultRenderer()          = default;
        virtual ~DefaultRenderer() = default;

        virtual void Initialize() override;
        virtual void Shutdown() override;
        virtual void RecordCommandBuffer(Frame& frame);
        virtual void SyncData();
        void         OnLevelUninstalled(const Event::ELevelUninstalled& ev);
        void         OnLevelInstalled(const Event::ELevelInstalled& ev);
        void         OnComponentCreated(const Event::EComponentCreated& ev);
        void         OnComponentDestroyed(const Event::EComponentDestroyed& ev);
        void         OnPreMainLoop(const Event::EPreMainLoop& ev);
        void         OnWindowResized(const Event::EWindowResized& ev);
        void         OnWindowPositioned(const Event::EWindowPositioned& newPos);
        void         HandleOutOfDateImage();
        void         MergeMeshes();

        inline GPUViewData& GetViewData()
        {
            return m_viewData;
        }

        inline void SetSwapchain(Swapchain* swp)
        {
            m_swapchain = swp;
        }

        inline void SetCommandPool(CommandPool* pool)
        {
            m_commandPool = pool;
        }

        inline uint32 GetCurrentImageIndex()
        {
            return m_currentImageIndex;
        }

        inline CommandBuffer* GetCurrentCommandBuffer()
        {
            return &m_cmds[m_currentImageIndex];
        }

        inline Swapchain* GetSwapchain()
        {
            return m_swapchain;
        }

    private:
        Vector<RenderableData> m_extractedRenderables;

        IDList<RenderableComponent*>          m_allRenderables;
        Vector<GPUObjectData>                 m_gpuObjectData;
        DrawPass                              m_opaquePass;
        GPUSceneData                          m_sceneData;
        GPUViewData                           m_viewData;
        GPULightData                          m_lightData;
        HashMap<Mesh*, MergedBufferMeshEntry> m_meshEntries;
        
        Buffer       m_cpuVtxBuffer;
        Buffer       m_cpuIndexBuffer;
        Buffer       m_gpuVtxBuffer;
        Buffer       m_gpuIndexBuffer;
        bool         m_recreateSwapchain = false;
        Atomic<bool> m_stopped           = false;
    };

} // namespace Lina::Graphics

#endif
