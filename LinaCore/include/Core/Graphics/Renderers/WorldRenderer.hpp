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
#include "Core/Graphics/MeshManager.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"

namespace LinaGX
{
	class CommandStream;
	class Instance;

} // namespace LinaGX

namespace Lina
{
	class CompModel;
	class CompLight;
	class Shader;
	class Texture;
	class ResourceManagerV2;
	class WorldRenderer;
	class WorldRendererListener;
	class EntityWorld;
	class Screen;
	class GfxContext;

	class WorldRenderer
	{
	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream	   = nullptr;
			LinaGX::CommandStream* copyStream	   = nullptr;
			SemaphoreData		   copySemaphore   = {};
			SemaphoreData		   signalSemaphore = {};

			Texture* gBufAlbedo			= nullptr;
			Texture* gBufPosition		= nullptr;
			Texture* gBufNormal			= nullptr;
			Texture* gBufDepth			= nullptr;
			Texture* lightingPassOutput = nullptr;

			Buffer entityBuffer	   = {};
			Buffer argumentsBuffer = {};
			Buffer boneBuffer	   = {};
			Buffer lightBuffer	   = {};

			Buffer lvgVtxBuffer;
			Buffer lvgIdxBuffer;
			Buffer line3DVtxBuffer;
			Buffer line3DIdxBuffer;
		};

	public:
		struct DrawData
		{
			Vector<DrawEntity>		 entities;
			Vector<GPUDrawArguments> arguments;
			Vector<Matrix4>			 bones;
			Vector<Line3DVertex>	 line3DVertices;
			Vector<uint16>			 line3DIndices;
			Vector<LinaVG::Vertex>	 lvgVertices;
			Vector<LinaVG::Index>	 lvgIndices;
			Vector<GPULight>		 lights;
		};

	public:
		WorldRenderer(GfxContext* context, ResourceManagerV2* rm, EntityWorld* world, const Vector2ui& viewSize, const String& name = "", Buffer* snapshotBuffer = nullptr, bool standaloneSubmit = false);
		~WorldRenderer();

		void Tick(float delta);
		void UpdateBuffers(uint32 frameIndex);
		void FlushTransfers(uint32 frameIndex);
		void Render(uint32 frameIndex);
		void CloseAndSend(uint32 frameIndex);
		void Resize(const Vector2ui& newSize);
		void SyncRender();

		void AddListener(WorldRendererListener* listener);
		void RemoveListener(WorldRendererListener* listener);

		uint32 GetBoneIndex(CompModel* comp);
		uint32 GetArgumentCount();
		uint32 PushEntity(const GPUEntity& e, const EntityIdent& ident);
		uint32 PushArgument(const GPUDrawArguments& args);

		void			StartLinaVGBatch();
		LinaVG::Drawer* GetLVGDrawer()
		{
			return &m_lvgDrawer;
		}
		void EndLinaVGBatch(RenderPass& pass, uint32 pushConstantValue, uint32 shaderHandle);
		void OnLinaVGDraw(LinaVG::DrawBuffer* buffer);

		inline SemaphoreData GetSubmitSemaphore(uint32 frameIndex)
		{
			if (m_snapshotBuffer != nullptr)
				return m_pfd[frameIndex].copySemaphore;

			return m_pfd[frameIndex].signalSemaphore;
		};

		inline EntityWorld* GetWorld() const
		{
			return m_world;
		}

		inline const Vector2ui& GetSize() const
		{
			return m_size;
		}

		inline Texture* GetGBufAlbedo(uint32 frameIndex)
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

		inline const String& GetName() const
		{
			return m_name;
		}

		inline SemaphoreData& GetSignalSemaphoreData(uint32 frameIndex)
		{
			return m_pfd[frameIndex].signalSemaphore;
		}

		inline SemaphoreData& GetCopySemaphoreData(uint32 frameIndex)
		{
			return m_pfd[frameIndex].copySemaphore;
		}

		inline ResourceUploadQueue& GetUploadQueue()
		{
			return m_uploadQueue;
		}

		inline LinaGX::CommandStream* GetGfxStream(uint32 frameIndex) const
		{
			return m_pfd[frameIndex].gfxStream;
		}

		inline JobExecutor& GetExecutor()
		{
			return m_executor;
		}

		inline Buffer& GetEntityDataBuffer(uint32 frameIndex)
		{
			return m_pfd[frameIndex].entityBuffer;
		}

		inline Buffer& GetInstanceDataBuffer(uint32 frameIndex)
		{
			return m_pfd[frameIndex].argumentsBuffer;
		}

		inline Buffer& GetBoneBuffer(uint32 frameIndex)
		{
			return m_pfd[frameIndex].boneBuffer;
		}

		inline BufferedGroup<Buffer, FRAMES_IN_FLIGHT> GetLine3DVtxBufferGroup()
		{
			BufferedGroup<Buffer, FRAMES_IN_FLIGHT> gr;

			for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				gr.items[i] = &m_pfd[i].line3DVtxBuffer;

			return gr;
		}

		inline BufferedGroup<Buffer, FRAMES_IN_FLIGHT> GetLine3DIdxBufferGroup()
		{
			BufferedGroup<Buffer, FRAMES_IN_FLIGHT> gr;

			for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				gr.items[i] = &m_pfd[i].line3DIdxBuffer;

			return gr;
		}

		inline const Vector<CompModel*>& GetCompModels() const
		{
			return m_compModels;
		}

		inline const Vector<DrawEntity>& GetEntitiesGPU() const
		{
			return m_gpuDrawData.entities;
		}

		inline const Vector<CompLight*>& GetCompLights() const
		{
			return m_compLights;
		}

		inline DrawData& GetCPUDrawData()
		{
			return m_cpuDrawData;
		}

	private:
		void CalculateSkinning(const Vector<CompModel*>& comps);

	private:
		void   CreateSizeRelativeResources();
		void   DestroySizeRelativeResources();
		uint64 BumpAndSendTransfers(uint32 frameIndex);
		bool   DrawEntityExists(uint32& outIndex, const EntityIdent& ident);

	private:
		Vector<WorldRendererListener*> m_listeners;
		GUIBackend					   m_guiBackend;
		ResourceManagerV2*			   m_resourceManagerV2 = nullptr;
		LinaGX::Instance*			   m_lgx			   = nullptr;
		ResourceUploadQueue			   m_uploadQueue;
		PerFrameData				   m_pfd[FRAMES_IN_FLIGHT]	= {};
		RenderPass					   m_deferredPass			= {};
		RenderPass					   m_forwardPass			= {};
		Vector2ui					   m_size					= Vector2ui::Zero;
		EntityWorld*				   m_world					= nullptr;
		TextureSampler*				   m_gBufSampler			= nullptr;
		Shader*						   m_deferredLightingShader = nullptr;
		Buffer*						   m_snapshotBuffer			= nullptr;
		bool						   m_standaloneSubmit		= false;
		GfxContext*					   m_gfxContext;
		String						   m_name = "";
		JobExecutor					   m_executor;
		Vector<CompModel*>			   m_compModels;
		Vector<CompLight*>			   m_compLights;
		DrawData					   m_cpuDrawData = {};
		DrawData					   m_gpuDrawData = {};
		Vector<CompModel*>			   m_skinnedModels;

		LinaVG::Drawer m_lvgDrawer;

		uint32 m_currentLine3DStartVertex = 0;
		uint32 m_currentLine3DStartIndex  = 0;

		uint32 m_currentLvgStartVertex = 0;
		uint32 m_currentLvgStartIndex  = 0;
	};

} // namespace Lina
