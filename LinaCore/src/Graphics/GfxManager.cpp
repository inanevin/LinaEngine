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

#include "Common/System/System.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Theme.hpp"

#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/Renderers/Renderer.hpp"

#include "Core/Resources/ResourceManager.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"

#include <memoryallocators/PoolAllocator.h>

namespace
{
	LINAGX_STRING FormatString(const char* fmt, va_list args)
	{
		// Determine the required size
		va_list args_copy;
		va_copy(args_copy, args);
		int size = vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for the null terminator
		va_end(args_copy);

		// Allocate a buffer and format the string
		std::vector<char> buffer(size);
		vsnprintf(buffer.data(), size, fmt, args);

		return std::string(buffer.data());
	}

	void LinaGX_ErrorCallback(const char* err, ...)
	{
		va_list args;
		va_start(args, err);
		std::string formattedStr = FormatString(err, args);
		LINA_ERR(formattedStr.c_str());
		va_end(args);
	}

	void LinaGX_LogCallback(const char* err, ...)
	{
		va_list args;
		va_start(args, err);
		std::string formattedStr = FormatString(err, args);
		LINA_INFO(formattedStr.c_str());
		va_end(args);
	}
} // namespace

namespace Lina
{

	LinaGX::Instance* GfxManager::s_lgx		= nullptr;
	LinaVG::Text*	  GfxManager::s_lvgText = nullptr;

	GfxManager::GfxManager(System* sys) : Subsystem(sys, SubsystemType::GfxManager), m_meshManager(this)
	{
		m_resourceManager = sys->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		// Setup LinaVG
		LinaVG::Config.globalFramebufferScale = 1.0f;
		LinaVG::Config.globalAAMultiplier	  = 1.0f;
		LinaVG::Config.gcCollectInterval	  = 4000;
		LinaVG::Config.textCachingEnabled	  = true;
		LinaVG::Config.textCachingSDFEnabled  = true;
		LinaVG::Config.textCacheReserve		  = 10000;
		LinaVG::Config.maxFontAtlasSize		  = 1024;
		LinaVG::Config.errorCallback		  = [](const std::string& err) { LINA_ERR(err.c_str()); };
		LinaVG::Config.logCallback			  = [](const std::string& log) { LINA_TRACE(log.c_str()); };

		m_guiBackend.Initialize(this);

		m_lvgText.GetCallbacks().fontTextureBind	   = std::bind(&GUIBackend::BindFontTexture, &m_guiBackend, std::placeholders::_1);
		m_lvgText.GetCallbacks().fontTextureBufferData = std::bind(&GUIBackend::BufferFontTextureAtlas, &m_guiBackend, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
		m_lvgText.GetCallbacks().fontTextureBufferEnd  = std::bind(&GUIBackend::BufferEnded, &m_guiBackend);
		m_lvgText.GetCallbacks().fontTextureCreate	   = std::bind(&GUIBackend::CreateFontTexture, &m_guiBackend, std::placeholders::_1, std::placeholders::_2);
		LinaVG::InitializeText();
	}

	void GfxManager::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		// Setup LinaGX
		m_lgx		   = new LinaGX::Instance();
		m_appDelegate  = m_system->GetApp()->GetAppDelegate();
		m_clearColor   = initInfo.clearColor;
		m_currentVsync = initInfo.vsyncStyle;
		m_resourceManager->AddListener(this);
		s_lgx	  = m_lgx;
		s_lvgText = &m_lvgText;

		LinaGX::Config.dx12Config = {
			.allowTearing				 = initInfo.allowTearing,
			.enableDebugLayers			 = true,
			.serializeShaderDebugSymbols = true,
		};

		LinaGX::Config.vulkanConfig = {
			.enableVulkanFeatures = LinaGX::VulkanFeatureFlags::VKF_Bindless | LinaGX::VulkanFeatureFlags::VKF_MultiDrawIndirect,
		};

		LinaGX::Config.gpuLimits = {
			.samplerLimit = 2048,
			.bufferLimit  = 2048,
		};

		LinaGX::Config.logLevel		 = LinaGX::LogLevel::Verbose;
		LinaGX::Config.errorCallback = LinaGX_ErrorCallback;
		LinaGX::Config.infoCallback	 = LinaGX_LogCallback;
		LinaGX::BackendAPI api		 = LinaGX::BackendAPI::Vulkan;

#ifdef LINA_PLATFORM_APPLE
		api = LinaGX::BackendAPI::Metal;
#endif

		LinaGX::Config.api						 = api;
		LinaGX::Config.gpu						 = LinaGX::PreferredGPUType::Discrete;
		LinaGX::Config.framesInFlight			 = FRAMES_IN_FLIGHT;
		LinaGX::Config.backbufferCount			 = BACK_BUFFER_COUNT;
		LinaGX::Config.mutexLockCreationDeletion = true;

		m_lgx->Initialize();

		// Default samplers
		/*
		{
			LinaGX::SamplerDesc samplerData = {};
			samplerData.minFilter			= LinaGX::Filter::Anisotropic;
			samplerData.magFilter			= LinaGX::Filter::Anisotropic;
			samplerData.mode				= LinaGX::SamplerAddressMode::Repeat;
			samplerData.anisotropy			= 6;
			samplerData.borderColor			= LinaGX::BorderColor::WhiteOpaque;
			samplerData.mipLodBias			= 0.0f;
			samplerData.minLod				= 0.0f;
			samplerData.maxLod				= 30.0f; // upper limit

			TextureSampler* defaultSampler		  = m_resourceManager->CreateResource<TextureSampler>(DEFAULT_SAMPLER_PATH, DEFAULT_SAMPLER_SID);
			TextureSampler* defaultGUISampler	  = m_resourceManager->CreateResource<TextureSampler>(DEFAULT_SAMPLER_GUI_PATH, DEFAULT_SAMPLER_GUI_SID);
			TextureSampler* defaultGUITextSampler = m_resourceManager->CreateResource<TextureSampler>(DEFAULT_SAMPLER_TEXT_PATH, DEFAULT_SAMPLER_TEXT_SID);
			defaultSampler->m_samplerDesc		  = samplerData;
			defaultSampler->GenerateHW();

			samplerData.mipLodBias			 = -1.0f;
			samplerData.mode				 = LinaGX::SamplerAddressMode::ClampToEdge;
			samplerData.magFilter			 = LinaGX::Filter::Linear;
			defaultGUISampler->m_samplerDesc = samplerData;
			defaultGUISampler->GenerateHW();

			samplerData.minFilter				 = LinaGX::Filter::Anisotropic;
			samplerData.magFilter				 = LinaGX::Filter::Anisotropic;
			samplerData.mode					 = LinaGX::SamplerAddressMode::ClampToEdge;
			samplerData.mipLodBias				 = 0.0f;
			defaultGUITextSampler->m_samplerDesc = samplerData;
			defaultGUITextSampler->GenerateHW();

			m_defaultSamplers.push_back(defaultSampler);
			m_defaultSamplers.push_back(defaultGUISampler);
			m_defaultSamplers.push_back(defaultGUITextSampler);
		}*/
	}

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_meshManager.Initialize();

		// pfd
		{
			for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& data = m_pfd[i];

				LinaGX::ResourceDesc globalDataDesc = {
					.size		   = sizeof(GPUDataEngineGlobals),
					.typeHintFlags = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
					.heapType	   = LinaGX::ResourceHeap::StagingHeap,
					.debugName	   = "GfxManager: Global Data Buffer",
				};
				data.globalDataBuffer.Create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataEngineGlobals), "GfxManager: Engine Globals", true);
				data.globalMaterialsBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(uint32) * 1000, "GfxManager: Materials", false);

				data.globalCopyStream	 = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, .commandCount = 200, .totalMemoryLimit = 24000, .auxMemorySize = 8192, .constantBlockSize = 64});
				data.globalCopySemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());

				// Descriptor set 0 - global res
				data.descriptorSetPersistentGlobal = m_lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentGlobal());
				m_lgx->DescriptorUpdateBuffer({
					.setHandle	   = data.descriptorSetPersistentGlobal,
					.binding	   = 0,
					.buffers	   = {data.globalDataBuffer.GetGPUResource()},
					.isWriteAccess = false,
				});

				m_lgx->DescriptorUpdateBuffer({
					.setHandle	   = data.descriptorSetPersistentGlobal,
					.binding	   = 1,
					.buffers	   = {data.globalMaterialsBuffer.GetGPUResource()},
					.isWriteAccess = false,
				});

				data.pipelineLayoutPersistentGlobal = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentGlobal());

				data.globalTexturesDesc = {
					.setHandle = data.descriptorSetPersistentGlobal,
					.binding   = 2,
				};

				data.globalSamplersDesc = {
					.setHandle = data.descriptorSetPersistentGlobal,
					.binding   = 3,
				};

				for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
				{
					data.pipelineLayoutPersistentRenderpass[j] = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(static_cast<RenderPassDescriptorType>(j)));
				}

				data.poolSubmissionSemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
			}
		}

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			UpdateBindlessResources(m_pfd[i]);

		// Default materials
		/*
		{

			Material* defaultObjectMaterial = m_resourceManager->CreateResource<Material>(DEFAULT_MATERIAL_OBJECT_PATH, DEFAULT_MATERIAL_OBJECT_SID);
			defaultObjectMaterial->SetShader(DEFAULT_SHADER_OBJECT_SID);
			defaultObjectMaterial->SetProperty("albedo"_hs, LinaTexture2D{DEFAULT_TEXTURE_CHECKERED_DARK_SID, m_defaultSamplers[0]->GetSID()});
			m_defaultMaterials.push_back(defaultObjectMaterial);

			Material* defaultSkyMaterial = m_resourceManager->CreateResource<Material>(DEFAULT_MATERIAL_SKY_PATH, DEFAULT_MATERIAL_SKY_SID);
			defaultSkyMaterial->SetShader(DEFAULT_SHADER_SKY_SID);
			m_defaultMaterials.push_back(defaultSkyMaterial);

			// Material* defaultUnlitMaterial = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultUnlit.linamaterial", DEFAULT_UNLIT_MATERIAL);
			// Material* defaultLitMaterial   = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultLit.linamaterial", DEFAULT_LIT_MATERIAL);
			// defaultLitMaterial->SetShader("Resources/Core/Shaders/LitStandard.linashader"_hs);
			// defaultUnlitMaterial->SetShader("Resources/Core/Shaders/UnlitStandard.linashader"_hs);
			// m_defaultMaterials.push_back(defaultLitMaterial);
			// m_defaultMaterials.push_back(defaultUnlitMaterial);
		}
		 */
	}

	void GfxManager::PreShutdown()
	{
		m_resourceManager->RemoveListener(this);

		// Preshutdown is before resource manager, make sure
		// to remove user managed resources.
		LinaVG::TerminateText();
		m_guiBackend.Shutdown();

		for (auto m : m_defaultMaterials)
			m_resourceManager->DestroyResource<Material>(m);

		for (auto s : m_defaultSamplers)
			m_resourceManager->DestroyResource<TextureSampler>(s);
	}

	void GfxManager::Shutdown()
	{
		for (const RendererPool& pool : m_rendererPools)
		{
			LINA_ASSERT(pool.renderers.empty(), "");
		}

		DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);

		// Frame resources.
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyDescriptorSet(data.descriptorSetPersistentGlobal);
			m_lgx->DestroyPipelineLayout(data.pipelineLayoutPersistentGlobal);

			data.globalDataBuffer.Destroy();
			data.globalMaterialsBuffer.Destroy();
			m_lgx->DestroyCommandStream(data.globalCopyStream);
			m_lgx->DestroyUserSemaphore(data.globalCopySemaphore.GetSemaphore());

			for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
				m_lgx->DestroyPipelineLayout(data.pipelineLayoutPersistentRenderpass[j]);

			m_lgx->DestroyUserSemaphore(data.poolSubmissionSemaphore.GetSemaphore());
		}

		// Other gfx resources
		m_meshManager.Shutdown();

		// Final
		delete m_lgx;
	}

	void GfxManager::CreateRendererPool(StringID sid, uint32 order, bool submitInBatch)
	{
		m_rendererPools.push_back({.sid = sid, .order = order, .submitInBatch = submitInBatch});
		linatl::sort(m_rendererPools.begin(), m_rendererPools.end(), [](const RendererPool& p1, const RendererPool& p2) -> bool { return p1.order < p2.order; });
	}

	void GfxManager::AddRenderer(Renderer* renderer, StringID sid)
	{
		auto it = linatl::find_if(m_rendererPools.begin(), m_rendererPools.end(), [sid](const RendererPool& pool) -> bool { return pool.sid == sid; });
		LINA_ASSERT(it != m_rendererPools.end(), "");
		it->renderers.push_back(renderer);
	}

	void GfxManager::RemoveRenderer(Renderer* renderer)
	{
		for (RendererPool& pool : m_rendererPools)
		{
			auto it = linatl::find_if(pool.renderers.begin(), pool.renderers.end(), [renderer](Renderer* r) -> bool { return r == renderer; });
			if (it != pool.renderers.end())
			{
				pool.renderers.erase(it);
				return;
			}
		}
		LINA_ASSERT(false, "");
	}

	void GfxManager::WaitForSwapchains()
	{
	}

	void GfxManager::Join()
	{
		m_lgx->Join();
	}

	void GfxManager::Poll()
	{
		m_lgx->TickWindowSystem();
	}

	void GfxManager::PreTick()
	{
		WaitForSwapchains();

		for (const RendererPool& pool : m_rendererPools)
		{
			for (Renderer* rend : pool.renderers)
				rend->PreTick();
		}
	}

	void GfxManager::Tick(float delta)
	{
		PROFILER_FUNCTION();

		if (m_mainWindow == nullptr)
			m_mainWindow = m_lgx->GetWindowManager().GetWindow(LINA_MAIN_SWAPCHAIN);

		for (const RendererPool& pool : m_rendererPools)
		{
			for (auto* r : pool.renderers)
				r->Tick(delta);

			// if (pool.renderers.size() == 1)
			// {
			// 	auto* rend = pool.renderers[0];
			// 	rend->Tick(delta);
			// }
			// else
			// {
			// 	Taskflow tf;
			// 	tf.for_each_index(0, static_cast<int>(pool.renderers.size()), 1, [&](int i) {
			// 		auto* rend = pool.renderers.at(i);
			// 		rend->Tick(delta);
			// 	});
			// 	m_system->GetMainExecutor()->RunAndWait(tf);
			// }
		}
	}

	void GfxManager::UpdateBindlessResources(PerFrameData& pfd)
	{
		CONDITIONAL_LOCK(m_resourceManagerLocked, m_resourceManager->GetLock());
		// Textures.
		ResourceCache<Texture>* cacheTxt = m_resourceManager->GetCache<Texture>();
		pfd.globalTexturesDesc.textures.resize(static_cast<size_t>(cacheTxt->GetActiveItemCount()));
		cacheTxt->View([&](Texture* txt, uint32 index) -> bool {
			pfd.globalTexturesDesc.textures[index] = txt->GetGPUHandle();
			txt->SetBindlessIndex(static_cast<uint32>(index));
			return false;
		});

		if (!pfd.globalTexturesDesc.textures.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalTexturesDesc);

		// Samplers
		ResourceCache<TextureSampler>* cacheSmp = m_resourceManager->GetCache<TextureSampler>();
		pfd.globalSamplersDesc.samplers.resize(static_cast<size_t>(cacheSmp->GetActiveItemCount()));
		cacheSmp->View([&](TextureSampler* smp, uint32 index) -> bool {
			smp->SetBindlessIndex(static_cast<uint32>(index));
			return false;
		});

		if (!pfd.globalSamplersDesc.samplers.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalSamplersDesc);

		// Materials
		ResourceCache<Material>* cacheMat = m_resourceManager->GetCache<Material>();
		size_t					 padding  = 0;
		cacheMat->View([&](Material* mat, uint32 index) -> bool {
			padding += mat->BufferDataInto(pfd.globalMaterialsBuffer, padding);
			return false;
		});
	}

	void GfxManager::Render(StringID targetPool)
	{
		PROFILER_FUNCTION();
		const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
		auto&		 currentFrame	   = m_pfd[currentFrameIndex];

		m_lgx->StartFrame();

		m_appDelegate->Render(currentFrameIndex);

		/*
				if (m_mainWindow == nullptr)
					m_mainWindow = m_lgx->GetWindowManager().GetWindow(LINA_MAIN_SWAPCHAIN);

				// Update global data.
				{
					const auto&			 mp			= m_lgx->GetInput().GetMousePositionAbs();
					const auto&			 windowSize = m_mainWindow->GetSize();
					GPUDataEngineGlobals globalData = {};
					globalData.mouseScreen			= Vector4(static_cast<float>(mp.x), static_cast<float>(mp.y), static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
					globalData.deltaElapsed			= Vector4(SystemInfo::GetDeltaTimeF(), SystemInfo::GetAppTimeF(), 0.0f, 0.0f);
					currentFrame.globalDataBuffer.BufferData(0, (uint8*)&globalData, sizeof(GPUDataEngineGlobals));
				}

				// Bindless resources
				{
					UpdateBindlessResources(currentFrame);
				}

				// Update dirty materials.
				{
					m_resourceUploadQueue.AddBufferRequest(&currentFrame.globalDataBuffer);
					m_resourceUploadQueue.AddBufferRequest(&currentFrame.globalMaterialsBuffer);
				}

				// Mesh manager refresh
				{
					m_meshManager.Refresh();
				}

				// Wait for all transfers.
				{

					if (m_resourceUploadQueue.FlushAll(currentFrame.globalCopyStream))
					{
						m_lgx->CloseCommandStreams(&currentFrame.globalCopyStream, 1);
						currentFrame.globalCopySemaphore.Increment();
						LinaGX::SubmitDesc desc = LinaGX::SubmitDesc{
							.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
							.streams		  = &currentFrame.globalCopyStream,
							.streamCount	  = 1,
							.useSignal		  = true,
							.signalCount	  = 1,
							.signalSemaphores = currentFrame.globalCopySemaphore.GetSemaphorePtr(),
							.signalValues	  = currentFrame.globalCopySemaphore.GetValuePtr(),
							.isMultithreaded  = true,
						};
						m_lgx->SubmitCommandStreams(desc);
						m_lgx->WaitForUserSemaphore(currentFrame.globalCopySemaphore.GetSemaphore(), currentFrame.globalCopySemaphore.GetValue());
					}
				}

				// Renderer work
				Vector<uint16> waitSemaphores;
				Vector<uint64> waitValues;

				for (const RendererPool& pool : m_rendererPools)
				{
					if (targetPool != 0 && pool.sid != targetPool)
						continue;

					if (pool.renderers.empty())
						continue;

					Vector<Renderer*> validRenderers;

					for (auto* rend : pool.renderers)
					{
						if (rend->IsValidThisFrame())
							validRenderers.push_back(rend);
					}

					Vector<LinaGX::CommandStream*> streams;
					streams.resize(validRenderers.size());

					if (validRenderers.size() == 1)
					{
						auto* rend = validRenderers[0];
						rend->Render(currentFrameIndex, static_cast<uint32>(waitSemaphores.size()), waitSemaphores.data(), waitValues.data());
						streams[0] = rend->GetStreamForBatchSubmit(currentFrameIndex);
					}
					else
					{
						Taskflow tf;
						tf.for_each_index(0, static_cast<int>(validRenderers.size()), 1, [&](int i) {
							auto* rend = validRenderers.at(i);
							rend->Render(currentFrameIndex, static_cast<uint32>(waitSemaphores.size()), waitSemaphores.data(), waitValues.data());
							streams[i] = rend->GetStreamForBatchSubmit(currentFrameIndex);
						});
						m_system->GetMainExecutor()->RunAndWait(tf);
					}

					if (pool.submitInBatch)
					{

						for (Renderer* rend : pool.renderers)
						{
							const SemaphoreData waitSemaphore = rend->GetWaitSemaphore(currentFrameIndex);

							if (waitSemaphore.IsModified())
							{
								waitSemaphores.push_back(waitSemaphore.GetSemaphore());
								waitValues.push_back(waitSemaphore.GetValue());
							}
						}

						currentFrame.poolSubmissionSemaphore.Increment();

						m_lgx->SubmitCommandStreams({
							.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
							.streams		  = streams.data(),
							.streamCount	  = static_cast<uint32>(streams.size()),
							.useWait		  = !waitValues.empty(),
							.waitCount		  = static_cast<uint32>(waitValues.size()),
							.waitSemaphores	  = waitSemaphores.data(),
							.waitValues		  = waitValues.data(),
							.useSignal		  = true,
							.signalSemaphores = currentFrame.poolSubmissionSemaphore.GetSemaphorePtr(),
							.signalValues	  = currentFrame.poolSubmissionSemaphore.GetValuePtr(),
							.isMultithreaded  = true,
						});

						waitValues.clear();
						waitSemaphores.clear();
						waitSemaphores.push_back(currentFrame.poolSubmissionSemaphore.GetSemaphore());
						waitValues.push_back(currentFrame.poolSubmissionSemaphore.GetValue());
					}
					else
					{
						waitValues.clear();
						waitSemaphores.clear();

						for (Renderer* rend : pool.renderers)
						{
							SemaphoreData submittedSemaphore = rend->GetSubmitSemaphore(currentFrameIndex);

							if (submittedSemaphore.IsModified())
							{
								waitSemaphores.push_back(submittedSemaphore.GetSemaphore());
								waitValues.push_back(submittedSemaphore.GetValue());
							}
						}
					}
				}

				// Presentation
				Vector<uint8> swapchains;

				for (const RendererPool& pool : m_rendererPools)
				{
					for (Renderer* rend : pool.renderers)
					{
						if (!rend->IsValidThisFrame())
							continue;

						uint8 outSwapchain = 0;
						if (rend->GetSwapchainToPresent(outSwapchain))
							swapchains.push_back(outSwapchain);
					}
				}

				if (!swapchains.empty())
				{
					m_lgx->Present({
						.swapchains		= swapchains.data(),
						.swapchainCount = static_cast<uint32>(swapchains.size()),
					});
				}
		*/
		m_lgx->EndFrame();
	}

	LinaGX::Window* GfxManager::CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow)
	{
		m_lgx->Join();
		auto window = m_lgx->GetWindowManager().CreateApplicationWindow(sid, title, pos.x, pos.y, size.x, size.y, static_cast<LinaGX::WindowStyle>(style), parentWindow);
		window->AddListener(this);

		return window;
	}

	void GfxManager::DestroyApplicationWindow(StringID sid)
	{
		m_lgx->Join();
		auto* window = m_lgx->GetWindowManager().GetWindow(sid);
		window->RemoveListener(this);
		m_lgx->GetWindowManager().DestroyApplicationWindow(sid);
	}

	void GfxManager::OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& newSize)
	{
		Join();

		for (const RendererPool& pool : m_rendererPools)
		{
			for (Renderer* rend : pool.renderers)
			{
				rend->OnWindowSizeChanged(window, newSize);
			}
		}
	}

	LinaGX::Window* GfxManager::GetApplicationWindow(StringID sid)
	{
		return m_lgx->GetWindowManager().GetWindow(sid);
	}

	void GfxManager::OnManagerLock(uint32 lockCount)
	{
		m_resourceManagerLocked = true;
	}

	void GfxManager::OnManagerUnlock(uint32 lockCount)
	{
		if (lockCount == 0)
			m_resourceManagerLocked = false;
	}
} // namespace Lina
