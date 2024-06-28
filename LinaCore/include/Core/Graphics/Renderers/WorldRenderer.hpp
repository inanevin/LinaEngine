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
#include "Core/Graphics/Renderers/Renderer.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
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
	class GUIWidget;
	class WidgetComponent;
	class Material;
	class Texture;
	class TextureSampler;
	class ResourceManager;
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

	class WorldRenderer : public EntityWorldListener, public Renderer
	{
	private:
		struct PerFrameData
		{
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
		};

	public:
		WorldRenderer(GfxManager* man, EntityWorld* world, const Vector2ui& viewSize, Buffer* snapshotBuffer = nullptr);
		~WorldRenderer();

		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void Render(uint32 frameIndex, uint32 waitCount, uint16* waitSemaphores, uint64* waitValues) override;
		virtual void Resize(const Vector2ui& newSize);

		/// If this renderer is submitting its own commands, return the submission semaphore so that the next batch can wait on them.
		virtual SemaphoreData GetSubmitSemaphore(uint32 frameIndex) override
		{
			if (m_snapshotBuffer != nullptr)
				return m_pfd[frameIndex].copySemaphore;

			return m_pfd[frameIndex].signalSemaphore;
		};

		virtual void OnComponentAdded(Component* c) override;
		virtual void OnComponentRemoved(Component* c) override;

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

	private:
		void   UpdateBuffers(uint32 frameIndex);
		void   FetchRenderables();
		void   CreateSizeRelativeResources();
		void   DestroySizeRelativeResources();
		uint64 BumpAndSendTransfers(uint32 frameIndex);

	private:
		Shader* m_guiShader3D			= nullptr;
		uint32	m_guiShader3DVariantGPU = 0;

		ResourceUploadQueue							  m_uploadQueue;
		PerFrameData								  m_pfd[FRAMES_IN_FLIGHT]	= {};
		RenderPass									  m_mainPass				= {};
		RenderPass									  m_lightingPass			= {};
		RenderPass									  m_forwardTransparencyPass = {};
		Vector2ui									  m_size					= Vector2ui::Zero;
		EntityWorld*								  m_world					= nullptr;
		Vector<MeshComponent*>						  m_meshComponents;
		Vector<WidgetComponent*>					  m_widgetComponents;
		Vector<GPUDataObject>						  m_objects = {};
		ResourceManager*							  m_rm		= nullptr;
		HashMap<Shader*, Vector<DrawDataMeshDefault>> m_drawData;
		TextureSampler*								  m_gBufSampler			   = nullptr;
		Shader*										  m_deferredLightingShader = nullptr;
		MeshDefault*								  m_skyCube				   = nullptr;
		Vector<WorldRendererExtension*>				  m_extensions;
		Buffer*										  m_snapshotBuffer = nullptr;
	};

} // namespace Lina
