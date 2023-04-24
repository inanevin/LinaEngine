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

#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Event/ISystemEventListener.hpp"
#include "Math/Vector.hpp"
#include "Data/Bitmask.hpp"
#include "Core/StringID.hpp"
#include "Math/Rect.hpp"

namespace Lina
{
	class GfxManager;
	class ISwapchain;
	class Material;
	class WorldRenderer;
	class Texture;
	class Renderer;
	class IWindow;
	class IGUIDrawer;
	class GUIRenderer;
	class IUploadContext;
	class IGfxContext;
	class EntityWorld;

	class SurfaceRenderer : public ISystemEventListener
	{

	private:
		struct RenderData
		{
			Viewport viewport = Viewport();
			Recti	 scissors = Recti();
		};

		struct DataPerImage
		{
			Texture*  renderTargetColor		 = nullptr;
			Texture*  renderTargetDepth		 = nullptr;
			Material* offscreenMaterial		 = nullptr;
			Texture*  targetOffscreenTexture = nullptr;
			bool	  updateTexture			 = true;
		};

		struct DataPerFrame
		{
			uint32 cmdAllocator			= 0;
			uint32 cmdList				= 0;
			uint32 cmdAllocatorTransfer = 0;
			uint32 cmdListTransfer		= 0;
		};

		struct CreateWorldRendererRequest
		{
			Delegate<void(WorldRenderer*)> onCreated;
			EntityWorld*				   world = nullptr;
			Vector2						   size	 = Vector2::Zero;
			WorldRendererMask			   mask;
		};

		struct DeleteWorldRendererRequest
		{
			WorldRenderer* renderer = nullptr;
		};

	public:
		SurfaceRenderer(GfxManager* man, uint32 imageCount, StringID sid, IWindow* window, const Vector2i& initialSize, Bitmask16 mask);
		virtual ~SurfaceRenderer();

		void		 Tick(float interpolationAlpha);
		void		 Sync();
		void		 Render(int surfaceRendererIndex, uint32 frameIndex);
		void		 Present();
		void		 SetOffscreenTexture(Texture* txt, uint32 imageIndex);
		void		 ClearOffscreenTexture();
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev) override;
		void		 SetGUIDrawer(IGUIDrawer* rend);
		void		 CreateWorldRenderer(Delegate<void(WorldRenderer*)>&& onCreated, EntityWorld* world, const Vector2& size, WorldRendererMask mask);
		void		 DeleteWorldRenderer(WorldRenderer* renderer);

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_WindowResized;
		}

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

		inline uint32 GetCurrentImageIndex()
		{
			return m_currentImageIndex;
		}

		inline int GetSurfaceRendererIndex() const
		{
			return m_surfaceRendererIndex;
		}

		inline IGUIDrawer* GetGUIDrawer() const
		{
			return m_guiDrawer;
		}

	private:
		void CreateTextures();
		void DestroyTextures();

	protected:
		static int						   s_surfaceRendererCount;
		int								   m_surfaceRendererIndex = 0;
		Vector<CreateWorldRendererRequest> m_worldRendererCreateRequests;
		Vector<DeleteWorldRendererRequest> m_worldRendererDeleteRequests;
		IGfxContext*					   m_contextGraphics   = nullptr;
		IGfxContext*					   m_contextTransfer   = nullptr;
		IUploadContext*					   m_uploadContext	   = nullptr;
		GUIRenderer*					   m_guiRenderer	   = nullptr;
		uint32							   m_currentImageIndex = 0;
		GfxManager*						   m_gfxManager		   = nullptr;
		ISwapchain*						   m_swapchain		   = nullptr;
		Bitmask16						   m_mask			   = 0;
		uint32							   m_imageCount		   = 0;
		Vector<WorldRenderer*>			   m_worldRenderers;
		IGUIDrawer*						   m_guiDrawer = nullptr;
		Renderer*						   m_renderer  = nullptr;
		Vector<DataPerImage>			   m_dataPerImage;
		DataPerFrame					   m_frames[FRAMES_IN_FLIGHT];
		RenderData						   m_renderData;
	};

} // namespace Lina

#endif
