/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Rendering/RenderTarget.hpp"

#include "Core/RenderEngineBackend.hpp"
#include "Log/Log.hpp"

namespace Lina::Graphics
{
    // Destructor releases the render target through render engine.
    RenderTarget::~RenderTarget()
    {
        if (m_constructed)
            m_engineBoundID = RenderEngineBackend::Get()->GetRenderDevice()->ReleaseRenderTarget(m_engineBoundID);
    }

    // TODO: Take care of texture compression. Should not be compressed.
    // Constructors create the target through render engine.
    void RenderTarget::Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, bool noReadWrite, uint32 attachmentNumber, uint32 mipLevel)
    {
        m_constructed   = true;
        m_engineBoundID = RenderEngineBackend::Get()->GetRenderDevice()->CreateRenderTarget(texture.GetID(), bindTextureMode, attachment, attachmentNumber, mipLevel, noReadWrite, false, FrameBufferAttachment::ATTACHMENT_COLOR, 0, true);
        CheckCompressed(texture);
    }

    void RenderTarget::Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, FrameBufferAttachment rboAttachment, uint32 rbo, uint32 attachmentNumber, uint32 mipLevel)
    {
        m_constructed   = true;
        m_rbo           = rbo;
        m_engineBoundID = RenderEngineBackend::Get()->GetRenderDevice()->CreateRenderTarget(texture.GetID(), bindTextureMode, attachment, attachmentNumber, mipLevel, false, rbo != 0, rboAttachment, rbo, true);
        CheckCompressed(texture);
    }

    void RenderTarget::Construct(FrameBufferAttachment rboAttachment, uint32 rbo)
    {
        m_constructed   = true;
        m_rbo           = rbo;
        m_engineBoundID = RenderEngineBackend::Get()->GetRenderDevice()->CreateRenderTarget(0, TextureBindMode::BINDTEXTURE_NONE, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, false, true, rboAttachment, rbo, false);
    }

    // Check if the texture is compressed. It should not be so to properly create the target.
    void RenderTarget::CheckCompressed(const Texture& texture)
    {

        if (texture.IsCompressed())
        {
            LINA_TRACE("Compressed textures cannot be used as render targets!");
            throw std::invalid_argument("Compressed textures cannot be used as render targets!");
        }

        if (texture.HasMipmaps())
        {
            LINA_TRACE("Rendering to a texture with mipmaps will NOT render to all mipmap levels! Unexpected results may occur.");
        }
    }
} // namespace Lina::Graphics
