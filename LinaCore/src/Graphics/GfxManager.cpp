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
	GfxManager::GfxManager(System* sys) : Subsystem(sys, SubsystemType::GfxManager), m_meshManager(this), m_resourceUploadQueue(this)
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

		LinaVG::Text::GetCallbacks().fontTextureBind	   = std::bind(&GUIBackend::BindFontTexture, &m_guiBackend, std::placeholders::_1);
		LinaVG::Text::GetCallbacks().fontTextureBufferData = std::bind(&GUIBackend::BufferFontTextureAtlas, &m_guiBackend, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
		LinaVG::Text::GetCallbacks().fontTextureBufferEnd  = std::bind(&GUIBackend::BufferEnded, &m_guiBackend);
		LinaVG::Text::GetCallbacks().fontTextureCreate	   = std::bind(&GUIBackend::CreateFontTexture, &m_guiBackend, std::placeholders::_1, std::placeholders::_2);
		LinaVG::Text::Initialize();
	}

	void GfxManager::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		// Setup LinaGX
		m_lgx		   = new LinaGX::Instance();
		m_appDelegate  = m_system->GetApp()->GetAppDelegate();
		m_clearColor   = initInfo.clearColor;
		m_currentVsync = initInfo.vsyncStyle;

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

			TextureSampler* defaultSampler		  = m_resourceManager->CreateUserResource<TextureSampler>(DEFAULT_SAMPLER_PATH, DEFAULT_SAMPLER_SID);
			TextureSampler* defaultGUISampler	  = m_resourceManager->CreateUserResource<TextureSampler>(DEFAULT_SAMPLER_GUI_PATH, DEFAULT_SAMPLER_GUI_SID);
			TextureSampler* defaultGUITextSampler = m_resourceManager->CreateUserResource<TextureSampler>(DEFAULT_SAMPLER_TEXT_PATH, DEFAULT_SAMPLER_TEXT_SID);
			defaultSampler->m_samplerDesc		  = samplerData;

			samplerData.mipLodBias			 = -1.0f;
			samplerData.mode				 = LinaGX::SamplerAddressMode::ClampToEdge;
			samplerData.magFilter			 = LinaGX::Filter::Linear;
			defaultGUISampler->m_samplerDesc = samplerData;

			samplerData.minFilter				 = LinaGX::Filter::Anisotropic;
			samplerData.magFilter				 = LinaGX::Filter::Anisotropic;
			samplerData.mode					 = LinaGX::SamplerAddressMode::ClampToEdge;
			samplerData.mipLodBias				 = 0.0f;
			defaultGUITextSampler->m_samplerDesc = samplerData;

			// Force creation.
			defaultSampler->BatchLoaded();
			defaultGUISampler->BatchLoaded();
			defaultGUITextSampler->BatchLoaded();

			m_defaultSamplers.push_back(defaultSampler);
			m_defaultSamplers.push_back(defaultGUISampler);
			m_defaultSamplers.push_back(defaultGUITextSampler);
		}
	}

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{

		m_resourceUploadQueue.Initialize();
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
				data.globalDataBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataEngineGlobals), "GfxManager: Engine Globals", true);
				data.globalMaterialsBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(uint32) * 1000, "GfxManager: Materials", false);

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

				for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
				{
					data.pipelineLayoutPersistentRenderpass[j] = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(static_cast<RenderPassDescriptorType>(j)));
				}

				data.poolSubmissionSemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
			}
		}

		MarkBindlessDirty();
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			UpdateBindlessResources(m_pfd[i]);

		// Default materials
		{

			Material* defaultObjectMaterial = m_resourceManager->CreateUserResource<Material>(DEFAULT_MATERIAL_OBJECT_PATH, DEFAULT_MATERIAL_OBJECT_SID);
			defaultObjectMaterial->SetShader(DEFAULT_SHADER_OBJECT_SID);
			defaultObjectMaterial->BatchLoaded();
			defaultObjectMaterial->SetProperty("albedo"_hs, LinaTexture2D{"Resources/Core/Textures/CheckeredDark.png"_hs, m_defaultSamplers[0]->GetSID()});
			m_defaultMaterials.push_back(defaultObjectMaterial);

			Material* defaultSkyMaterial = m_resourceManager->CreateUserResource<Material>(DEFAULT_MATERIAL_SKY_PATH, DEFAULT_MATERIAL_SKY_SID);
			defaultSkyMaterial->SetShader(DEFAULT_SHADER_SKY_SID);
			defaultSkyMaterial->BatchLoaded();
			m_defaultMaterials.push_back(defaultSkyMaterial);

			// Material* defaultUnlitMaterial = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultUnlit.linamaterial", DEFAULT_UNLIT_MATERIAL);
			// Material* defaultLitMaterial   = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultLit.linamaterial", DEFAULT_LIT_MATERIAL);
			// defaultLitMaterial->SetShader("Resources/Core/Shaders/LitStandard.linashader"_hs);
			// defaultUnlitMaterial->SetShader("Resources/Core/Shaders/UnlitStandard.linashader"_hs);
			// m_defaultMaterials.push_back(defaultLitMaterial);
			// m_defaultMaterials.push_back(defaultUnlitMaterial);
		}
	}

	void GfxManager::PreShutdown()
	{
		// Preshutdown is before resource manager, make sure
		// to remove user managed resources.
		LinaVG::Text::Terminate();
		m_guiBackend.Shutdown();

		for (auto m : m_defaultMaterials)
			m_resourceManager->DestroyUserResource<Material>(m);

		for (auto s : m_defaultSamplers)
			m_resourceManager->DestroyUserResource<TextureSampler>(s);
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

			for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
				m_lgx->DestroyPipelineLayout(data.pipelineLayoutPersistentRenderpass[j]);

			m_lgx->DestroyUserSemaphore(data.poolSubmissionSemaphore.GetSemaphore());
		}

		// Other gfx resources
		m_resourceUploadQueue.Shutdown();
		m_meshManager.Shutdown();

		// Final
		delete m_lgx;
	}

	void GfxManager::CreateRendererPool(StringID sid, uint32 order, bool submitInBatch)
	{
		m_rendererPools.push_back({.sid = sid, .order = order, .submitInBatch = submitInBatch});
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
			for (auto* rend : pool.renderers)
			{
				rend->Tick(delta);
			}
			// if(pool.renderers.size() == 1)
			// {
			//     auto* rend = pool.renderers[0];
			//     rend->Tick(delta);
			// }
			// else {
			//     Taskflow tf;
			//     tf.for_each_index(0, static_cast<int>(pool.renderers.size()), 1, [&](int i) {
			//         auto* rend = pool.renderers.at(i);
			//         rend->Tick(delta);
			//     });
			//     m_system->GetMainExecutor()->RunAndWait(tf);
			// }
		}
	}

	void GfxManager::UpdateBindlessResources(PerFrameData& pfd)
	{
		if (!pfd.bindlessDirty.load())
			return;

		LinaGX::DescriptorUpdateImageDesc imgUpdate = {
			.setHandle = pfd.descriptorSetPersistentGlobal,
			.binding   = 2,
		};

		LinaGX::DescriptorUpdateImageDesc smpUpdate = {
			.setHandle = pfd.descriptorSetPersistentGlobal,
			.binding   = 3,
		};

		Vector<Texture*>		textures;
		Vector<TextureSampler*> samplers;
		Vector<Material*>		materials;
		m_resourceManager->GetAllResourcesRaw<Texture>(textures, true);
		m_resourceManager->GetAllResourcesRaw<TextureSampler>(samplers, true);
		m_resourceManager->GetAllResourcesRaw<Material>(materials, true);
		imgUpdate.textures.resize(textures.size());
		smpUpdate.samplers.resize(samplers.size());

		for (size_t i = 0; i < textures.size(); i++)
		{
			Texture* txt		  = textures[i];
			imgUpdate.textures[i] = txt->GetGPUHandle();
			txt->m_bindlessIndex  = static_cast<uint32>(i);
		}

		for (size_t i = 0; i < samplers.size(); i++)
		{
			TextureSampler* smp	  = samplers[i];
			smpUpdate.samplers[i] = smp->GetGPUHandle();
			smp->m_bindlessIndex  = static_cast<uint32>(i);
		}

		size_t padding = 0;
		for (auto* mat : materials)
			padding += mat->BufferDataInto(pfd.globalMaterialsBuffer, padding);

		m_lgx->DescriptorUpdateImage(imgUpdate);
		m_lgx->DescriptorUpdateImage(smpUpdate);
		pfd.bindlessDirty.store(false);
	}

	void GfxManager::Render()
	{
		PROFILER_FUNCTION();
		const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
		auto&		 currentFrame	   = m_pfd[currentFrameIndex];

		m_lgx->StartFrame();

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

		// Update dirty materials.
		{
			Vector<Material*> materials;
			m_resourceManager->GetAllResourcesRaw<Material>(materials, true);
			for (auto* mat : materials)
			{
				if (mat->m_propsDirty)
				{
					mat->BufferDataInto(currentFrame.globalMaterialsBuffer, mat->m_bindlessBytePadding);
					mat->m_propsDirty = false;
				}
			}

			m_resourceUploadQueue.AddBufferRequest(&currentFrame.globalMaterialsBuffer);
		}

		// Bindless resources
		{
			UpdateBindlessResources(currentFrame);
		}

		// Mesh manager refresh
		{
			m_meshManager.Refresh();
		}

		// Wait for all transfers.
		{
			bool transferExists = m_resourceUploadQueue.FlushAll(currentFrameIndex);
			if (transferExists)
			{
				const SemaphoreData& semaphoreData = m_resourceUploadQueue.GetSemaphoreData(currentFrameIndex);
				;
				m_lgx->WaitForUserSemaphore(semaphoreData.GetSemaphore(), semaphoreData.GetValue());
			}
		}

		// Renderer work
		Vector<uint16> waitSemaphores;
		Vector<uint64> waitValues;

		for (const RendererPool& pool : m_rendererPools)
		{
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

} // namespace Lina
