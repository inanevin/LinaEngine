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

#ifndef SurfaceRenderer_HPP
#define SurfaceRenderer_HPP

#include "Event/SystemEventListener.hpp"
#include "Core/StringID.hpp"
#include "Math/Vector.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GPUBuffer.hpp"

namespace LinaGX
{
	class CommandStream;
	class Window;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	class IGUIDrawer;
	class GUIBackend;
	class GfxManager;

	class SurfaceRenderer : public SystemEventListener
	{
	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream			 = nullptr;
			LinaGX::CommandStream* guiCopyStream		 = nullptr;
			uint16				   guiCopySemaphore		 = 0;
			uint64				   guiCopySemaphoreValue = 0;
			GPUBuffer			   guiVertexBuffer;
			GPUBuffer			   guiIndexBuffer;
			GPUBuffer			   guiMaterialBuffer;
			uint16				   guiDescriptorSet1   = 0;
			uint16				   guiDescriptorSet2   = 0;
			uint32				   guiSceneResource	   = 0;
			uint8*				   guiSceneDataMapping = nullptr;
		};

	public:
		SurfaceRenderer(GfxManager* man, LinaGX::Window* window, StringID sid, const Vector2ui& initialSize);
		virtual ~SurfaceRenderer();

		void		 Render(int guiThreadID, uint32 frameIndex);
		void		 Present();
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev) override;
		void		 SetGUIDrawer(IGUIDrawer* rend);
		bool		 IsVisible();
		void		 Resize(const Vector2ui& newSize);

		virtual Bitmask32 GetSystemEventMask() override
		{
			return 0;
		}

		inline IGUIDrawer* GetGUIDrawer() const
		{
			return m_guiDrawer;
		}

		inline StringID GetSID() const
		{
			return m_sid;
		}

		inline LinaGX::Window* GetWindow()
		{
			return m_window;
		}

		inline uint8 GetSwapchain() const
		{
			return m_swapchain;
		}

		inline LinaGX::CommandStream* GetStream(uint32 index)
		{
			return m_pfd[index].gfxStream;
		}

		inline const Vector<uint16>& GetCurrentWaitSemaphores() const
		{
			return m_waitSemaphores;
		}

		inline const Vector<uint64>& GetCurrentWaitValues() const
		{
			return m_waitValues;
		}

	protected:
		static int s_surfaceRendererCount;

		StringID		  m_sid		   = 0;
		GfxManager*		  m_gfxManager = nullptr;
		IGUIDrawer*		  m_guiDrawer  = nullptr;
		GUIBackend*		  m_guiBackend = nullptr;
		Vector2ui		  m_size	   = Vector2ui::Zero;
		PerFrameData	  m_pfd[FRAMES_IN_FLIGHT];
		LinaGX::Instance* m_lgx		  = nullptr;
		LinaGX::Window*	  m_window	  = nullptr;
		uint8			  m_swapchain = 0;
		Vector<uint16>	  m_waitSemaphores;
		Vector<uint64>	  m_waitValues;
		uint8			  m_gfxQueue = 0;
	};

} // namespace Lina

#endif
