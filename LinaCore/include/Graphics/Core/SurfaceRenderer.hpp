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

#include "Event/IEventListener.hpp"
#include "Math/Vector.hpp"
#include "Data/Bitmask.hpp"
#include "Core/StringID.hpp"

namespace Lina
{
	class GfxManager;
	class ISwapchain;
	class Material;
	class WorldRenderer;
	class Texture;
	class Renderer;

	class SurfaceRenderer : public IEventListener
	{

	public:
		SurfaceRenderer(GfxManager* man, uint32 imageCount, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask);
		virtual ~SurfaceRenderer();

		void Tick(float delta);
		void Render();
		void Present();
		void AddWorldRenderer(WorldRenderer* renderer);
		void RemoveWorldRenderer(WorldRenderer* renderer);
		void SetOffscreenTexture(Texture* txt, uint32 imageIndex);
		void ClearOffscreenTexture();

		virtual ISwapchain* GetSwapchain() const
		{
			return m_swapchain;
		};

		inline uint32 GetWorldRendererSize()
		{
			return static_cast<uint32>(m_worldRenderers.size());
		}

		inline Vector<WorldRenderer*> GetWorldRenderers()
		{
			return m_worldRenderers;
		}

		inline uint32 GetImageCount()
		{
			return m_imageCount;
		}

		inline StringID GetSID()
		{
			return m_sid;
		}

		inline uint32 GetCurrentImageIndex()
		{
			return m_currentImageIndex;
		}

		inline void SetThreadID(int tid)
		{
			m_threadID = tid;
		}

	protected:
		static int			   s_surfaceRendererCount;
		int					   m_threadID		   = 0;
		void*				   m_windowHandle	   = nullptr;
		uint32				   m_currentImageIndex = 0;
		GfxManager*			   m_gfxManager		   = nullptr;
		ISwapchain*			   m_swapchain		   = nullptr;
		StringID			   m_sid			   = 0;
		Vector2i			   m_size			   = Vector2i::Zero;
		Bitmask16			   m_mask			   = 0;
		uint32				   m_imageCount		   = 0;
		Vector<Material*>	   m_offscreenMaterials;
		Vector<WorldRenderer*> m_worldRenderers;
		Renderer*			   m_renderer = nullptr;
	};

} // namespace Lina

#endif
