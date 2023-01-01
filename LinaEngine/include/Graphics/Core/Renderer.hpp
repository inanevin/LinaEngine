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
#include "Math/Rect.hpp"
#include "CameraSystem.hpp"
#include "Graphics/PipelineObjects/RenderPass.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
    namespace Editor
    {
        class Editor;
    }
} // namespace Lina
namespace Lina::Graphics
{
    class GUIBackend;
    class Swapchain;

    class Renderer
    {
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

        void UpdateViewport(const Vector2i& size);

    protected:
        friend class RenderEngine;
        friend class Editor::Editor;

        virtual void Initialize(Swapchain* swp, GUIBackend* guiBackend)
        {
            m_swapchain  = swp;
            m_guiBackend = guiBackend;
        };

        virtual void Shutdown(){};
        virtual void Tick(){};
        virtual void Render(){};
        virtual void SyncData(){};
        virtual void Stop(){};
        virtual void Join(){};
        virtual void SetMaterialTextures(){};

    protected:
        Viewport     m_viewport;
        Recti        m_scissors;
        CameraSystem m_cameraSystem;
        Swapchain*   m_swapchain  = nullptr;
        GUIBackend*  m_guiBackend = nullptr;
    };

} // namespace Lina::Graphics

#endif
