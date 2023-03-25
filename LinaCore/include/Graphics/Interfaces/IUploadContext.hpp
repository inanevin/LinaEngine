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

#ifndef IUploadContext_HPP
#define IUploadContext_HPP

#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Data/Mutex.hpp"
#include "Data/Functional.hpp"

namespace Lina
{
	class Renderer;
	class Texture;
	class IGfxResourceTexture;
	class IGfxResourceCPU;
	class IGfxResourceGPU;

	struct TextureUploadRequest
	{
		IGfxResourceCPU*	 stagingResource = nullptr;
		IGfxResourceTexture* targetResource	 = nullptr;
		Texture*			 targetTexture	 = nullptr;
		uint32				 totalDataSize	 = 0;
		ImageGenerateRequest genReq;
	};

	struct StagingToGPURequests
	{
		IGfxResourceCPU* cpuRes = nullptr;
		IGfxResourceGPU* gpuRes = nullptr;
		Delegate<void()> onCopied;
	};

	enum UploadContextMask
	{
		UCM_FlushTextures			  = 1 << 0,
		UCM_FlushStagingToGPURequests = 1 << 1,
	};

	class IUploadContext
	{
	public:
		IUploadContext(Renderer* rend) : m_renderer(rend){};
		virtual ~IUploadContext(){};

		virtual void FlushViaMask(Bitmask16 mask)																		 = 0;
		virtual void CopyTextureImmediate(IGfxResourceTexture* targetGPUTexture, Texture* src, ImageGenerateRequest req) = 0;
		virtual void CopyTextureQueueUp(IGfxResourceTexture* targetGPUTexture, Texture* src, ImageGenerateRequest req)	 = 0;
		virtual void CopyBuffersImmediate(IGfxResourceCPU* cpuRes, IGfxResourceGPU* gpuRes)								 = 0;
		virtual void CopyBuffersQueueUp(IGfxResourceCPU* cpuRes, IGfxResourceGPU* gpuRes, Delegate<void()>&& onCopied)	 = 0;
		virtual void CopyBuffersQueueUp(IGfxResourceCPU* cpuRes, IGfxResourceGPU* gpuRes)								 = 0;
		virtual void TransferToReadyQueue()																				 = 0;

	protected:
		Renderer*					 m_renderer = nullptr;
		Vector<TextureUploadRequest> m_textureRequests;
		Vector<StagingToGPURequests> m_stagingToGPURequests;
		Vector<TextureUploadRequest> m_rdyTextureRequests;
		Vector<StagingToGPURequests> m_rdyStagingToGPURequests;
	};
} // namespace Lina

#endif
