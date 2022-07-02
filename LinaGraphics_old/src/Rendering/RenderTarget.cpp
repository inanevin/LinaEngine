#include "Rendering/RenderTarget.hpp"

#include "Core/RenderEngine.hpp"
#include "Log/Log.hpp"

namespace Lina::Graphics
{
    // Destructor releases the render target through render engine.
    RenderTarget::~RenderTarget()
    {
        if (m_constructed)
            m_engineBoundID = RenderEngine::Get()->GetRenderDevice()->ReleaseRenderTarget(m_engineBoundID);
    }

    // TODO: Take care of texture compression. Should not be compressed.
    // Constructors create the target through render engine.
    void RenderTarget::Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, bool noReadWrite, uint32 attachmentNumber, uint32 mipLevel)
    {
        m_constructed   = true;
        m_engineBoundID = RenderEngine::Get()->GetRenderDevice()->CreateRenderTarget(texture.GetID(), bindTextureMode, attachment, attachmentNumber, mipLevel, noReadWrite, false, FrameBufferAttachment::ATTACHMENT_COLOR, 0, true);
        CheckCompressed(texture);
    }

    void RenderTarget::Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, FrameBufferAttachment rboAttachment, uint32 rbo, uint32 attachmentNumber, uint32 mipLevel)
    {
        m_constructed   = true;
        m_rbo           = rbo;
        m_engineBoundID = RenderEngine::Get()->GetRenderDevice()->CreateRenderTarget(texture.GetID(), bindTextureMode, attachment, attachmentNumber, mipLevel, false, rbo != 0, rboAttachment, rbo, true);
        CheckCompressed(texture);
    }

    void RenderTarget::Construct(FrameBufferAttachment rboAttachment, uint32 rbo)
    {
        m_constructed   = true;
        m_rbo           = rbo;
        m_engineBoundID = RenderEngine::Get()->GetRenderDevice()->CreateRenderTarget(0, TextureBindMode::BINDTEXTURE_NONE, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, false, true, rboAttachment, rbo, false);
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
