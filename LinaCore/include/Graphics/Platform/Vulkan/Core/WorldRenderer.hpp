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

#include "Renderer.hpp"
#include "Data/IDList.hpp"
#include "DrawPass.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Core/View.hpp"
#include "Graphics/Core/CameraSystem.hpp"
#include "Graphics/Platform/Vulkan/Objects/Buffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/DescriptorSet.hpp"
#include "Event/IEventListener.hpp"
#include "Core/ObjectWrapper.hpp"

namespace Lina
{
	class SurfaceRenderer;
	class EntityWorld;
	class Texture;
	class GfxManager;
	class ResourceManager;

	class WorldRenderer : public Renderer, public IEventListener
	{

	public:
		struct RenderWorldData
		{
			RenderWorldData(DrawPass opaquePass, EntityWorld* w) : allRenderables(250, ObjectWrapper<RenderableComponent>()), opaquePass(opaquePass), world(w){};

			EntityWorld*							   world = nullptr;
			Vector<Texture*>						   finalColorTexture;
			Vector<Texture*>						   finalDepthTexture;
			Vector<Texture*>						   postProcessTexture;
			IDList<ObjectWrapper<RenderableComponent>> allRenderables;
			Vector<RenderableData>					   extractedRenderables;
			DrawPass								   opaquePass;
			View									   playerView;
			GPUSceneData							   sceneData;
			GPULightData							   lightData;
			Vector<Buffer>							   objDataBuffer;
			Vector<Buffer>							   indirectBuffer;
			Vector<Buffer>							   sceneDataBuffer;
			Vector<Buffer>							   viewDataBuffer;
			Vector<Buffer>							   lightDataBuffer;
			Vector<Buffer>							   globalDataBuffer;
			Vector<Buffer>							   debugDataBuffer;
			Vector<DescriptorSet>					   passDescriptor;
			Vector<DescriptorSet>					   globalDescriptor;
		};

	public:
		WorldRenderer(GfxManager* manager, SurfaceRenderer* surf, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio);
		virtual ~WorldRenderer();

		void				   SetAspectRatio(float aspect);
		void				   SetRenderResolution(const Vector2i& res);
		virtual void		   OnGameEvent(EGameEvent type, const Event& ev);
		virtual void		   SyncData(float alpha) override;
		virtual void		   Tick(float delta) override;
		virtual CommandBuffer* Render(uint32 frameIndex) override;
		Texture*			   GetFinalTexture();

		virtual Bitmask32 GetGameEventMask()
		{
			return EVG_ComponentCreated | EVG_ComponentDestroyed;
		}

		inline CameraSystem& GetCameraSystem()
		{
			return m_cameraSystem;
		}

		virtual EntityWorld* GetWorld() const
		{
			return m_targetWorldData.world;
		}

	private:
		void RenderWorld(uint32 frameIndex, const CommandBuffer& cmd, RenderWorldData& data);
		void CreateTextures(const Vector2i& res, bool createMaterials);
		void DestroyTextures();

	protected:
		SurfaceRenderer*									m_surfaceRenderer = nullptr;
		ResourceManager*									m_resourceManager = nullptr;
		RenderWorldData										m_targetWorldData;
		CameraSystem										m_cameraSystem;
		Vector<Material*>									m_worldPostProcessMaterials;
		Vector2i											m_renderResolution = Vector2i::Zero;
		Vector<SurfaceRenderer*>							m_finalTextureListeners;
		HashMap<uint32, ObjectWrapper<RenderableComponent>> m_renderableIDs;
		float												m_aspectRatio = 1.0f;
	};

} // namespace Lina

#endif