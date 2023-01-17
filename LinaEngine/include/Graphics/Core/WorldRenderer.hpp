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

#ifndef WorldRenderer_HPP
#define WorldRenderer_HPP

#include "Renderer.hpp"
#include "Data/IDList.hpp"
#include "DrawPass.hpp"
#include "View.hpp"
#include "CameraSystem.hpp"

namespace Lina
{
    namespace World
    {
        class EntityWorld;
    }

    namespace Event
    {
        struct EComponentCreated;
        struct EComponentDestroyed;
    } // namespace Event

} // namespace Lina

namespace Lina::Graphics
{
    class SurfaceRenderer;

    class WorldRenderer : public Renderer
    {
    public:
        struct RenderWorldData
        {
            World::EntityWorld*          world              = nullptr;
            Texture*                     finalColorTexture  = nullptr;
            Texture*                     finalDepthTexture  = nullptr;
            Texture*                     postProcessTexture = nullptr;
            IDList<RenderableComponent*> allRenderables;
            Vector<RenderableData>       extractedRenderables;
            DrawPass                     opaquePass;
            View                         playerView;
            GPUSceneData                 sceneData;
            GPULightData                 lightData;
            Buffer                       objDataBuffer[FRAMES_IN_FLIGHT];
            Buffer                       indirectBuffer[FRAMES_IN_FLIGHT];
            Buffer                       sceneDataBuffer[FRAMES_IN_FLIGHT];
            Buffer                       viewDataBuffer[FRAMES_IN_FLIGHT];
            Buffer                       lightDataBuffer[FRAMES_IN_FLIGHT];
            DescriptorSet                passDescriptor;
            // DescriptorSet             globalDescriptor;
        };

    public:
        WorldRenderer()          = default;
        virtual ~WorldRenderer() = default;

        inline CameraSystem& GetCameraSystem()
        {
            return m_cameraSystem;
        }

        void     AddFinalTextureListener(SurfaceRenderer* listener);
        void     RemoveFinalTextureListener(SurfaceRenderer* listener);
        Texture* GetFinalTexture();
        void     SetAspectRatio(float aspect);
        void     SetRenderResolution(const Vector2i& res);
        void     SetWorld(World::EntityWorld* world);
        void     RemoveWorld();

    protected:
        friend class RenderEngine;

        virtual bool           Initialize(RenderEngine* renderEngine) override;
        virtual void           Shutdown() override;
        virtual void           SyncData() override;
        virtual void           Tick() override;
        virtual CommandBuffer* Render(uint32 frameIndex, Fence& fence) override;

    private:
        void RenderWorld(uint32 frameIndex, const CommandBuffer& cmd, RenderWorldData& data);
        void OnComponentCreated(const Event::EComponentCreated& ev);
        void OnComponentDestroyed(const Event::EComponentDestroyed& ev);
        void CreateTextures(const Vector2i& res, bool createMaterials);
        void DestroyTextures();

    protected:
        static Atomic<uint32> s_worldCounter;

        RenderWorldData          m_targetWorldData;
        CameraSystem             m_cameraSystem;
        Material*                m_worldPostProcessMaterials[FRAMES_IN_FLIGHT];
        float                    m_aspectRatio       = 1.0f;
        Vector2i                 m_renderResolution  = Vector2i(1920, 1080);
        SurfaceRenderer*         m_presentDirectlyTo = nullptr;
        Vector<SurfaceRenderer*> m_finalTextureListeners;
    };

} // namespace Lina::Graphics

#endif
