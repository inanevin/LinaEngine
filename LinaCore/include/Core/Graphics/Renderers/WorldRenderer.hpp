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
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/MeshManager.hpp"
#include "Core/Resources/ResourceManagerListener.hpp"
#include "Common/Data/Map.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"

namespace LinaGX
{
	class CommandStream;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	class ModelNode;
	class MeshComponent;
	class Shader;
	class GUIWidget;
	class WidgetComponent;
	class Texture;
	class ResourceManagerV2;
	class WorldRenderer;

	class WorldRendererExtension
	{
	public:
		virtual ~WorldRendererExtension() = default;

	protected:
		friend class WorldRenderer;

		virtual void Tick(float delta){};
		virtual void RenderForward(uint32 frameIndex, LinaGX::CommandStream* stream){};
		virtual void Render(uint32 frameIndex, LinaGX::CommandStream* stream){};
		virtual void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue){};

		WorldRenderer* m_worldRenderer = nullptr;
	};

	class WorldRenderer : public EntityWorldListener, public ResourceManagerListener
	{
	private:
		struct PerFrameData
		{
			uint16							  pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Max];
			uint16							  pipelineLayoutPersistentGlobal = 0;
			uint16							  descriptorSetPersistentGlobal	 = 0;
			Buffer							  globalDataBuffer;
			Buffer							  globalMaterialsBuffer;
			LinaGX::DescriptorUpdateImageDesc globalTexturesDesc = {};
			LinaGX::DescriptorUpdateImageDesc globalSamplersDesc = {};

			LinaGX::CommandStream* gfxStream		 = nullptr;
			LinaGX::CommandStream* copyStream		 = nullptr;
			SemaphoreData		   copySemaphore	 = {};
			SemaphoreData		   signalSemaphore	 = {};
			Buffer				   guiVertexBuffer	 = {};
			Buffer				   guiIndexBuffer	 = {};
			Buffer				   guiMaterialBuffer = {};
			Buffer				   objectBuffer		 = {};

			Texture* gBufAlbedo			= nullptr;
			Texture* gBufPosition		= nullptr;
			Texture* gBufNormal			= nullptr;
			Texture* gBufDepth			= nullptr;
			Texture* lightingPassOutput = nullptr;

			bool bindlessDirty = false;
		};

	public:
		WorldRenderer(EntityWorld* world, const Vector2ui& viewSize, Buffer* snapshotBuffer = nullptr, bool standaloneSubmit = false);
		~WorldRenderer();

		void Tick(float delta);
		void Render(uint32 frameIndex);
		void Resize(const Vector2ui& newSize);

		virtual void OnResourceLoadEnded(int32 taskID, const Vector<Resource*>& resources) override;
		virtual void OnResourceUnloaded(const Vector<ResourceDef>& resources) override;
		virtual void OnComponentAdded(Component* c) override;
		virtual void OnComponentRemoved(Component* c) override;

		inline SemaphoreData GetSubmitSemaphore(uint32 frameIndex)
		{
			if (m_snapshotBuffer != nullptr)
				return m_pfd[frameIndex].copySemaphore;

			return m_pfd[frameIndex].signalSemaphore;
		};

		inline void AddExtension(WorldRendererExtension* ext)
		{
			ext->m_worldRenderer = this;
			m_extensions.push_back(ext);
		}

		inline void RemoveExtension(WorldRendererExtension* ext)
		{
			m_extensions.erase(linatl::find_if(m_extensions.begin(), m_extensions.end(), [ext](WorldRendererExtension* extension) -> bool { return ext == extension; }));
		}

		inline EntityWorld* GetWorld() const
		{
			return m_world;
		}

		inline const Vector2ui& GetSize() const
		{
			return m_size;
		}

		inline Texture* GetGBufColorMaterialID(uint32 frameIndex)
		{
			return m_pfd[frameIndex].gBufAlbedo;
		}

		inline Texture* GetGBufPosition(uint32 frameIndex)
		{
			return m_pfd[frameIndex].gBufPosition;
		}

		inline Texture* GetGBufNormal(uint32 frameIndex)
		{
			return m_pfd[frameIndex].gBufNormal;
		}

		inline Texture* GetGBufDepth(uint32 frameIndex)
		{
			return m_pfd[frameIndex].gBufDepth;
		}

		inline Texture* GetLightingPassOutput(uint32 frameIndex)
		{
			return m_pfd[frameIndex].lightingPassOutput;
		}

		inline Buffer* GetSnapshotBuffer() const
		{
			return m_snapshotBuffer;
		}

		inline void MarkBindlessDirty()
		{
			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				m_pfd[i].bindlessDirty = true;
		}

	private:
		void   UpdateBindlessResources(uint32 frameIndex);
		void   UpdateBuffers(uint32 frameIndex);
		void   FetchRenderables();
		void   CreateSizeRelativeResources();
		void   DestroySizeRelativeResources();
		uint64 BumpAndSendTransfers(uint32 frameIndex);

	private:
		GUIBackend									  m_guiBackend;
		MeshManager									  m_meshManager;
		ResourceManagerV2*							  m_resourceManagerV2 = nullptr;
		LinaGX::Instance*							  m_lgx				  = nullptr;
		ResourceUploadQueue							  m_uploadQueue;
		ResourceUploadQueue							  m_globalUploadQueue;
		PerFrameData								  m_pfd[FRAMES_IN_FLIGHT]	= {};
		RenderPass									  m_mainPass				= {};
		RenderPass									  m_lightingPass			= {};
		RenderPass									  m_forwardTransparencyPass = {};
		Vector2ui									  m_size					= Vector2ui::Zero;
		EntityWorld*								  m_world					= nullptr;
		Vector<MeshComponent*>						  m_meshComponents;
		Vector<WidgetComponent*>					  m_widgetComponents;
		Vector<GPUDataObject>						  m_objects = {};
		HashMap<Shader*, Vector<DrawDataMeshDefault>> m_drawData;
		TextureSampler*								  m_gBufSampler			   = nullptr;
		Shader*										  m_deferredLightingShader = nullptr;
		MeshDefault*								  m_skyCube				   = nullptr;
		Vector<WorldRendererExtension*>				  m_extensions;
		Buffer*										  m_snapshotBuffer	 = nullptr;
		bool										  m_standaloneSubmit = false;
	};

} // namespace Lina
