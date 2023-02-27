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
#include "Event/IEventListener.hpp"
#include "Math/Rect.hpp"

namespace Lina
{
	class GfxManager;
	class SurfaceRenderer;
	class EntityWorld;
	class Texture;
	class Material;
	class IGfxResource;

	// DEBUG
	class ISwapchain;

	class WorldRenderer : public IEventListener
	{
	public:
		static ISwapchain* testSwapchain;
		uint32			   testImageIndex = 0;

		WorldRenderer(GfxManager* gfxManager, uint32 imageCount, SurfaceRenderer* surface, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio);
		virtual ~WorldRenderer();

		struct RenderData
		{
			RenderData() : allRenderables(100, ObjectWrapper<RenderableComponent>(nullptr)){};

			IDList<ObjectWrapper<RenderableComponent>> allRenderables;
			GPUSceneData							   gpuSceneData;
			GPUViewData								   gpuViewData;
		};

		struct DataPerFrame
		{
			uint32 cmdAllocator = 0;
			uint32 cmdList		= 0;
			uint64 storedFence	= 0;
		};

		struct DataPerImage
		{
			Texture*  renderTargetColor = nullptr;
			Texture*  renderTargetDepth = nullptr;
			Texture*  renderTargetPP	= nullptr;
			Material* ppMaterial		= nullptr;
		};

		Texture*	 GetFinalTexture();
		virtual void OnGameEvent(EGameEvent type, const Event& ev) override;
		virtual void Tick(float delta);
		virtual void Render(uint32 frameIndex);
		virtual void Join();

		virtual Bitmask32 GetGameEventMask()
		{
			return EVG_ComponentCreated | EVG_ComponentDestroyed;
		}

		inline void SetResolution(const Vector2i& res)
		{
			m_renderResolution = res;
		}

		inline void SetAspect(float aspect)
		{
			m_aspectRatio = aspect;
		}

	protected:
		void CreateTextures(const Vector2i& res, bool createMaterials);
		void DestroyTextures();

	protected:
		static int											s_worldRendererCount;
		uint32												m_imageCount = 0;
		RenderData											m_renderData;
		GfxManager*											m_gfxManager	   = nullptr;
		SurfaceRenderer*									m_surfaceRenderer  = nullptr;
		Bitmask16											m_mask			   = 0;
		EntityWorld*										m_world			   = nullptr;
		Vector2i											m_renderResolution = Vector2i::Zero;
		float												m_aspectRatio	   = 0.0f;
		HashMap<uint32, ObjectWrapper<RenderableComponent>> m_renderableIDs;
		DataPerFrame										m_frames[FRAMES_IN_FLIGHT];
		Vector<DataPerImage>								m_dataPerImage;
		uint32												m_fence		 = 0;
		uint64												m_fenceValue = 0;
		Viewport											m_viewport	 = Viewport();
		Recti												m_scissors	 = Recti();
	};
} // namespace Lina

#endif
