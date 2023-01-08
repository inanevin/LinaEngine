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

#ifndef Renderer_HPP
#define Renderer_HPP

#include "GraphicsCommon.hpp"
#include "RenderData.hpp"
#include "Math/Rect.hpp"
#include "CameraSystem.hpp"
#include "Graphics/PipelineObjects/RenderPass.hpp"
#include "Graphics/PipelineObjects/CommandPool.hpp"
#include "Graphics/PipelineObjects/CommandBuffer.hpp"
#include "Graphics/PipelineObjects/DescriptorPool.hpp"
#include "Data/HashMap.hpp"
#include "View.hpp"
#include "DrawPass.hpp"
#include "Data/IDList.hpp"

namespace Lina
{
    namespace Editor
    {
        class Editor;
    }

    namespace World
    {
        class EntityWorld;
    }

    namespace Event
    {
        struct EWindowResized;
        struct EWindowPositioned;
        struct ELevelUninstalled;
        struct ELevelInstalled;
        struct EResourceLoaded;
        struct EComponentCreated;
        struct EComponentDestroyed;
    } // namespace Event

} // namespace Lina

namespace Lina::Graphics
{
    class GUIBackend;
    class Swapchain;
    class WindowManager;

    class Renderer
    {
    public:
        struct RenderWorldData
        {
            Texture*                     finalColorTexture = nullptr;
            Texture*                     finalDepthTexture = nullptr;
            IDList<RenderableComponent*> allRenderables;
            Vector<RenderableData>       extractedRenderables;
            DrawPass                     opaquePass;
            View                         playerView;
            CameraComponent*             cameraComponent = nullptr;
            GPUSceneData                 sceneData;
            GPULightData                 lightData;
            Buffer                       objDataBuffer[FRAMES_IN_FLIGHT];
            Buffer                       indirectBuffer[FRAMES_IN_FLIGHT];
            Buffer                       sceneDataBuffer[FRAMES_IN_FLIGHT];
            Buffer                       viewDataBuffer[FRAMES_IN_FLIGHT];
            Buffer                       lightDataBuffer[FRAMES_IN_FLIGHT];
            DescriptorSet                passDescriptor;
            bool                         initialized = false;
            // DescriptorSet             globalDescriptor;
        };

        Renderer()          = default;
        virtual ~Renderer() = default;

        inline const Viewport& GetViewport() const
        {
            return m_viewport;
        }

        inline const Recti& GetScissors() const
        {
            return m_scissors;
        }

        inline CameraSystem& GetCameraSystem()
        {
            return m_cameraSystem;
        }

        inline uint32 GetFrameIndex()
        {
            return m_frameNumber % FRAMES_IN_FLIGHT;
        }

        void UpdateViewport(const Vector2i& size);

        void AddWorldToRender(World::EntityWorld* world);
        void RemoveWorldToRender(World::EntityWorld* world);

    protected:
        friend class RenderEngine;
        friend class Editor::Editor;

        virtual void Initialize(Swapchain* swp, GUIBackend* guiBackend, WindowManager* windowManager);
        virtual void Shutdown();
        virtual void MergeMeshes();
        virtual bool HandleOutOfDateImage(VulkanResult res, bool checkSemaphoreSignal);
        virtual void ConnectEvents();
        virtual void DisconnectEvents();
        virtual void Join();
        virtual void RenderWorld(const CommandBuffer& cmd, RenderWorldData& data);
        virtual void SyncData();

        virtual void Tick()                = 0;
        virtual void Render()              = 0;
        virtual void OnTexturesRecreated() = 0;

        virtual void OnLevelUninstalled(const Event::ELevelUninstalled& ev);
        virtual void OnLevelInstalled(const Event::ELevelInstalled& ev);
        virtual void OnResourceLoaded(const Event::EResourceLoaded& res);
        virtual void OnComponentCreated(const Event::EComponentCreated& ev);
        virtual void OnComponentDestroyed(const Event::EComponentDestroyed& ev);

    protected:
        Viewport                                      m_viewport;
        Recti                                         m_scissors;
        CameraSystem                                  m_cameraSystem;
        Swapchain*                                    m_swapchain  = nullptr;
        GUIBackend*                                   m_guiBackend = nullptr;
        CommandPool                                   m_cmdPool;
        uint32                                        m_frameNumber = 0;
        Frame                                         m_frames[FRAMES_IN_FLIGHT];
        DescriptorPool                                m_descriptorPool;
        Vector<CommandBuffer>                         m_cmds;
        HashMap<Mesh*, MergedBufferMeshEntry>         m_meshEntries;
        Vector<StringID>                              m_mergedModelIDs;
        Buffer                                        m_cpuVtxBuffer;
        Buffer                                        m_cpuIndexBuffer;
        Buffer                                        m_gpuVtxBuffer;
        Buffer                                        m_gpuIndexBuffer;
        bool                                          m_hasLevelLoaded = false;
        HashMap<World::EntityWorld*, RenderWorldData> m_worldsToRender;
        HashMap<World::EntityWorld*, RenderWorldData> m_worldsToRenderGPU;
        World::EntityWorld*                           m_installedWorld = nullptr;
        uint32                                        m_worldCounter   = 0;
        WindowManager*                                m_windowManager;
        Vector2i                                      m_lastMainSwapchainSize = Vector2i::Zero;
    };

} // namespace Lina::Graphics

#endif
