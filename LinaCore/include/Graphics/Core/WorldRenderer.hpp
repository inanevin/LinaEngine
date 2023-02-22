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

namespace Lina
{
	class IGfxManager;
	class SurfaceRenderer;
	class EntityWorld;
	class Texture;
	class Material;

	class WorldRenderer : public IEventListener
	{
	public:
		WorldRenderer(IGfxManager* gfxManager, uint32 imageCount, SurfaceRenderer* surface, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio);
		virtual ~WorldRenderer();

		struct RenderData
		{
			RenderData() : allRenderables(100, ObjectWrapper<RenderableComponent>(nullptr)){};

			Vector<Texture*>						   finalColorTexture;
			Vector<Texture*>						   finalDepthTexture;
			Vector<Texture*>						   finalPPTexture;
			IDList<ObjectWrapper<RenderableComponent>> allRenderables;
			GPUSceneData							   gpuSceneData;
			GPULightData							   gpuLightData;
			GPUViewData								   gpuViewData;
		};

		Texture*	 GetFinalTexture();
		virtual void OnGameEvent(EGameEvent type, const Event& ev) override;
		virtual void Tick(float delta) = 0;
		virtual void Render()		   = 0;
		virtual void Join()			   = 0;

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
		uint32												m_imageCount = 0;
		RenderData											m_renderData;
		IGfxManager*										m_gfxManager	   = nullptr;
		SurfaceRenderer*									m_surfaceRenderer  = nullptr;
		Bitmask16											m_mask			   = 0;
		EntityWorld*										m_world			   = nullptr;
		Vector2i											m_renderResolution = Vector2i::Zero;
		float												m_aspectRatio	   = 0.0f;
		HashMap<uint32, ObjectWrapper<RenderableComponent>> m_renderableIDs;
		Vector<Material*>									m_worldPostProcessMaterials;
	};
} // namespace Lina

#endif
