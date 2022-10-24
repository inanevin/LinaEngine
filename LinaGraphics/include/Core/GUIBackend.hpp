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

#include "PipelineObjects/Buffer.hpp"
#include "Data/Vector.hpp"
#include <LinaVG/Backends/BaseBackend.hpp>

namespace Lina
{
    namespace Editor
    {
        class EditorRenderer;
    }

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
            Material*              transientMat = nullptr;
            LinaVGDrawCategoryType type         = LinaVGDrawCategoryType::Default;
            OrderedDrawRequestMeta meta;
        };

        struct BufferCapsule
        {
            Vector<OrderedDrawRequest> orderedDrawRequests;
            uint32                     indexCounter  = 0;
            uint32                     vertexCounter = 0;
            Buffer                     testVtxBuffer;
            Buffer                     testIndexBuffer;
            void*                      indexPtr = nullptr;
            void*                      vtxPtr   = nullptr;
        };

        struct MaterialPool
        {
            uint32           index = 0;
            Vector<Material> materials;
        };

    public:
        virtual bool                  Initialize() override;
        virtual void                  Terminate() override;
        virtual void                  StartFrame() override;
        virtual void                  DrawGradient(LinaVG::GradientDrawBuffer* buf) override;
        virtual void                  DrawTextured(LinaVG::TextureDrawBuffer* buf) override;
        virtual void                  DrawDefault(LinaVG::DrawBuffer* buf) override;
        virtual void                  DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf) override;
        virtual void                  DrawSDFText(LinaVG::SDFTextDrawBuffer* buf) override;
        virtual void                  EndFrame() override;
        virtual void                  BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) override;
        virtual void                  BindFontTexture(LinaVG::BackendHandle texture) override;
        virtual void                  SaveAPIState() override;
        virtual void                  RestoreAPIState() override;
        virtual LinaVG::BackendHandle CreateFontTexture(int width, int height) override;

        static inline GUIBackend* Get()
        {
            return s_instance;
        }

    private:
        friend class Editor::EditorRenderer;
        friend class Renderer;
        friend class RenderEngine;

        inline void SetCmd(CommandBuffer* cmd)
        {
            m_cmd = cmd;
        }

        void      OnPreMainLoop(const Event::EPreMainLoop& ev);
        void      CreateBufferCapsule();
        void      UpdateProjection();
        void      RecordDrawCommands();
        void      UploadFontTexture(uint32 handle);
        Material* AddOrderedDrawRequest(LinaVG::DrawBuffer* buf, LinaVGDrawCategoryType type);

    private:
        Vector<BufferCapsule>     m_bufferCapsules;
        Matrix                    m_projection = Matrix::Identity();
        CommandBuffer*            m_cmd        = nullptr;
        static GUIBackend*        s_instance;
        Material*                 m_guiStandard = nullptr;
        HashMap<uint32, Texture*> m_fontTextures;
        uint32                    m_bufferingFontTexture = 0;

        // Pool per frame-in-flight
        HashMap<uint32, MaterialPool> m_materialPools;
    };

} // namespace Lina::Graphics

#endif
