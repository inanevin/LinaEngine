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

#include "RenderData.hpp"
#include "Math/Rect.hpp"
#include "CameraSystem.hpp"
#include "Graphics/PipelineObjects/RenderPass.hpp"
#include "Graphics/PipelineObjects/CommandPool.hpp"
#include "Graphics/PipelineObjects/CommandBuffer.hpp"
#include "Graphics/PipelineObjects/DescriptorPool.hpp"
#include "Core/Command.hpp"
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
        struct EComponentCreated;
        struct EComponentDestroyed;
        struct EWindowResized;
    } // namespace Event

} // namespace Lina

namespace Lina::Graphics
{
    class GUIBackend;
    class Swapchain;
    class WindowManager;
    class RenderEngine;

    class Renderer
    {

    public:
        struct RenderWorldData
        {
            World::EntityWorld*          world             = nullptr;
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

    public:
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

        inline const Bitmask16& GetMask() const
        {
            return m_renderMask;
        }

        inline uint32 GetAcquiredImage() const
        {
            return m_acquiredImage;
        }

        inline void SetRenderMask(const Bitmask16& mask)
        {
            m_renderMask = mask;
        }

        void AssignSwapchain(Swapchain* swp);
        void SetWorld(World::EntityWorld* world);
        void RemoveWorld();

    protected:
        friend class RenderEngine;
        friend class Editor::Editor;

        virtual bool           Initialize(GUIBackend* guiBackend, WindowManager* windowManager, RenderEngine* eng);
        virtual void           Shutdown();
        virtual bool           CanContinueWithAcquiredImage(VulkanResult res);
        virtual void           RenderWorld(uint32 frameIndex, const CommandBuffer& cmd, RenderWorldData* data);
        virtual void           SyncData();
        virtual void           Tick();
        virtual bool           AcquireImage(uint32 frameIndex);
        virtual CommandBuffer* Render(uint32 frameIndex, Fence& fence);
        void                   UpdateViewport(const Vector2i& size);

    private:
        void OnComponentCreated(const Event::EComponentCreated& ev);
        void OnComponentDestroyed(const Event::EComponentDestroyed& ev);
        void OnWindowResized(const Event::EWindowResized& ev);
        void SetWorldImpl();
        void RemoveWorldImpl();

    protected:
        static Atomic<uint32> s_worldCounter;

        Vector<SimpleAction>  m_syncedActions;
        Bitmask16             m_renderMask = 0;
        Viewport              m_viewport;
        Recti                 m_scissors;
        CameraSystem          m_cameraSystem;
        Swapchain*            m_swapchain  = nullptr;
        GUIBackend*           m_guiBackend = nullptr;
        CommandPool           m_cmdPool;
        DescriptorPool        m_descriptorPool;
        Vector<CommandBuffer> m_cmds;
        RenderEngine*         m_renderEngine = nullptr;
        WindowManager*        m_windowManager;
        World::EntityWorld*   m_worldToSet      = nullptr;
        RenderWorldData*      m_targetWorldData = nullptr;
        Material*             m_worldMaterials[FRAMES_IN_FLIGHT];
        uint32                m_acquiredImage     = 0;
        bool                  m_shouldRemoveWorld = false;
        bool                  m_initialized       = false;
        bool                  m_recreateSwapchain = false;
        Vector2i              m_newSwapchainSize  = Vector2i::Zero;
    };

} // namespace Lina::Graphics

#endif
