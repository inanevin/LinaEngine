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

#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Common/Data/Map.hpp"

namespace LinaGX
{
	class CommandStream;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	class GfxManager;
	class ModelNode;
	class MeshComponent;
	class Shader;
	class Material;
	class Texture;
	class ResourceManager;

	class WorldRenderer : public EntityWorldListener
	{
	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream				= nullptr;
			LinaGX::CommandStream* copyStream				= nullptr;
			SemaphoreData		   copySemaphore			= {};
			SemaphoreData		   signalSemaphore			= {};
			Buffer				   guiVertexBuffer			= {};
			Buffer				   guiIndexBuffer			= {};
			Buffer				   guiMaterialBuffer		= {};
			Buffer				   objectBuffer				= {};
			Buffer				   sceneBuffer				= {};
			uint32				   colorTargetBindlessIndex = 0;

			Texture* colorTarget = nullptr;
			Texture* depthTarget = nullptr;
		};

	public:
		WorldRenderer(GfxManager* man, EntityWorld* world, const Vector2ui& viewSize);
		~WorldRenderer();

		void		  Tick(float delta);
		SemaphoreData Render(uint32 frameIndex, const SemaphoreData& waitSemaphore);
		void		  Resize(const Vector2ui& newSize);

		virtual void OnComponentAdded(Component* c) override;
		virtual void OnComponentRemoved(Component* c) override;

		inline const SemaphoreData& GetCopySemaphore(uint32 index) const
		{
			return m_pfd[index].copySemaphore;
		}

		inline EntityWorld* GetWorld() const
		{
			return m_world;
		}

		inline const Vector2ui& GetSize() const
		{
			return m_size;
		}

		inline Texture* GetTexture(uint32 frameIndex)
		{
			return m_pfd[frameIndex].colorTarget;
		}

	private:
		void   UpdateBuffers(uint32 frameIndex);
		void   FetchRenderables();
		void   DrawSky(LinaGX::CommandStream* stream);
		void   CreateSizeRelativeResources();
		void   DestroySizeRelativeResources();
		uint64 BumpAndSendTransfers(uint32 frameIndex);

	private:
		GfxManager*			   m_gfxManager			   = nullptr;
		LinaGX::Instance*	   m_lgx				   = nullptr;
		PerFrameData		   m_pfd[FRAMES_IN_FLIGHT] = {};
		RenderPass			   m_mainPass			   = {};
		Vector2ui			   m_size				   = Vector2ui::Zero;
		EntityWorld*		   m_world				   = nullptr;
		ModelNode*			   m_skyCube			   = nullptr;
		Shader*				   m_skyShader			   = nullptr;
		Vector<MeshComponent*> m_meshComponents;
		GPUDataView			   m_gpuDataView  = {};
		GPUDataScene		   m_gpuDataScene = {};
		Vector<GPUDataObject>  m_objects	  = {};
		ResourceManager*	   m_rm			  = nullptr;
		MaterialToMeshDataMap  m_drawDataMap;
	};

} // namespace Lina
