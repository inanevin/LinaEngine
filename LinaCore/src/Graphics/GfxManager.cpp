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
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
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
		LinaVG::Config.maxFontAtlasSize		  = 512;
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
		}

		// Other gfx resources
		m_resourceUploadQueue.Shutdown();
		m_meshManager.Shutdown();
		for (auto sr : m_surfaceRenderers)
			delete sr;
		m_surfaceRenderers.clear();

		// Final
		delete m_lgx;

		// GUI allocators
		linatl::for_each(m_guiAllocators.begin(), m_guiAllocators.end(), [](auto& pair) -> void { delete pair.second; });
		m_guiAllocators.clear();
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

		for (auto* s : m_surfaceRenderers)
			s->PreTick();
	}

	void GfxManager::Tick(float delta)
	{
		PROFILER_FUNCTION();

		if (m_mainWindow == nullptr)
			m_mainWindow = m_lgx->GetWindowManager().GetWindow(LINA_MAIN_SWAPCHAIN);

		bool mt = false;

		if (m_worldRenderers.size() == 1)
			m_worldRenderers[0]->Tick(delta);
		else
		{
			Taskflow tf;
			tf.for_each(m_worldRenderers.begin(), m_worldRenderers.end(), [delta](WorldRenderer* wr) { wr->Tick(delta); });
			m_system->GetMainExecutor()->RunMove(tf);
			mt = true;
		}

		if (m_surfaceRenderers.size() == 1)
			m_surfaceRenderers[0]->Tick(delta);
		else
		{
			Taskflow tf;
			tf.for_each(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [delta](SurfaceRenderer* sf) { sf->Tick(delta); });
			m_system->GetMainExecutor()->RunMove(tf);
			mt = true;
		}

		if (mt)
			m_system->GetMainExecutor()->Wait();
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
		/*
			1. Transfer resources
			2. Render active worlds, they wait on transfer semaphores (and their own) if exist.
			3. Render surface renderers, they wait on world semaphores (and their own).
			4. Present surface renderers.
		 */
		PROFILER_FUNCTION();

		const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
		auto&		 currentFrame	   = m_pfd[currentFrameIndex];

		// Determine eligible surface renderers.
		Vector<uint8>			 swapchains;
		Vector<SurfaceRenderer*> validSurfaceRenderers;
		Vector<uint64>			 storedSurfaceRendererSemaphoreValues;
		for (const auto& sr : m_surfaceRenderers)
		{
			if (sr->IsVisible())
			{
				validSurfaceRenderers.push_back(sr);
				swapchains.push_back(sr->GetSwapchain());
				storedSurfaceRendererSemaphoreValues.push_back(sr->GetCopySemaphoreData(currentFrameIndex).value);
			}
			m_lgx->SetSwapchainActive(sr->GetSwapchain(), sr->IsVisible());
		}
		if (validSurfaceRenderers.empty())
			return;

		// Frame init
		m_lgx->StartFrame();

		// Update engine global data.
		const auto&			 mp			= m_lgx->GetInput().GetMousePositionAbs();
		const auto&			 windowSize = m_mainWindow->GetSize();
		GPUDataEngineGlobals globalData = {};
		globalData.mouseScreen			= Vector4(static_cast<float>(mp.x), static_cast<float>(mp.y), static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
		globalData.deltaElapsed			= Vector4(SystemInfo::GetDeltaTimeF(), SystemInfo::GetAppTimeF(), 0.0f, 0.0f);
		currentFrame.globalDataBuffer.BufferData(0, (uint8*)&globalData, sizeof(GPUDataEngineGlobals));

		// Update dirty materials.
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

		// Update descriptors
		UpdateBindlessResources(currentFrame);
		m_resourceUploadQueue.AddBufferRequest(&currentFrame.globalMaterialsBuffer);

		// Transfer data
		const bool			transferExists		  = m_resourceUploadQueue.FlushAll(currentFrameIndex);
		const SemaphoreData transferSemaphoreData = m_resourceUploadQueue.GetSemaphoreData(currentFrameIndex);

		// Render worlds.
		Vector<SemaphoreData> waitOnWorldRenderers;
		waitOnWorldRenderers.resize(m_worldRenderers.size());
		if (m_worldRenderers.size() == 1)
		{
			waitOnWorldRenderers[0] = m_worldRenderers[0]->Render(currentFrameIndex, transferExists ? transferSemaphoreData : SemaphoreData{});
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) { waitOnWorldRenderers[i] = m_worldRenderers[i]->Render(currentFrameIndex, transferExists ? transferSemaphoreData : SemaphoreData{}); });
			m_system->GetMainExecutor()->RunAndWait(tf);
		}

		// Render surfaces.
		Vector<LinaGX::CommandStream*> surfaceRendererStreams(validSurfaceRenderers.size());
		if (validSurfaceRenderers.size() == 1)
		{
			auto sf					  = validSurfaceRenderers[0];
			surfaceRendererStreams[0] = sf->Render(currentFrameIndex);
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(validSurfaceRenderers.size()), 1, [&](int i) { surfaceRendererStreams[i] = validSurfaceRenderers[i]->Render(currentFrameIndex); });
			m_system->GetMainExecutor()->RunAndWait(tf);
		}

		// Waits for surface renderer submission.
		Vector<uint16> surfaceWaitSemaphores;
		Vector<uint64> surfaceWaitValues;

		// We wait for all worlds to be completed first...
		if (!waitOnWorldRenderers.empty())
		{
			for (const auto& semaphoreData : waitOnWorldRenderers)
			{
				surfaceWaitValues.push_back(semaphoreData.value);
				surfaceWaitSemaphores.push_back(semaphoreData.semaphore);
			}
		}

		// We wait for any copy semaphores used during surface renderer recording (e.g. gui renderers)
		for (size_t i = 0; i < storedSurfaceRendererSemaphoreValues.size(); i++)
		{
			const SemaphoreData& sem = validSurfaceRenderers[i]->GetCopySemaphoreData(currentFrameIndex);
			if (storedSurfaceRendererSemaphoreValues[i] == sem.value)
				continue;

			// Surface renderer's copy semaphore was modified, we need to wait for it's completion.
			surfaceWaitSemaphores.push_back(sem.semaphore);
			surfaceWaitValues.push_back(sem.value); // We submit a value and increment always.
		}

		// If no world renderers, transfers might have not been consumed, so make sure we wait for it.
		if (transferExists && waitOnWorldRenderers.empty())
		{
			surfaceWaitSemaphores.push_back(transferSemaphoreData.semaphore);
			surfaceWaitValues.push_back(transferSemaphoreData.value);
		}

		// Submit surface contents.
		m_lgx->SubmitCommandStreams({
			.targetQueue	 = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams		 = surfaceRendererStreams.data(),
			.streamCount	 = static_cast<uint32>(surfaceRendererStreams.size()),
			.useWait		 = !surfaceWaitSemaphores.empty(),
			.waitCount		 = static_cast<uint32>(surfaceWaitSemaphores.size()),
			.waitSemaphores	 = surfaceWaitSemaphores.data(),
			.waitValues		 = surfaceWaitValues.data(),
			.isMultithreaded = true,
		});

		// Present surface renderers.
		m_lgx->Present({
			.swapchains		= swapchains.data(),
			.swapchainCount = static_cast<uint32>(swapchains.size()),
		});

		// End
		m_lgx->EndFrame();
	}

	LinaGX::Window* GfxManager::CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow)
	{
		m_lgx->Join();
		auto window = m_lgx->GetWindowManager().CreateApplicationWindow(sid, title, pos.x, pos.y, size.x, size.y, static_cast<LinaGX::WindowStyle>(style), parentWindow);

		window->AddListener(this);
		SurfaceRenderer* renderer = new SurfaceRenderer(this, window, sid, size, m_clearColor);
		m_surfaceRenderers.push_back(renderer);

		return window;
	}

	void GfxManager::DestroyApplicationWindow(StringID sid)
	{
		m_lgx->Join();

		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		delete *it;
		m_surfaceRenderers.erase(it);

		auto* window = m_lgx->GetWindowManager().GetWindow(sid);
		window->RemoveListener(this);
		m_lgx->GetWindowManager().DestroyApplicationWindow(sid);
	}

	void GfxManager::OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& newSize)
	{
		Join();

		for (auto* wr : m_worldRenderers)
		{
			if (wr->GetSize().x > newSize.x || wr->GetSize().y > newSize.y)
			{
				const Vector2ui size = wr->GetSize().Min(newSize);
				wr->Resize(size);
			}
		}

		for (auto* sr : m_surfaceRenderers)
		{
			if (sr->GetWindow() == window)
			{
				sr->Resize(newSize);
				break;
			}
		}
	}

	WorldRenderer* GfxManager::CreateWorldRenderer(EntityWorld* world, const Vector2ui& size)
	{
		LOCK_GUARD(m_wrMtx);
		WorldRenderer* wr = new WorldRenderer(this, world, size);
		m_worldRenderers.push_back(wr);
		MarkBindlessDirty();
		return wr;
	}

	void GfxManager::DestroyWorldRenderer(WorldRenderer* renderer)
	{
		LOCK_GUARD(m_wrMtx);
		m_worldRenderers.erase(linatl::find_if(m_worldRenderers.begin(), m_worldRenderers.end(), [renderer](WorldRenderer* rnd) -> bool { return renderer == rnd; }));
		MarkBindlessDirty();
		delete renderer;
	}

	WorldRenderer* GfxManager::GetWorldRenderer(EntityWorld* world)
	{
		auto it = linatl::find_if(m_worldRenderers.begin(), m_worldRenderers.end(), [world](WorldRenderer* rnd) -> bool { return rnd->GetWorld() == world; });
		return *it;
	}

	void GfxManager::DestroyWorldRenderer(EntityWorld* world)
	{
		LOCK_GUARD(m_wrMtx);
		auto		   it		= linatl::find_if(m_worldRenderers.begin(), m_worldRenderers.end(), [world](WorldRenderer* rnd) -> bool { return rnd->GetWorld() == world; });
		WorldRenderer* renderer = *it;
		m_worldRenderers.erase(it);
		delete renderer;
	}

	LinaGX::Window* GfxManager::GetApplicationWindow(StringID sid)
	{
		return m_lgx->GetWindowManager().GetWindow(sid);
	}

	SurfaceRenderer* GfxManager::GetSurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) -> bool { return renderer->GetSID() == sid; });
		return *it;
	}

	PoolAllocator* GfxManager::GetGUIAllocator(TypeID tid, size_t typeSize)
	{
		LINA_ASSERT(SystemInfo::IsMainThread(), "");

		PoolAllocator*& alloc = m_guiAllocators[tid];
		if (alloc == nullptr)
		{
			alloc = new PoolAllocator(typeSize * Theme::GetWidgetChunkCount(tid), typeSize);
			alloc->Init();
		}

		return alloc;
	}

} // namespace Lina
