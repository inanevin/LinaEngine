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

/*
Class: RenderTarget

Represents a frame buffer object created on the GPU backend.

Timestamp: 4/26/2019 9:13:05 PM
*/

#pragma once

#ifndef RenderTarget_HPP
#define RenderTarget_HPP

#include "Texture.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "Utility/Log.hpp"

namespace LinaEngine::Graphics
{
	class RenderTarget
	{
	public:

		RenderTarget() {}

		// Destructor releases the render target through render engine.
		~RenderTarget()
		{
			if (m_constructed)
				m_engineBoundID = m_renderDevice->ReleaseRenderTarget(m_engineBoundID);
		}


		// TODO: Take care of texture compression. Should not be compressed.
		// Constructors create the target through render engine.
		void Construct(RenderDevice& renderDeviceIn, Texture& texture, const Vector2& size, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, bool noReadWrite = false, uint32 attachmentNumber = 0, uint32 mipLevel = 0)
		{
			m_constructed = true;
			m_renderDevice = &renderDeviceIn;
			m_engineBoundID = m_renderDevice->CreateRenderTarget(texture.GetID(), (uint32)size.x, (uint32)size.y, bindTextureMode, attachment, attachmentNumber, mipLevel, noReadWrite, false, FrameBufferAttachment::ATTACHMENT_COLOR, 0, true);
			CheckCompressed(texture);
		}

		void Construct(RenderDevice& renderDeviceIn, Texture& texture, const Vector2& size, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, FrameBufferAttachment rboAttachment, uint32 rbo, uint32 attachmentNumber = 0, uint32 mipLevel = 0)
		{
			m_constructed = true;
			m_renderDevice = &renderDeviceIn;
			m_rbo = rbo;
			m_engineBoundID = m_renderDevice->CreateRenderTarget(texture.GetID(), (uint32)size.x, (uint32)size.y, bindTextureMode, attachment, attachmentNumber, mipLevel, false, rbo != 0, rboAttachment, rbo, true);
			CheckCompressed(texture);
		}

		void Construct(RenderDevice& renderDeviceIn, Vector2 size, FrameBufferAttachment rboAttachment, uint32 rbo)
		{
			m_constructed = true;
			m_renderDevice = &renderDeviceIn;
			m_rbo = rbo;
			m_engineBoundID = m_renderDevice->CreateRenderTarget(0, size.x, size.y, TextureBindMode::BINDTEXTURE_NONE, FrameBufferAttachment::ATTACHMENT_COLOR, 0,0, false, true, rboAttachment, rbo, false);
		}

		// Check if the texture is compressed. It should not be so to properly create the target.
		void CheckCompressed(const Texture& texture) {

			if (texture.IsCompressed())
			{
				LINA_CORE_TRACE("Compressed textures cannot be used as render targets!");
				throw std::invalid_argument("Compressed textures cannot be used as render targets!");
			}

			if (texture.HasMipmaps())
			{
				LINA_CORE_TRACE("Rendering to a texture with mipmaps will NOT render to all mipmap levels! Unexpected results may occur.");
			}
		}


		uint32 GetID() { return m_engineBoundID; }

	private:

		bool m_constructed = false;
		RenderDevice* m_renderDevice = nullptr;
		uint32 m_engineBoundID = 0;
		uint32 m_rbo = 0;

	};
}


#endif