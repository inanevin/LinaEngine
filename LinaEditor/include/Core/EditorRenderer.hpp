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

#ifndef EditorRenderer_HPP
#define EditorRenderer_HPP

#include "Graphics/Core/Renderer.hpp"
#include "Graphics/PipelineObjects/Semaphore.hpp"

namespace Lina::Editor
{
    class ImmediateGUI;

    class EditorRenderer : public Graphics::Renderer
    {

    public:
        inline const HashMap<World::EntityWorld*, Graphics::Renderer::RenderWorldData>& GetWorldData()
        {
            return m_worldsToRenderGPU;
        }

    private:
        struct AdditionalWindowData
        {
            Graphics::Swapchain*            swapchain = nullptr;
            Graphics::Semaphore             submitSemaphores[FRAMES_IN_FLIGHT];
            Graphics::Semaphore             presentSemaphore[FRAMES_IN_FLIGHT];
            Vector<Graphics::CommandBuffer> cmds;
            void*                           windowHandle = nullptr;
            Vector2i                        pos          = Vector2i::Zero;
            Vector2i                        size         = Vector2i::Zero;
        };

        struct EditorFrameData
        {
            Graphics::Semaphore*     submitSemaphore  = nullptr;
            Graphics::Semaphore*     presentSemaphore = nullptr;
            Graphics::CommandBuffer* cmd              = nullptr;
            Graphics::Swapchain*     swp              = nullptr;
            Graphics::Viewport       viewport;
            AdditionalWindowData*    wd         = nullptr;
            uint32                   imageIndex = 0;
        };

        struct AdditionalWindowRequest
        {
            String   name = "";
            Vector2i pos  = Vector2i::Zero;
            Vector2i size = Vector2i::Zero;
        };

        struct AdditionalWindowRemoveRequest
        {
            StringID sid = 0;
        };

    protected:
        friend class Editor;
        friend class ImmediateGUI;

        void CreateAdditionalWindow(const String& name, const Vector2i& pos, const Vector2i& size);
        void RemoveAdditionalWindow(StringID sid);

        virtual void Tick() override;
        virtual void Shutdown() override;
        virtual void Render() override;
        virtual void OnTexturesRecreated() override{};
        virtual void SyncData() override;

    private:
        bool                  HandleOutOfDateImageAdditional(AdditionalWindowData* wd, Graphics::VulkanResult res);
        AdditionalWindowData* GetWindowDataFromSwapchain(Graphics::Swapchain* swp);

    private:
        HashMap<StringID, AdditionalWindowData> m_additionalWindows;
        Mutex                                   m_additionalWindowMtx;
        Vector<AdditionalWindowRequest>         m_additionalWindowRequests;
        Vector<AdditionalWindowRemoveRequest>   m_additionalWindowRemoveRequests;
    };
} // namespace Lina::Editor

#endif
