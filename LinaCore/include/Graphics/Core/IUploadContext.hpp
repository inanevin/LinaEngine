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

namespace Lina
{
	class Renderer;
	class IGfxBufferResource;
	class IGfxTextureResource;
	class Texture;

	struct BufferUploadRequest
	{
		IGfxBufferResource* stagingResource = nullptr;
		IGfxBufferResource* targetResource	= nullptr;
	};

	struct TextureUploadRequest
	{
		IGfxBufferResource*	 stagingResource = nullptr;
		IGfxTextureResource* targetResource	 = nullptr;
		Texture*			 targetTexture	 = nullptr;
		ImageGenerateRequest genReq;
		uint32				 totalDataSize = 0;
	};

	enum UploadContextFlushFlags
	{
		UCF_FlushDataRequests	   = 1 << 0,
		UCF_FlushTextureRequests   = 1 << 1,
		UCF_FlushImmediateRequests = 1 << 2,
		UCF_FlushAll			   = 1 << 3,
	};

	class IUploadContext
	{
	public:
		IUploadContext(Renderer* rend) : m_renderer(rend){};
		virtual ~IUploadContext(){};

		virtual void Flush(uint32 frameIndex, Bitmask16 flushFlags)												  = 0;
		virtual void UploadBuffers(IGfxBufferResource* targetGPUResource, void* data, size_t dataSize)			  = 0;
		virtual void UploadTexture(IGfxTextureResource* targetGPUTexture, Texture* src, ImageGenerateRequest req) = 0;
		virtual void UploadBuffersImmediate(IGfxBufferResource* targetGpuResource, IGfxBufferResource* staging)	  = 0;
		virtual void PushCustomCommand(const GfxCommand& cmd)													  = 0;

	protected:
		Renderer*					 m_renderer = nullptr;
		Vector<BufferUploadRequest>	 m_bufferRequests;
		Vector<BufferUploadRequest>	 m_immediateBufferRequests;
		Vector<TextureUploadRequest> m_textureRequests;
		Mutex						 m_mtx;
	};
} // namespace Lina

#endif
