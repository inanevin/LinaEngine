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

#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Components/MeshComponent.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/System/System.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Common/Math/Math.hpp>

namespace Lina
{
#define MAX_GFX_COMMANDS  250
#define MAX_COPY_COMMANDS 50
#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100
#define MAX_OBJECTS		  256

	WorldRenderer::WorldRenderer(GfxManager* man, EntityWorld* world, const Vector2ui& viewSize)
	{
		m_world		 = world;
		m_gfxManager = man;
		m_lgx		 = m_gfxManager->GetLGX();
		m_world->AddListener(this);
		m_size = viewSize;
		m_rm   = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		auto* rm = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		//	m_skyCube	= rm->GetResource<Model>("Resources/Core/Models/SkyCube.glb"_hs)->GetFirstNodeWMesh();
		m_skyShader = rm->GetResource<Shader>("Resources/Core/Shaders/Sky/SimpleSky.linashader"_hs);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		= m_pfd[i];
			data.gfxStream	= m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "WorldRenderer: Gfx Stream"});
			data.copyStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, "WorldRenderer: Copy Stream"});
			data.objectBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPUDataObject) * MAX_OBJECTS, "WorldRenderer: ObjectData", false);
			data.sceneBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataScene), "WorldRenderer: SceneData", false);
			data.signalSemaphore.semaphore = m_lgx->CreateUserSemaphore();
			data.copySemaphore.semaphore   = m_lgx->CreateUserSemaphore();
		}

		CreateSizeRelativeResources();
		FetchRenderables();
	}

	WorldRenderer::~WorldRenderer()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.copyStream);
			data.objectBuffer.Destroy();
			data.sceneBuffer.Destroy();
			m_lgx->DestroyUserSemaphore(data.signalSemaphore.semaphore);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.semaphore);
		}
		DestroySizeRelativeResources();
		m_world->RemoveListener(this);
	}

	void WorldRenderer::CreateSizeRelativeResources()
	{
		LinaGX::TextureDesc rtDesc = {
			.format	   = DEFAULT_RT_FORMAT_HDR,
			.flags	   = LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	   = m_size.x,
			.height	   = m_size.y,
			.debugName = "WorldRendererTexture",
		};

		LinaGX::TextureDesc depthDesc = {
			.format					  = LinaGX::Format::D32_SFLOAT,
			.depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
			.flags					  = LinaGX::TF_DepthTexture | LinaGX::TF_Sampled,
			.width					  = m_size.x,
			.height					  = m_size.y,
			.debugName				  = "WorldRendererDepthTexture",
		};

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			const String colorTargetName = "WorldRenderer Color Target " + TO_STRING(i);
			const String depthTargetName = "WorldRenderer Depth Target " + TO_STRING(i);
			data.colorTarget			 = m_rm->CreateUserResource<Texture>(colorTargetName, TO_SID(colorTargetName));
			data.depthTarget			 = m_rm->CreateUserResource<Texture>(colorTargetName, TO_SID(colorTargetName));
			data.colorTarget->CreateGPUOnly(rtDesc);
			data.depthTarget->CreateGPUOnly(depthDesc);
			m_mainPass.SetColorAttachment(i, 0, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.colorTarget->GetGPUHandle(), .isSwapchain = false});
			m_mainPass.DepthStencilAttachment(i, {.useDepth = true, .texture = data.depthTarget->GetGPUHandle(), .clearDepth = 1.0f});
		}

		m_mainPass.Create(m_gfxManager, RenderPassDescriptorType::Main);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			m_lgx->DescriptorUpdateBuffer({
				.setHandle			= m_mainPass.GetDescriptorSet(i),
				.setAllocationIndex = 0,
				.binding			= 1,
				.buffers			= {data.sceneBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle			= m_mainPass.GetDescriptorSet(i),
				.setAllocationIndex = 0,
				.binding			= 2,
				.buffers			= {data.objectBuffer.GetGPUResource()},
			});
		}
	}

	void WorldRenderer::DestroySizeRelativeResources()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_rm->DestroyUserResource(data.colorTarget);
			m_rm->DestroyUserResource(data.depthTarget);
		}

		m_mainPass.Destroy();
	}

	void WorldRenderer::FetchRenderables()
	{
		m_meshComponents.clear();
		m_world->GetAllComponents<MeshComponent>(m_meshComponents);
	}

	void WorldRenderer::OnComponentAdded(Component* c)
	{
		if (c->GetComponentType() == GetTypeID<MeshComponent>())
			m_meshComponents.push_back(static_cast<MeshComponent*>(c));
	}

	void WorldRenderer::OnComponentRemoved(Component* c)
	{
		if (c->GetComponentType() == GetTypeID<MeshComponent>())
			m_meshComponents.erase(linatl::find_if(m_meshComponents.begin(), m_meshComponents.end(), [c](MeshComponent* model) -> bool { return c == model; }));
	}

	void WorldRenderer::Tick(float delta)
	{
	}

	void WorldRenderer::Resize(const Vector2ui& newSize)
	{
		m_size = newSize;
		DestroySizeRelativeResources();
		CreateSizeRelativeResources();
	}

	void WorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		CameraComponent* camera = m_world->GetActiveCamera();

		m_gpuDataView = {
			.screenSize = Vector2(static_cast<float>(m_size.x), static_cast<float>(m_size.y)),
		};
		if (camera == nullptr)
		{
			m_gpuDataView.view			= Matrix4::InitLookAt(Vector3(25, 25, 25), Vector3::Zero, Vector3::Up);
			m_gpuDataView.proj			= Matrix4::Perspective(90, static_cast<float>(m_size.x) / static_cast<float>(m_size.y), 0.1f, 100.0f);
			m_gpuDataView.viewProj		= m_gpuDataView.proj * m_gpuDataView.view;
			m_gpuDataView.cameraNearFar = Vector2(0.1f, 100.0f);
		}
		else
		{
		}

		m_gpuDataScene = {.skyTop = Vector4(1, 0, 0, 1), .skyHor = Vector4(1, 1, 1, 1), .skyBot = Vector4(1, 0, 0, 1)};
		currentFrame.sceneBuffer.BufferData(0, (uint8*)&m_gpuDataScene, sizeof(GPUDataScene));

		m_mainPass.GetBuffer(frameIndex, 0).BufferData(0, (uint8*)&m_gpuDataView, sizeof(GPUDataView));

		Vector<Entity*> entities;
		m_world->GetAllEntities(entities);
		m_objects.resize(entities.size());

		for (size_t i = 0; i < entities.size(); i++)
		{
			Entity* e	   = entities[i];
			e->m_ssboIndex = static_cast<uint32>(i);
			auto& data	   = m_objects[i];
			data.model	   = e->GetTransform().GetMatrix();
			// static Quaternion rot = Quaternion();
			// static float angle = 0.0f;
			// angle += 0.016f * 10;
			// rot.AxisAngle(Vector3(0, 1,0), angle);
			// data.model = Matrix4::TransformMatrix(Vector3(Math::RandF(-10, 10), Math::RandF(-10, 10), Math::RandF(-10, 10)), rot, Vector3::One);
		}
		currentFrame.objectBuffer.BufferData(0, (uint8*)m_objects.data(), sizeof(GPUDataObject) * m_objects.size());

		// Draw data map.
		m_drawDataMap.clear();
		for (auto* mc : m_meshComponents)
		{
			MeshDefault* mesh	  = mc->GetMeshRaw();
			Material*	 material = mc->GetMaterialRaw();
			auto&		 vec	  = m_drawDataMap[material];

			auto it = linatl::find_if(vec.begin(), vec.end(), [mesh](const DrawDataMeshDefault& dd) -> bool { return dd.mesh == mesh; });

			if (it != vec.end())
				it->entityIndices.push_back(mc->GetEntity()->m_ssboIndex);
			else
			{
				DrawDataMeshDefault drawData = {
					.mesh		   = mesh,
					.entityIndices = {mc->GetEntity()->m_ssboIndex},
				};
				vec.push_back(drawData);
			}
		}

		// Indirect commands.
		uint32 constantsCount = 0;
		for (auto& [material, meshDataVec] : m_drawDataMap)
		{
			uint32 drawID = 0;

			for (const auto& md : meshDataVec)
			{
				for (uint32 entityID : md.entityIndices)
				{
					m_lgx->BufferIndexedIndirectCommandData(m_mainPass.GetBuffer(frameIndex, 1).GetMapped(), drawID * m_lgx->GetIndexedIndirectCommandSize(), drawID, md.mesh->GetIndexCount(), 1, md.mesh->GetIndexOffset(), md.mesh->GetVertexOffset(), 0);
					drawID++;

					GPUIndirectConstants0 constants = {
						.entityID = entityID,
					};
					m_mainPass.GetBuffer(frameIndex, 2).BufferData(constantsCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));
					constantsCount++;
				}
			}
		}
	}

	SemaphoreData WorldRenderer::Render(uint32 frameIndex, const SemaphoreData& waitSemaphore)
	{
		UpdateBuffers(frameIndex);

		auto&		 currentFrame		= m_pfd[frameIndex];
		const uint64 copySemaphoreValue = currentFrame.copySemaphore.value;

		const LinaGX::Viewport viewport = {
			.x		  = 0,
			.y		  = 0,
			.width	  = m_size.x,
			.height	  = m_size.y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		const LinaGX::ScissorsRect scissors = {
			.x		= 0,
			.y		= 0,
			.width	= m_size.x,
			.height = m_size.y,
		};

		// Global vertex/index buffers.
		m_gfxManager->GetMeshManager().BindBuffers(currentFrame.gfxStream, 0);

		// Global set.
		LinaGX::CMDBindDescriptorSets* bindGlobal = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bindGlobal->descriptorSetHandles		  = currentFrame.gfxStream->EmplaceAuxMemory<uint16>(m_gfxManager->GetDescriptorSetPersistentGlobal(frameIndex));
		bindGlobal->firstSet					  = 0;
		bindGlobal->setCount					  = 1;
		bindGlobal->layoutSource				  = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bindGlobal->customLayout				  = m_gfxManager->GetPipelineLayoutPersistentGlobal(frameIndex);

		uint8 transferExists = 0;

		transferExists |= currentFrame.objectBuffer.Copy(currentFrame.copyStream);
		transferExists |= currentFrame.sceneBuffer.Copy(currentFrame.copyStream);
		transferExists |= m_mainPass.GetBuffer(frameIndex, 0).Copy(currentFrame.copyStream); // view
		transferExists |= m_mainPass.GetBuffer(frameIndex, 1).Copy(currentFrame.copyStream); // indirect
		transferExists |= m_mainPass.GetBuffer(frameIndex, 2).Copy(currentFrame.copyStream); // indirect args

		if (transferExists != 0)
			BumpAndSendTransfers(frameIndex);

		// Barrier to Attachment
		LinaGX::CMDBarrier* barrierToAttachment	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToAttachment->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
		barrierToAttachment->dstStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
		barrierToAttachment->textureBarrierCount = 2;
		barrierToAttachment->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
		barrierToAttachment->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.colorTarget->GetGPUHandle());
		barrierToAttachment->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierDepthRead2Att(currentFrame.depthTarget->GetGPUHandle());

		m_mainPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_mainPass.BindDescriptors(currentFrame.gfxStream, frameIndex, false);

		Shader*	  lastBoundShader	= nullptr;
		Material* lastBoundMaterial = nullptr;

		uint32 indirectOffset = 0;
		uint32 batchStart	  = 0;
		for (auto& [material, meshDataVec] : m_drawDataMap)
		{
			auto* shader = material->GetShader();
			if (shader != lastBoundShader)
			{
				shader->Bind(currentFrame.gfxStream, shader->GetGPUHandle());
				lastBoundShader = shader;
			}
			if (material != lastBoundMaterial)
			{
				material->Bind(currentFrame.gfxStream, frameIndex, LinaGX::DescriptorSetsLayoutSource::CustomLayout);
				lastBoundMaterial = material;
			}

			LinaGX::CMDBindConstants* constants = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindConstants>();
			constants->size						= sizeof(uint32);
			constants->data						= currentFrame.gfxStream->EmplaceAuxMemory(batchStart);
			constants->stages					= currentFrame.gfxStream->EmplaceAuxMemory(LinaGX::ShaderStage::Vertex);
			constants->stagesSize				= 1;

			uint32 total = 0;

			for (const auto& d : meshDataVec)
				total += static_cast<uint32>(d.entityIndices.size());

			LinaGX::CMDDrawIndexedIndirect* draw = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawIndexedIndirect>();
			draw->count							 = total;
			draw->indirectBuffer				 = m_mainPass.GetBuffer(frameIndex, 1).GetGPUResource();
			draw->indirectBufferOffset			 = indirectOffset;

			indirectOffset += total * static_cast<uint32>(m_lgx->GetIndexedIndirectCommandSize());
			batchStart += total;
		}

		m_mainPass.End(currentFrame.gfxStream);

		// Barrier to shader read.
		LinaGX::CMDBarrier* barrierFromAttachment  = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierFromAttachment->srcStageFlags	   = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
		barrierFromAttachment->dstStageFlags	   = LinaGX::PSF_FragmentShader;
		barrierFromAttachment->textureBarrierCount = 2;
		barrierFromAttachment->textureBarriers	   = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
		barrierFromAttachment->textureBarriers[0]  = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.colorTarget->GetGPUHandle());
		barrierFromAttachment->textureBarriers[1]  = GfxHelpers::GetTextureBarrierDepthAtt2Read(currentFrame.depthTarget->GetGPUHandle());

		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);

		Vector<uint16> waitSemaphores;
		Vector<uint64> waitValues;

		if (waitSemaphore.value != 0)
		{
			waitSemaphores.push_back(waitSemaphore.semaphore);
			waitValues.push_back(waitSemaphore.value);
		}

		if (currentFrame.copySemaphore.value != copySemaphoreValue)
		{
			waitSemaphores.push_back(currentFrame.copySemaphore.semaphore);
			waitValues.push_back(currentFrame.copySemaphore.value);
		}

		currentFrame.signalSemaphore.value++;

		m_lgx->SubmitCommandStreams({
			.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams		  = &currentFrame.gfxStream,
			.streamCount	  = 1,
			.useWait		  = !waitSemaphores.empty(),
			.waitCount		  = static_cast<uint32>(waitSemaphores.size()),
			.waitSemaphores	  = waitSemaphores.data(),
			.waitValues		  = waitValues.data(),
			.useSignal		  = true,
			.signalCount	  = 1,
			.signalSemaphores = &currentFrame.signalSemaphore.semaphore,
			.signalValues	  = &currentFrame.signalSemaphore.value,
			.isMultithreaded  = true,
		});

		return currentFrame.signalSemaphore;
	}

	void WorldRenderer::DrawSky(LinaGX::CommandStream* stream)
	{
		m_skyShader->Bind(stream, m_skyShader->GetGPUHandle());
		// m_skyCube->GetMesh()->Draw(stream, 1);
	}

	uint64 WorldRenderer::BumpAndSendTransfers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.value++;
		m_lgx->CloseCommandStreams(&currentFrame.copyStream, 1);
		m_lgx->SubmitCommandStreams({
			.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
			.streams		  = &currentFrame.copyStream,
			.streamCount	  = 1,
			.useSignal		  = true,
			.signalCount	  = 1,
			.signalSemaphores = &(currentFrame.copySemaphore.semaphore),
			.signalValues	  = &currentFrame.copySemaphore.value,
			.isMultithreaded  = true,
		});
		return currentFrame.copySemaphore.value;
	}

} // namespace Lina
