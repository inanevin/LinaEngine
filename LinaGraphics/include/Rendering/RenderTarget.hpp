/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: RenderTarget
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
		FORCEINLINE ~RenderTarget()
		{
			m_EngineBoundID = renderDevice->ReleaseRenderTarget(m_EngineBoundID);
		}

		FORCEINLINE void Construct(RenderDevice& renderDeviceIn)
		{
			renderDevice = &renderDeviceIn;
			m_EngineBoundID = 0;
		}

		// TODO: Take care of texture compression. Should not be compressed.
		// Constructors create the target through render engine.
		FORCEINLINE void Construct(RenderDevice& renderDeviceIn, Texture& texture, uint32 width, uint32 height, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber = 0, uint32 mipLevel = 0)
		{
			renderDevice = &renderDeviceIn;
			m_EngineBoundID = renderDevice->CreateRenderTarget(texture.GetID(), width, height, bindTextureMode, attachment, attachmentNumber, mipLevel);
			CheckCompressed(texture);
		}

		// Constructors create the target through render engine.
		FORCEINLINE void Construct(RenderDevice& renderDeviceIn, Texture& texture, uint32 width, uint32 height, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, FrameBufferAttachment rboAttachment, uint32 rbo, uint32 attachmentNumber = 0, uint32 mipLevel = 0)
		{
			renderDevice = &renderDeviceIn;
			m_RBO = rbo;
			m_EngineBoundID = renderDevice->CreateRenderTarget(texture.GetID(), width, height, bindTextureMode, attachment, attachmentNumber, mipLevel, true, rboAttachment, rbo);
			CheckCompressed(texture);
		}

		// Check if the texture is compressed. It should not be so to properly create the target.
		FORCEINLINE void CheckCompressed(const Texture& texture) {

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


		FORCEINLINE uint32 GetID() { return m_EngineBoundID;  }

	private:

		RenderDevice* renderDevice;
		uint32 m_EngineBoundID;
		uint32 m_RBO;

	};
}


#endif