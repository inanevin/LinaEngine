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

#ifndef SurfaceRenderer_HPP
#define SurfaceRenderer_HPP

#include "Renderer.hpp"

namespace Lina
{
    namespace Event
    {
        struct EWindowResized;
    }
} // namespace Lina
namespace Lina::Graphics
{
    class Texture;
    class SurfaceRenderer : public Renderer
    {

    public:
        SurfaceRenderer()          = default;
        virtual ~SurfaceRenderer() = default;

        virtual uint32 GetAcquiredImage() const override
        {
            return m_acquiredImage;
        }

        virtual Swapchain* GetSwapchain() const override
        {
            return m_swapchain;
        }

        inline void AssignSwapchain(Swapchain* swp)
        {
            m_swapchain = swp;
        }

        void SetOffscreenTexture(Texture* txt);

    protected:
        friend class RenderEngine;

        virtual bool           Initialize(GUIBackend* guiBackend, WindowManager* windowManager, RenderEngine* eng) override;
        virtual void           Shutdown() override;
        virtual bool           AcquireImage(uint32 frameIndex) override;
        virtual CommandBuffer* Render(uint32 frameIndex, Fence& fence) override;
        virtual void           AcquiredImageInvalid(uint32 frameIndex) override;

        virtual void OnPostPresent(VulkanResult res) override
        {
            CanContinueWithAcquiredImage(res, true);
        }

    private:
        bool CanContinueWithAcquiredImage(VulkanResult res, bool disallowSuboptimal = false);
        void OnWindowResized(const Event::EWindowResized& ev);

    protected:
        static Atomic<uint32> s_worldCounter;

        Swapchain*        m_swapchain         = nullptr;
        uint32            m_acquiredImage     = 0;
        bool              m_recreateSwapchain = false;
        Vector2i          m_newSwapchainSize  = Vector2i::Zero;
        Vector<Material*> m_offscreenMaterials;
    };

} // namespace Lina::Graphics

#endif
