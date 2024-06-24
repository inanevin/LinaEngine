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

#include "Common/StringID.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Data/Bitmask.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace LinaGX
{
	class CommandStream;
	class Window;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	class GfxManager;
	class ApplicationDelegate;
	class ResourceManager;

	enum RendererFlags
	{
		RNDF_NONE			 = 1 << 0,
		RNDF_SUBMIT_IN_BATCH = 1 << 1,
	};

	class Renderer
	{
	public:
		Renderer(GfxManager* gfxMan, uint32 flags);
		virtual ~Renderer() = default;

		virtual void PreTick(){};
		virtual void Tick(float delta){};
		virtual void Render(uint32 frameIndex, uint32 waitCount, uint16* waitSemaphores, uint64* waitValues){};
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize){};

		virtual bool IsValidThisFrame()
		{
			return true;
		}

		/// If this renderer is submitted in a batch, return the recorded command stream for batch.
		virtual LinaGX::CommandStream* GetStreamForBatchSubmit(uint32 frameIndex)
		{
			return nullptr;
		}

		/// If this renderer is submitting its own commands, return the submission semaphore so that the next batch can wait on them.
		virtual SemaphoreData GetSubmitSemaphore(uint32 frameIndex)
		{
			return {};
		};

		/// If this renderer's commands are submitted in a batch, the batch will wait for semaphores from all batch contributors, actuqired via GetWaitSemaphore() for each.
		virtual SemaphoreData GetWaitSemaphore(uint32 frameIndex)
		{
			return {};
		};

		/// If desired the renderer can return a swapchain that'll be presented in batch at the end of the render loop.
		virtual bool GetSwapchainToPresent(uint8& outSwapchain)
		{
			return false;
		}

		inline const Bitmask32& GetFlags() const
		{
			return m_flags;
		}

	protected:
		GfxManager*		  m_gfxManager = nullptr;
		LinaGX::Instance* m_lgx		   = nullptr;
		Bitmask32		  m_flags	   = 0;
	};

} // namespace Lina
