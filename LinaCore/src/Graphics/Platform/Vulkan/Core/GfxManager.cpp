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

#include "Graphics/Platform/Vulkan/Core/GfxManager.hpp"
#include "Graphics/Platform/Vulkan/Core/Renderer.hpp"
#include "Graphics/Platform/Vulkan/Core/SurfaceRenderer.hpp"
#include "Graphics/Platform/Vulkan/Core/WorldRenderer.hpp"
#include "Graphics/Platform/Vulkan/Objects/Swapchain.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Core/CoreResourcesRegistry.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "FileSystem/FileSystem.hpp"
#include "System/ISystem.hpp"

#include "World/Core/EntityWorld.hpp"
#include "Graphics/Core/Window.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Math/Math.hpp"
namespace Lina
{

#define DEF_VTXBUF_SIZE	  sizeof(Vertex) * 2000
#define DEF_INDEXBUF_SIZE sizeof(uint32) * 2000

	EntityWorld* testWorld = nullptr;
	Entity*		 entity	   = nullptr;

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Gfx Manager] -> Initialization.");

		m_windowManager = m_system->GetSubsystem<WindowManager>(SubsystemType::WindowManager);
		m_backend.Initialize(initInfo, m_system->GetSubsystem<WindowManager>(SubsystemType::WindowManager)->GetWindow(LINA_MAIN_SWAPCHAIN));

		UserBinding globalBinding = UserBinding{
			.binding		 = 0,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Fragment, ShaderStage::Vertex},
		};

		UserBinding debugBinding = UserBinding{
			.binding		 = 1,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Fragment, ShaderStage::Vertex},
		};

		m_descriptorLayouts[DescriptorSetType::GlobalSet].device		= m_backend.GetDevice();
		m_descriptorLayouts[DescriptorSetType::GlobalSet].allocationCb	= m_backend.GetAllocationCb();
		m_descriptorLayouts[DescriptorSetType::GlobalSet].deletionQueue = &m_deletionQueue;
		m_descriptorLayouts[DescriptorSetType::GlobalSet].AddBinding(globalBinding).AddBinding(debugBinding).Create();

		UserBinding sceneBinding = UserBinding{
			.binding		 = 0,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Fragment, ShaderStage::Vertex},
		};

		UserBinding viewDataBinding = UserBinding{
			.binding		 = 1,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Vertex},
		};

		UserBinding lightDataBinding = UserBinding{
			.binding		 = 2,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Vertex},
		};

		UserBinding objDataBinding = UserBinding{
			.binding		 = 3,
			.descriptorCount = 1,
			.type			 = DescriptorType::StorageBuffer,
			.stages			 = {ShaderStage::Vertex},
		};

		m_descriptorLayouts[DescriptorSetType::PassSet].device		  = m_backend.GetDevice();
		m_descriptorLayouts[DescriptorSetType::PassSet].allocationCb  = m_backend.GetAllocationCb();
		m_descriptorLayouts[DescriptorSetType::PassSet].deletionQueue = &m_deletionQueue;
		m_descriptorLayouts[DescriptorSetType::PassSet].AddBinding(sceneBinding).AddBinding(viewDataBinding).AddBinding(lightDataBinding).AddBinding(objDataBinding).Create();

		// Pipeline layout
		m_globalAndPassLayout.device		= m_backend.GetDevice();
		m_globalAndPassLayout.allocationCb	= m_backend.GetAllocationCb();
		m_globalAndPassLayout.deletionQueue = &m_deletionQueue;
		m_globalAndPassLayout.AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::GlobalSet]).AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::PassSet]).Create();

		m_gpuUploader.Create();

		// Merged buffers.
		m_cpuVtxBuffer = Buffer{
			.allocator	 = m_backend.GetVMA(),
			.size		 = DEF_VTXBUF_SIZE,
			.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
			.memoryUsage = MemoryUsageFlags::CpuOnly,
		};

		m_cpuIndexBuffer = Buffer{
			.allocator	 = m_backend.GetVMA(),
			.size		 = DEF_INDEXBUF_SIZE,
			.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
			.memoryUsage = MemoryUsageFlags::CpuOnly,
		};

		m_gpuVtxBuffer = Buffer{
			.allocator	 = m_backend.GetVMA(),
			.size		 = DEF_VTXBUF_SIZE,
			.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
			.memoryUsage = MemoryUsageFlags::GpuOnly,
		};

		m_gpuIndexBuffer = Buffer{
			.allocator	 = m_backend.GetVMA(),
			.size		 = DEF_INDEXBUF_SIZE,
			.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
			.memoryUsage = MemoryUsageFlags::GpuOnly,
		};

		m_cpuVtxBuffer.Create();
		m_cpuIndexBuffer.Create();
		m_gpuVtxBuffer.Create();
		m_gpuIndexBuffer.Create();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_globalDataBuffer[i] = Buffer{
				.allocator	 = m_backend.GetVMA(),
				.size		 = sizeof(GPUGlobalData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_globalDataBuffer[i].Create();

			m_debugDataBuffer[i] = Buffer{
				.allocator	 = m_backend.GetVMA(),
				.size		 = sizeof(GPUDebugData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_debugDataBuffer[i].Create();

			RenderFrame& f = m_frames[i];

			f.graphicsFence = Fence{
				.device		   = m_backend.GetDevice(),
				.allocationCb  = m_backend.GetAllocationCb(),
				.deletionQueue = &m_deletionQueue,
				.flags		   = GetFenceFlags(FenceFlags::Signaled),
			};

			f.graphicsFence.Create();
		}
	}

	void GfxManager::Shutdown()
	{
		Join();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_globalDataBuffer[i].Destroy();
			m_debugDataBuffer[i].Destroy();
		}

		m_gpuIndexBuffer.Destroy();
		m_gpuVtxBuffer.Destroy();
		m_cpuIndexBuffer.Destroy();
		m_cpuVtxBuffer.Destroy();

		m_gpuUploader.Destroy();
		m_deletionQueue.Flush();
		m_backend.Shutdown();
	}

	void GfxManager::Join()
	{
		m_backend.WaitIdle();
		m_backend.GetGraphicsQueue().WaitIdle();
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			m_frames[i].graphicsFence.Wait();
	}

	float m_time = 0.0f;

	void GfxManager::Tick(float delta)
	{
		for (auto r : m_renderers)
			r->Tick();

		m_time += 0.01f;
		entity->AddRotation(Vector3(0, 15 * delta, 0));
		//entity->SetPosition(Vector3(0, 0, Math::Sin(m_time) * 2.0f));
	}

	void GfxManager::Render()
	{
		// Window manage return if minimized.

		// If not editor && app is not active return.
		m_gpuUploader.Poll();

		// if no renderers return.
		if (m_renderers.empty())
			return;

		const uint32 frameIndex = GetFrameIndex();
		RenderFrame& frame		= m_frames[frameIndex];

		m_globalDataBuffer[frameIndex].CopyInto(&m_globalData, sizeof(GPUGlobalData));
		m_debugDataBuffer[frameIndex].CopyInto(&m_debugData, sizeof(GPUDebugData));

		Vector<Renderer*> surfaceRenderers;
		Vector<Renderer*> worldRenderers;

		for (auto r : m_renderers)
		{
			if (r->GetType() == RendererType::SurfaceRenderer)
				surfaceRenderers.push_back(r);

			if (r->GetType() == RendererType::WorldRenderer)
				worldRenderers.push_back(r);
		}

		frame.graphicsFence.Wait(true, 1.0f);

		Vector<Renderer*> acquiredSurfaceRenderers;
		for (auto r : surfaceRenderers)
		{
			auto  swp			  = r->GetSwapchain();
			auto& submitSemaphore = swp->_submitSemaphores[frameIndex];
			if (submitSemaphore._markSignaled)
			{
				submitSemaphore.Destroy();
				submitSemaphore.Create();
			}

			const bool imageOK			  = r->AcquireImage(frameIndex);
			submitSemaphore._markSignaled = true;

			if (imageOK)
				acquiredSurfaceRenderers.push_back(r);
		}

		// Nothing to present.
		if (acquiredSurfaceRenderers.empty())
			return;

		frame.graphicsFence.Reset();

		// Render all worlds.
		Vector<uint32>		   imageIndices;
		Vector<Semaphore*>	   submitSemaphores;
		Vector<Semaphore*>	   presentSemaphores;
		Vector<Swapchain*>	   swapchains;
		Vector<CommandBuffer*> commandBuffers;

		commandBuffers.resize(worldRenderers.size());

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(worldRenderers.size()), 1, [&](int i) { commandBuffers[i] = worldRenderers[i]->Render(frameIndex, frame.graphicsFence); });
		m_system->GetMainExecutor()->RunAndWait(tf);

		for (auto r : acquiredSurfaceRenderers)
		{
			auto swp = r->GetSwapchain();
			imageIndices.push_back(r->GetAcquiredImage());
			commandBuffers.push_back(r->Render(frameIndex, frame.graphicsFence));
			submitSemaphores.push_back(&swp->_submitSemaphores[frameIndex]);
			presentSemaphores.push_back(&swp->_presentSemaphores[frameIndex]);
			swapchains.push_back(swp);
		}

		// m_guiBackend->Reset();

		// Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
		// Then submits command, and signals render semaphore when its submitted.
		m_backend.GetGraphicsQueue().Submit(submitSemaphores, presentSemaphores, frame.graphicsFence, commandBuffers, 1);
		VulkanResult res;
		m_backend.GetGraphicsQueue().Present(presentSemaphores, swapchains, imageIndices, res);

		for (auto r : acquiredSurfaceRenderers)
		{
			// Not unsignalled yet on the GPU, but we don't care, by the time fence is over it will be.
			r->GetSwapchain()->_submitSemaphores[frameIndex]._markSignaled = false;
			r->OnPostPresent(res);
		}

		m_frameNumber++;
	}

	void GfxManager::SyncData()
	{
		m_syncQueue.Flush();

		for (auto r : m_renderers)
			r->SyncData();
	}

	SurfaceRenderer* GfxManager::CreateSurfaceRenderer(Swapchain* swapchain, Bitmask16 mask)
	{
		SurfaceRenderer* rend = new SurfaceRenderer(this, swapchain, mask);
		m_renderers.push_back(rend);
		return rend;
	}

	WorldRenderer* GfxManager::CreateWorldRenderer(EntityWorld* world, Bitmask16 mask, const Vector2i& resolution, float aspect)
	{
		WorldRenderer* rend = new WorldRenderer(this, mask, world, resolution, aspect);
		m_renderers.push_back(rend);
		return rend;
	}

	void GfxManager::DestroySurfaceRenderer(Swapchain* swapchain)
	{
		auto it = linatl::find_if(m_renderers.begin(), m_renderers.end(), [swapchain](Renderer* r) { return r->GetSwapchain() == swapchain; });
		delete *it;
		m_renderers.erase(it);
	}

	void GfxManager::DestroyWorldRenderer(EntityWorld* world)
	{
		auto it = linatl::find_if(m_renderers.begin(), m_renderers.end(), [world](Renderer* r) { return r->GetWorld() == world; });
		delete *it;
		m_renderers.erase(it);
	}

	void GfxManager::OnSystemEvent(ESystemEvent type, const Event& ev)
	{
		if (type & EVS_PostSystemInit)
		{
			auto					rm			= m_system->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);
			const Vector<StringID>& coreShaders = CoreResourcesRegistry::GetCoreShaders();

			for (const StringID& sid : coreShaders)
			{
				const String   matPath = "Resources/Core/Materials/" + FileSystem::RemoveExtensionFromPath(FileSystem::GetFilenameAndExtensionFromPath(rm->GetResource<Shader>(sid)->GetPath())) + ".linamat";
				const StringID matSid  = TO_SID(matPath);
				Material*	   mat	   = new Material(rm, true, matPath, matSid);
				mat->SetShader(sid);
				m_engineMaterials.push_back(mat);
			}

			m_mainSwapchainSurfaceRenderer = CreateSurfaceRenderer(m_backend.GetSwapchain(LINA_MAIN_SWAPCHAIN), RendererMask::RM_DrawOffscreenTexture);

			testWorld = new EntityWorld(m_system);

			auto camEntity	  = testWorld->CreateEntity("CamEntity");
			auto camComponent = testWorld->AddComponent<CameraComponent>(camEntity);
			camEntity->SetPosition(Vector3(0, 0, -5));
			testWorld->SetActiveCamera(camComponent);

			entity = rm->GetResource<Model>("Resources/Core/Models/Cube.fbx"_hs)->AddToWorld(testWorld);

			m_currentLevelRenderer = CreateWorldRenderer(testWorld, 0, m_windowManager->GetWindow(LINA_MAIN_SWAPCHAIN)->GetSize(), m_windowManager->GetWindow(LINA_MAIN_SWAPCHAIN)->GetAspect());
			m_mainSwapchainSurfaceRenderer->SetOffscreenTexture(m_currentLevelRenderer->GetFinalTexture());
			m_currentLevelRenderer->AddFinalTextureListener(m_mainSwapchainSurfaceRenderer);
		}
		else if (type & EVS_PreSystemShutdown)
		{
			for (auto mat : m_engineMaterials)
				delete mat;

			m_engineMaterials.clear();

			DestroySurfaceRenderer(m_backend.GetSwapchain(LINA_MAIN_SWAPCHAIN));

			DestroyWorldRenderer(testWorld);
			delete testWorld;
		}
		else if (type & EVS_WindowResize)
		{
			const Vector2i newSize = Vector2i(ev.iParams[1], ev.iParams[2]);

			if (m_currentLevelRenderer && newSize.x != 0 && newSize.y != 0)
			{
				m_currentLevelRenderer->SetRenderResolution(newSize);
				m_currentLevelRenderer->SetAspectRatio(static_cast<float>(newSize.x) / static_cast<float>(newSize.y));
			}
		}
		else if (type & EVS_ResourceBatchLoaded)
		{
			Vector<ResourceIdentifier>* idents = static_cast<Vector<ResourceIdentifier>*>(ev.pParams[0]);
			const uint32				size   = static_cast<uint32>(idents->size());
			bool						merge  = false;

			for (uint32 i = 0; i < size; i++)
			{
				auto& ident = idents->at(i);

				if (ident.tid == GetTypeID<Model>())
				{
					merge = true;
					break;
				}
			}

			if (merge)
			{
				Join();
				auto rm = m_system->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);

				m_meshEntries.clear();
				m_mergedModelIDs.clear();

				Vector<Vertex> mergedVertices;
				Vector<uint32> mergedIndices;

				uint32 vertexOffset = 0;
				uint32 firstIndex	= 0;

				Vector<Model*> resources = rm->GetAllResourcesRaw<Model>();

				for (auto mod : resources)
				{
					m_mergedModelIDs.push_back(mod->GetSID());

					for (auto node : mod->GetNodes())
					{
						for (auto m : node->GetMeshes())
						{
							const auto& vertices = m->GetVertices();
							const auto& indices	 = m->GetIndices();

							MergedBufferMeshEntry entry;
							entry.vertexOffset = vertexOffset;
							entry.firstIndex   = firstIndex;
							entry.indexSize	   = static_cast<uint32>(indices.size());

							const uint32 vtxSize   = static_cast<uint32>(vertices.size());
							const uint32 indexSize = static_cast<uint32>(indices.size());
							for (auto& v : vertices)
								mergedVertices.push_back(v);

							for (auto& i : indices)
								mergedIndices.push_back(i);

							vertexOffset += vtxSize;
							firstIndex += indexSize;

							m_meshEntries[m] = entry;
						}
					}
				}

				const uint32 vtxSize   = static_cast<uint32>(mergedVertices.size() * sizeof(Vertex));
				const uint32 indexSize = static_cast<uint32>(mergedIndices.size() * sizeof(uint32));

				m_cpuVtxBuffer.CopyInto(mergedVertices.data(), vtxSize);
				m_cpuIndexBuffer.CopyInto(mergedIndices.data(), indexSize);

				// Realloc if necessary.
				if (m_gpuVtxBuffer.size < m_cpuVtxBuffer.size)
				{
					m_gpuVtxBuffer.Destroy();
					m_gpuVtxBuffer.size = m_cpuVtxBuffer.size;
					m_gpuVtxBuffer.Create();
				}

				if (m_gpuIndexBuffer.size < m_cpuIndexBuffer.size)
				{
					m_gpuIndexBuffer.Destroy();
					m_gpuIndexBuffer.size = m_cpuIndexBuffer.size;
					m_gpuIndexBuffer.Create();
				}

				GPUCommand vtxCmd;
				vtxCmd.Record = [this, vtxSize](CommandBuffer& cmd) {
					BufferCopy copy = BufferCopy{
						.destinationOffset = 0,
						.sourceOffset	   = 0,
						.size			   = vtxSize,
					};

					Vector<BufferCopy> regions;
					regions.push_back(copy);

					cmd.CMD_CopyBuffer(m_cpuVtxBuffer._ptr, m_gpuVtxBuffer._ptr, regions);
				};

				vtxCmd.OnSubmitted = [this]() {
					m_gpuVtxBuffer._ready = true;
					// f.cpuVtxBuffer.Destroy();
				};

				m_gpuUploader.SubmitImmediate(vtxCmd);

				GPUCommand indexCmd;
				indexCmd.Record = [this, indexSize](CommandBuffer& cmd) {
					BufferCopy copy = BufferCopy{
						.destinationOffset = 0,
						.sourceOffset	   = 0,
						.size			   = indexSize,
					};

					Vector<BufferCopy> regions;
					regions.push_back(copy);

					cmd.CMD_CopyBuffer(m_cpuIndexBuffer._ptr, m_gpuIndexBuffer._ptr, regions);
				};

				indexCmd.OnSubmitted = [this] {
					m_gpuIndexBuffer._ready = true;
					// f.cpuIndexBuffer.Destroy();
				};

				m_gpuUploader.SubmitImmediate(indexCmd);
			}
		}
	}

	DescriptorSetLayout& GfxManager::GetLayout(DescriptorSetType type)
	{
		return m_descriptorLayouts[type];
	}

} // namespace Lina
