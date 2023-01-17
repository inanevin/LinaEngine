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

#ifndef LinaEditorBackend_HPP
#define LinaEditorBackend_HPP

#include "Graphics/PipelineObjects/Buffer.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Data/DataCommon.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina
{
    namespace Editor
    {
        class EditorGUIManager;
        class EditorRenderer;
    } // namespace Editor

    namespace Event
    {
        struct EPreMainLoop;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class CommandBuffer;
    class Material;
    class Texture;
    class Swapchain;
    class Renderer;
    class RenderEngine;

    class GUIBackend : public LinaVG::Backend::BaseBackend
    {
    private:
        enum class LinaVGDrawCategoryType
        {
            Default,
            Gradient,
            Textured,
            SimpleText,
            SDF
        };

        struct OrderedDrawRequestMeta
        {
            uint32 clipX = 0;
            uint32 clipY = 0;
            uint32 clipW = 0;
            uint32 clipH = 0;
        };

        struct OrderedDrawRequest
        {
            uint32                 vertexOffset = 0;
            uint32                 firstIndex   = 0;
            uint32                 indexSize    = 0;
            uint32                 matId        = 0;
            Material               m;
            LinaVGDrawCategoryType type = LinaVGDrawCategoryType::Default;
            OrderedDrawRequestMeta meta;
        };

        struct MaterialPool
        {
            uint32            index = 0;
            Vector<Material*> materials;
        };

        struct BufferCapsule
        {
            Vector<OrderedDrawRequest> orderedDrawRequests;
            uint32                     indexCounter  = 0;
            uint32                     vertexCounter = 0;
            Buffer                     vtxBuffer;
            Buffer                     indxBuffer;
            Vector<MaterialPool>       pools;
            Swapchain*                 swapchain          = nullptr;
            CommandBuffer*             cmd                = nullptr;
            Matrix                     projection         = Matrix::Identity();
            Vector2i                   lastProjectionSize = Vector2i::Zero;
        };

    public:
        virtual bool                  Initialize() override;
        virtual void                  Terminate() override;
        virtual void                  StartFrame(int threadCount) override;
        virtual void                  DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread) override;
        virtual void                  DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread) override;
        virtual void                  DrawDefault(LinaVG::DrawBuffer* buf, int thread) override;
        virtual void                  DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread) override;
        virtual void                  DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread) override;
        virtual void                  EndFrame() override;
        virtual void                  BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) override;
        virtual void                  BindFontTexture(LinaVG::BackendHandle texture) override;
        virtual void                  SaveAPIState() override;
        virtual void                  RestoreAPIState() override;
        virtual LinaVG::BackendHandle CreateFontTexture(int width, int height) override;

        void      Prepare(uint32 frameIndex);
        void      SetSwapchain(int thread, Swapchain* swp);
        void      SetCmd(int thread, Swapchain* swapchain, CommandBuffer* cmd);
        void      UpdateProjection(int thread, const Vector2i& size);
        void      OnPreMainLoop(const Event::EPreMainLoop& ev);
        void      CreateBufferCapsule(bool setMaterials);
        void      RecordDrawCommands(int thread);
        void      UploadAllFontTextures();
        Material* AddOrderedDrawRequest(LinaVG::DrawBuffer* buf, LinaVGDrawCategoryType type, int thread);

        inline void SetRenderEngine(RenderEngine* renderEngine)
        {
            m_renderEngine = renderEngine;
        }

    private:
        RenderEngine*             m_renderEngine;
        Vector<BufferCapsule>     m_bufferCapsules;
        Material*                 m_guiStandard = nullptr;
        HashMap<uint32, Texture*> m_fontTextures;
        uint32                    m_bufferingFontTexture = 0;
        uint32                    m_frameIndex           = 0;
    };

} // namespace Lina::Graphics

#endif
