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

#ifndef WorldRenderer_HPP
#define WorldRenderer_HPP

#include "Data/Bitmask.hpp"
#include "Math/Vector.hpp"
#include "Data/Vector.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Data/IDList.hpp"
#include "Data/HashMap.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Core/ObjectWrapper.hpp"
#include "Event/IGameEventListener.hpp"
#include "Math/Rect.hpp"
#include "CameraSystem.hpp"
#include "View.hpp"
#include "DrawPass.hpp"

namespace Lina
{
	class GfxManager;
	class SurfaceRenderer;
	class EntityWorld;
	class Texture;
	class Material;
	class IGfxBufferResource;
	class Renderer;

	// DEBUG
	class ISwapchain;

	class WorldRenderer : public IGameEventListener
	{

	private:
		struct RenderData
		{
			RenderData() : allRenderables(100, ObjectWrapper<RenderableComponent>(nullptr)){};

			float												aspectRatio		 = 0.0f;
			Vector2i											renderResolution = Vector2i::Zero;
			HashMap<uint32, ObjectWrapper<RenderableComponent>> renderableIDs;
			IDList<ObjectWrapper<RenderableComponent>>			allRenderables;
			GPUSceneData										gpuSceneData;
			GPUViewData											gpuViewData;
			Viewport											viewport = Viewport();
			Recti												scissors = Recti();
			Vector<RenderableData>								extractedRenderables;
		};

		struct DataPerFrame
		{
			uint32				cmdAllocator	= 0;
			uint32				cmdList			= 0;
			IGfxBufferResource* sceneDataBuffer = nullptr;
			IGfxBufferResource* viewDataBuffer	= nullptr;
		};

		struct DataPerImage
		{
			Texture*  renderTargetColor = nullptr;
			Texture*  renderTargetDepth = nullptr;
			Texture*  renderTargetPP	= nullptr;
			Material* ppMaterial		= nullptr;
		};

	public:
		static ISwapchain* testSwapchain;
		uint32			   testImageIndex = 0;

		WorldRenderer(GfxManager* gfxManager, uint32 imageCount, SurfaceRenderer* surface, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio);
		virtual ~WorldRenderer();

		Texture*	 GetFinalTexture();
		virtual void OnGameEvent(GameEvent eventType, const Event& ev) override;
		virtual void Tick(float delta);
		virtual void Render(uint32 frameIndex, uint32 imageIndex);

		virtual Bitmask32 GetGameEventMask() override
		{
			return EVG_ComponentCreated | EVG_ComponentDestroyed;
		}

		inline void SetResolution(const Vector2i& res)
		{
			m_renderData.renderResolution = res;
		}

		inline void SetAspect(float aspect)
		{
			m_renderData.aspectRatio = aspect;
		}

	protected:
		void CreateTextures(const Vector2i& res, bool createMaterials);
		void DestroyTextures();

	protected:
		static int s_worldRendererCount;

		Renderer*			 m_renderer = nullptr;
		DrawPass			 m_opaquePass;
		CameraSystem		 m_cameraSystem;
		ResourceManager*	 m_resourceManager = nullptr;
		View				 m_playerView;
		uint32				 m_imageCount = 0;
		RenderData			 m_renderData;
		GfxManager*			 m_gfxManager	   = nullptr;
		SurfaceRenderer*	 m_surfaceRenderer = nullptr;
		Bitmask16			 m_mask			   = 0;
		EntityWorld*		 m_world		   = nullptr;
		DataPerFrame		 m_frames[FRAMES_IN_FLIGHT];
		Vector<DataPerImage> m_dataPerImage;
	};
} // namespace Lina

#endif
