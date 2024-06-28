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
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/GUI/GUIRenderer.hpp"
#include "Core/Graphics/Renderers/Renderer.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"

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

	class SurfaceRenderer : public Renderer
	{
	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream	 = nullptr;
			LinaGX::CommandStream* copyStream	 = nullptr;
			SemaphoreData		   copySemaphore = {};
		};

	public:
		SurfaceRenderer(GfxManager* man, LinaGX::Window* window, const Vector2ui& initialSize, const Color& clearColor);
		virtual ~SurfaceRenderer();

		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void Render(uint32 frameIndex, uint32 waitCount, uint16* waitSemaphores, uint64* waitValues) override;
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize) override;

		virtual bool IsValidThisFrame() override
		{
			return m_isVisible;
		}

		/// If this renderer is submitted in a batch, return the recorded command stream for batch.
		virtual LinaGX::CommandStream* GetStreamForBatchSubmit(uint32 frameIndex) override
		{
			return m_pfd[frameIndex].gfxStream;
		}

		/// If this renderer's commands are submitted in a batch, the batch will wait for semaphores from all batch contributors, actuqired via GetWaitSemaphore() for each.
		virtual SemaphoreData GetWaitSemaphore(uint32 frameIndex) override
		{
			return m_pfd[frameIndex].copySemaphore;
		};

		/// If desired the renderer can return a swapchain that'll be presented in batch at the end of the render loop.
		virtual bool GetSwapchainToPresent(uint8& outSwapchain) override
		{
			outSwapchain = m_swapchain;
			return false;
		}

		inline uint8 GetSwapchain() const
		{
			return m_swapchain;
		}

		inline Widget* GetGUIRoot()
		{
			return m_guiRenderer.GetGUIRoot();
		}

		inline WidgetManager& GetWidgetManager()
		{
			return m_guiRenderer.GetWidgetManager();
		}

		LinaGX::Window* GetWindow() const
		{
			return m_window;
		}

	private:
		void UpdateBuffers(uint32 frameIndex);

	protected:
		ResourceUploadQueue	 m_uploadQueue;
		ResourceManager*	 m_rm				= nullptr;
		Shader*				 m_guiShader2D		= nullptr;
		uint32				 m_guiShaderVariant = 0;
		Vector2ui			 m_size				= Vector2ui::Zero;
		PerFrameData		 m_pfd[FRAMES_IN_FLIGHT];
		LinaGX::Window*		 m_window	   = nullptr;
		uint8				 m_swapchain   = 0;
		bool				 m_isVisible   = false;
		ApplicationDelegate* m_appListener = nullptr;
		RenderPass			 m_guiPass	   = {};
		GUIRenderer			 m_guiRenderer;
	};

} // namespace Lina
