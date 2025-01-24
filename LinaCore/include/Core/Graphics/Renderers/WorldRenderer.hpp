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
#include "Core/Graphics/Renderers/ShapeRenderer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Core/Graphics/MeshManager.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/World/Camera.hpp"

namespace LinaGX
{
	class CommandStream;
	class Instance;

} // namespace LinaGX

namespace Lina
{
	class CompModel;
	class CompLight;
	class CompWidget;
	class Shader;
	class Texture;
	class TextureSampler;
	class ResourceManagerV2;
	class WorldRenderer;
	class WorldRendererListener;
	class EntityWorld;
	class Entity;
	class Screen;
	class GfxContext;
	class PhysicsDebugRenderer;

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
		};

		struct LineData
		{
			Vector3	  p1		= Vector3::Zero;
			Vector3	  p2		= Vector3::Zero;
			float	  thickness = 1.0f;
			ColorGrad color		= Color::White;
		};

	public:
		struct DrawData
		{
			Vector<DrawEntity>		 entities;
			Vector<GPUDrawArguments> arguments;
			Vector<Matrix4>			 bones;
			Vector<LinaVG::Vertex>	 lvgVertices;
			Vector<LinaVG::Index>	 lvgIndices;
			Vector<GPULight>		 lights;
			Camera					 worldCamera;
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
		void CreatePhysicsDebugRenderer();

		void AddListener(WorldRendererListener* listener);
		void RemoveListener(WorldRendererListener* listener);

		uint32 GetBoneIndex(CompModel* comp);
		uint32 GetArgumentCount();
		uint32 PushEntity(const GPUEntity& e, const EntityIdent& ident);
		uint32 PushArgument(const GPUDrawArguments& args);

		void AddDebugLine(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color);

		void OnWorldLVGDraw(LinaVG::DrawBuffer* buf);

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

		inline ShapeRenderer& GetShapeRenderer()
		{
			return m_shapeRenderer;
		}

		inline void SetDrawPhysics(bool df)
		{
			m_drawPhysics = df;
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
		ResourceManagerV2*			   m_resourceManagerV2 = nullptr;
		LinaGX::Instance*			   m_lgx			   = nullptr;
		ResourceUploadQueue			   m_uploadQueue;
		PerFrameData				   m_pfd[FRAMES_IN_FLIGHT] = {};
		RenderPass					   m_deferredPass		   = {};
		RenderPass					   m_forwardPass		   = {};
		Vector2ui					   m_size				   = Vector2ui::Zero;
		EntityWorld*				   m_world				   = nullptr;
		TextureSampler*				   m_gBufSampler		   = nullptr;
		TextureSampler*				   m_samplerGUIText		   = nullptr;
		Shader*						   m_shaderLightingQuad	   = nullptr;
		Shader*						   m_shaderDebugLine	   = nullptr;
		Buffer*						   m_snapshotBuffer		   = nullptr;
		bool						   m_standaloneSubmit	   = false;
		GfxContext*					   m_gfxContext;
		String						   m_name = "";
		JobExecutor					   m_executor;
		Vector<CompModel*>			   m_compModels;
		Vector<CompLight*>			   m_compLights;
		Vector<CompWidget*>			   m_compWidgets;
		DrawData					   m_cpuDrawData = {};
		DrawData					   m_gpuDrawData = {};
		Vector<CompModel*>			   m_skinnedModels;
		PhysicsDebugRenderer*		   m_physicsDebugRenderer = nullptr;

		ShapeRenderer m_shapeRenderer;

		uint32			 m_currentLvgStartVertex = 0;
		uint32			 m_currentLvgStartIndex	 = 0;
		Vector<LineData> m_debugLines;
		Entity*			 m_currentLVGDrawEntity = nullptr;
		bool			 m_currentLVGDraw3D		= false;
		bool			 m_drawPhysics			= false;
	};

} // namespace Lina
