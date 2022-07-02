/*
Class: RenderTarget

Represents a frame buffer object created on the GPU backend.

Timestamp: 4/26/2019 9:13:05 PM
*/

#pragma once

#ifndef RenderTarget_HPP
#define RenderTarget_HPP

#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
    class Texture;

    class RenderTarget
    {
    public:
        RenderTarget()
        {
        }
        ~RenderTarget();

        void Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, bool noReadWrite = false, uint32 attachmentNumber = 0, uint32 mipLevel = 0);
        void Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, FrameBufferAttachment rboAttachment, uint32 rbo, uint32 attachmentNumber = 0, uint32 mipLevel = 0);
        void Construct(FrameBufferAttachment rboAttachment, uint32 rbo);
        void CheckCompressed(const Texture& texture);

        uint32 GetID()
        {
            return m_engineBoundID;
        }

    private:
        bool   m_constructed   = false;
        uint32 m_engineBoundID = 0;
        uint32 m_rbo           = 0;
    };
} // namespace Lina::Graphics

#endif
