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

#include "Renderer.hpp"
#include "Event/IEventListener.hpp"

namespace Lina
{
	class Texture;
	class Swapchain;
	class WorldRenderer;

	class SurfaceRenderer : public Renderer, public IEventListener
	{

	public:
		SurfaceRenderer(GfxManager* man, Swapchain* swapchain, Bitmask16 mask);
		virtual ~SurfaceRenderer();

		void AddWorldRenderer(WorldRenderer* renderer);
		void RemoveWorldRenderer(WorldRenderer* renderer);
		void SetOffscreenTexture(Texture* txt, uint32 imageIndex);
		void ClearOffscreenTexture();
		void AcquiredImageInvalid(uint32 frameIndex);
		bool AcquireImage(uint32 frameIndex);

		virtual void		   OnSystemEvent(ESystemEvent ev, const Event& data) override;
		virtual CommandBuffer* Render(uint32 frameIndex) override;
		virtual void		   Tick(float delta) override;
		virtual void		   SyncData(float alpha) override;

		virtual uint32 GetAcquiredImage() const
		{
			return m_acquiredImage;
		}

		virtual Swapchain* GetSwapchain() const
		{
			return m_swapchain;
		};

		void OnPostPresent(VulkanResult res)
		{
			CanContinueWithAcquiredImage(res, true);
		}

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_WindowResize;
		}

		inline uint32 GetWorldRendererSize()
		{
			return static_cast<uint32>(m_worldRenderers.size());
		}

		inline Vector<WorldRenderer*> GetWorldRenderers()
		{
			return m_worldRenderers;
		}

	private:
		bool CanContinueWithAcquiredImage(VulkanResult res, bool disallowSuboptimal = false);

		static int			   s_surfaceRendererCount;
		Swapchain*			   m_swapchain		   = nullptr;
		uint32				   m_acquiredImage	   = 0;
		bool				   m_recreateSwapchain = false;
		Vector2i			   m_newSwapchainSize  = Vector2i::Zero;
		Vector<Material*>	   m_offscreenMaterials;
		Vector<WorldRenderer*> m_worldRenderers;
	};

} // namespace Lina

#endif
