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

#ifndef ResourceUploadQueue_HPP
#define ResourceUploadQueue_HPP

#include "Data/Mutex.hpp"
#include "Data/Vector.hpp"
#include "Data/Functional.hpp"

namespace LinaGX
{
	class CommandStream;
}

namespace Lina
{
	class Texture;
	class GfxManager;
	class LGXWrapper;

	struct TextureUploadRequest
	{
		Texture*		 txt = nullptr;
		Delegate<void()> onComplete;
		uint64			 sentFrame = 0;
	};

	class ResourceUploadQueue
	{
	public:
		ResourceUploadQueue(GfxManager* gfxMan);
		~ResourceUploadQueue() = default;

		void Initialize();
		void Shutdown();
		void AddTextureRequest(Texture* txt, Delegate<void()>&& onComplete);

		void FlushAll();

		inline bool HasTransfer() const
		{
			return m_hasTransferForThisFrame;
		}

		inline uint16 GetSemaphore() const
		{
			return m_copySemaphore;
		}

		inline uint64 GetSemaphoreValue() const
		{
			return m_copySemaphoreValue;
		}

	private:
		bool						 m_hasTransferForThisFrame = false;
		Mutex						 m_mtx;
		Vector<TextureUploadRequest> m_textureRequests;
		LGXWrapper*					 m_lgxWrapper		  = nullptr;
		GfxManager*					 m_gfxManager		  = nullptr;
		LinaGX::CommandStream*		 m_copyStream		  = nullptr;
		uint16						 m_copySemaphore	  = 0;
		uint64						 m_copySemaphoreValue = 0;
	};
} // namespace Lina

#endif
