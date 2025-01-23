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

#include "Core/ApplicationDelegate.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include <LinaGX/Core/WindowListener.hpp>

namespace LinaGX
{
	struct LGXVector2;
	class Instance;
	class Window;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class TextureSampler;
	class ResourceManagerV2;
	class Application;
	class Shader;
	class WorldRenderer;

	class SwapchainRenderer
	{

	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream	 = nullptr;
			LinaGX::CommandStream* copyStream	 = nullptr;
			SemaphoreData		   copySemaphore = {};
		};

	public:
		SwapchainRenderer(Application* app, LinaGX::Instance* lgx, LinaGX::Window* window, bool vsyncOn);
		virtual ~SwapchainRenderer();

		void				   OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize);
		bool				   CheckVisibility();
		void				   Tick();
		LinaGX::CommandStream* Render(uint32 frameIndex);
		void				   SyncRender();

		inline bool GetIsVisible() const
		{
			return m_isVisible;
		}

		inline void SetWorldRenderer(WorldRenderer* wr)
		{
			m_wr = wr;
		}

		inline uint8 GetSwapchain() const
		{
			return m_swapchain;
		}

	private:
		WorldRenderer*		m_wr			 = nullptr;
		uint16				m_pipelineLayout = 0;
		ResourceUploadQueue m_uploadQueue;
		PerFrameData		m_pfd[FRAMES_IN_FLIGHT];
		bool				m_isVisible		   = false;
		bool				m_vsync			   = false;
		Application*		m_app			   = nullptr;
		ResourceManagerV2*	m_rm			   = nullptr;
		Vector2ui			m_size			   = Vector2ui::Zero;
		LinaGX::Instance*	m_lgx			   = nullptr;
		LinaGX::Window*		m_window		   = nullptr;
		TextureSampler*		m_samplerSwapchain = nullptr;
		Shader*				m_shaderSwapchain  = nullptr;
		RenderPass			m_renderPass;
		uint8				m_swapchain = 0;
	};
} // namespace Lina