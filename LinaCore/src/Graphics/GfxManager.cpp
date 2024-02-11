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
#include "Common/GUI/WidgetAllocator.hpp"

#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
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
#include "Core/SystemInfo.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"

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
		m_system->AddListener(this);

		// Setup LinaVG
		m_guiBackend						  = new GUIBackend(this);
		LinaVG::Config.globalFramebufferScale = 1.0f;
		LinaVG::Config.globalAAMultiplier	  = 1.0f;
		LinaVG::Config.gcCollectInterval	  = 4000;
		LinaVG::Config.textCachingEnabled	  = true;
		LinaVG::Config.textCachingSDFEnabled  = true;
		LinaVG::Config.textCacheReserve		  = 10000;
		LinaVG::Config.maxFontAtlasSize		  = 2048;
		LinaVG::Config.errorCallback		  = [](const std::string& err) { LINA_ERR(err.c_str()); };
		LinaVG::Config.logCallback			  = [](const std::string& log) { LINA_TRACE(log.c_str()); };
		LinaVG::Backend::BaseBackend::SetBackend(m_guiBackend);
		LinaVG::Initialize();
	}

	void GfxManager::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		// Setup LinaGX
		m_lgx		  = new LinaGX::Instance();
		m_appDelegate = m_system->GetApp()->GetAppDelegate();

		LinaGX::Config.dx12Config = {
			.allowTearing = initInfo.allowTearing,
		};

		LinaGX::Config.vulkanConfig = {};

		LinaGX::Config.logLevel		 = LinaGX::LogLevel::Verbose;
		LinaGX::Config.errorCallback = LinaGX_ErrorCallback;
		LinaGX::Config.infoCallback	 = LinaGX_LogCallback;

		LinaGX::BackendAPI api = LinaGX::BackendAPI::Vulkan;

#ifdef LINA_PLATFORM_APPLE
		api = LinaGX::BackendAPI::Metal;
#endif

		LinaGX::Config.api			   = api;
		LinaGX::Config.gpu			   = LinaGX::PreferredGPUType::Discrete;
		LinaGX::Config.framesInFlight  = FRAMES_IN_FLIGHT;
		LinaGX::Config.backbufferCount = BACK_BUFFER_COUNT;
		LinaGX::Config.gpuLimits	   = {};

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

			TextureSampler* defaultSampler		  = m_resourceManager->CreateUserResource<TextureSampler>("Resources/Core/Samplers/DefaultSampler.linasampler", DEFAULT_SAMPLER_SID);
			TextureSampler* defaultGUISampler	  = m_resourceManager->CreateUserResource<TextureSampler>("Resources/Core/Samplers/DefaultGUISampler.linasampler", DEFAULT_GUI_SAMPLER_SID);
			TextureSampler* defaultGUITextSampler = m_resourceManager->CreateUserResource<TextureSampler>("Resources/Core/Samplers/DefaultGUITextSampler.linasampler", DEFAULT_GUI_TEXT_SAMPLER_SID);
			defaultSampler->m_samplerDesc		  = samplerData;

			samplerData.mipLodBias			 = -1.0f;
			defaultGUISampler->m_samplerDesc = samplerData;

			samplerData.minFilter				 = LinaGX::Filter::Nearest;
			samplerData.magFilter				 = LinaGX::Filter::Nearest;
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

		// Default materials
		{
			// Material* defaultUnlitMaterial = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultUnlit.linamaterial", DEFAULT_UNLIT_MATERIAL);
			// Material* defaultLitMaterial   = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultLit.linamaterial", DEFAULT_LIT_MATERIAL);
			// defaultLitMaterial->SetShader("Resources/Core/Shaders/LitStandard.linashader"_hs);
			// defaultUnlitMaterial->SetShader("Resources/Core/Shaders/UnlitStandard.linashader"_hs);
			// m_defaultMaterials.push_back(defaultLitMaterial);
			// m_defaultMaterials.push_back(defaultUnlitMaterial);
		}
	}

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_resourceUploadQueue.Initialize();
		m_meshManager.Initialize();
		m_currentVsync = initInfo.vsyncStyle;

		// LinaGX
		{
			m_lgx->GetInput().SetCallbackKey([&](uint32 key, int32 scanCode, LinaGX::InputAction action, LinaGX::Window* window) {
				Event ev;
				ev.iParams[0] = key;
				ev.iParams[1] = static_cast<uint32>(action);
				ev.pParams[0] = window;
				m_system->DispatchEvent(EVS_OnKey, ev);
			});
		}

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
				data.globalDataResource = m_lgx->CreateResource(globalDataDesc);
				m_lgx->MapResource(data.globalDataResource, data.globalDataMapped);

				// Descriptor set 0 - global res
				data.descriptorSetPersistentGlobal = m_lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentGlobal());
				m_lgx->DescriptorUpdateBuffer({
					.setHandle	   = data.descriptorSetPersistentGlobal,
					.binding	   = 0,
					.buffers	   = {data.globalDataResource},
					.isWriteAccess = false,
				});

				data.pipelineLayoutPersistentGlobal = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentGlobal());

				for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
				{
					data.pipelineLayoutPersistentRenderpass[j] = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(static_cast<RenderPassDescriptorType>(j)));
				}
			}
		}
	}

	void GfxManager::PreShutdown()
	{
		// Preshutdown is before resource manager, make sure
		// to remove user managed resources.

		LinaVG::Terminate();

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
			m_lgx->DestroyResource(data.globalDataResource);
			m_lgx->DestroyPipelineLayout(data.pipelineLayoutPersistentGlobal);

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
		m_system->RemoveListener(this);
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

	void GfxManager::Tick()
	{
		PROFILER_FUNCTION();

		const uint32		 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
		auto&				 currentFrame	   = m_pfd[currentFrameIndex];
		const auto&			 mp				   = m_lgx->GetInput().GetMousePositionAbs();
		GPUDataEngineGlobals globalData		   = {
				   .mousePosition = Vector2(static_cast<float>(mp.x), static_cast<float>(mp.y)),
				   .deltaTime	  = SystemInfo::GetDeltaTimeF(),
				   .elapsedTime	  = SystemInfo::GetAppTimeF(),
		   };
		MEMCPY(currentFrame.globalDataMapped, &globalData, sizeof(GPUDataEngineGlobals));

		for (const auto& sr : m_surfaceRenderers)
			sr->Tick();
	}

	void GfxManager::RenderSync()
	{
		PROFILER_FUNCTION();
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

		// Transfer.
		SemaphoreData transferSemaphoreData = {};
		if (m_resourceUploadQueue.FlushAll(transferSemaphoreData))
		{
		}

		// Frame init
		m_lgx->StartFrame();

		// WORLD RENDER

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

		// Start LinaVG for surface renderers.
		WidgetAllocator::Get().StartFrame(static_cast<int>(validSurfaceRenderers.size()));
		LinaVG::StartFrame(static_cast<int>(validSurfaceRenderers.size()));

		// Record surface renderers.
		Vector<LinaGX::CommandStream*> surfaceRendererStreams(validSurfaceRenderers.size());
		if (validSurfaceRenderers.size() == 1)
		{
			auto sf					  = validSurfaceRenderers[0];
			surfaceRendererStreams[0] = sf->Render(currentFrameIndex, 0);
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(validSurfaceRenderers.size()), 1, [&](int i) { surfaceRendererStreams[i] = validSurfaceRenderers[i]->Render(currentFrameIndex, i); });
			m_system->GetMainExecutor()->RunAndWait(tf);
		}

		// Clean up all surface renderer related buffers in LinaVG.
		LinaVG::EndFrame();
		WidgetAllocator::Get().EndFrame();

		// Waits for surface renderer submission.
		Vector<uint16> waitSemaphores;
		Vector<uint64> waitValues;

		// TODO: check waits from world renderers.

		// Check waits from surface renderers themselves.
		for (size_t i = 0; i < storedSurfaceRendererSemaphoreValues.size(); i++)
		{
			const SemaphoreData& sem = validSurfaceRenderers[i]->GetCopySemaphoreData(currentFrameIndex);
			if (storedSurfaceRendererSemaphoreValues[i] == sem.value)
				continue;

			// Surface renderer's copy semaphore was modified, we need to wait for it's completion.
			waitSemaphores.push_back(sem.semaphore);
			waitValues.push_back(sem.value - 1); // We submit a value and increment always.
		}

		// Submit surface contents.
		m_lgx->SubmitCommandStreams({
			.targetQueue	 = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams		 = surfaceRendererStreams.data(),
			.streamCount	 = static_cast<uint32>(surfaceRendererStreams.size()),
			.useWait		 = !waitSemaphores.empty(),
			.waitCount		 = static_cast<uint32>(waitSemaphores.size()),
			.waitSemaphores	 = waitSemaphores.data(),
			.waitValues		 = waitValues.data(),
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
		window->SetCallbackSizeChanged([this, sid](const LinaGX::LGXVector2ui& newSize) {
			Event ev;
			ev.pParams[0] = m_lgx->GetWindowManager().GetWindow(sid);
			ev.iParams[0] = newSize.x;
			ev.iParams[1] = newSize.y;
			m_system->DispatchEvent(SystemEvent::EVS_WindowResized, ev);
		});

		SurfaceRenderer* renderer = new SurfaceRenderer(this, window, sid, size);
		m_surfaceRenderers.push_back(renderer);

		return window;
	}

	void GfxManager::DestroyApplicationWindow(StringID sid)
	{
		m_lgx->Join();

		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		delete *it;
		m_surfaceRenderers.erase(it);

		m_lgx->GetWindowManager().DestroyApplicationWindow(sid);
	}

	void GfxManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_WindowResized)
		{
			Join();
			for (auto sr : m_surfaceRenderers)
				sr->OnResize(ev.pParams[0], Vector2ui(ev.iParams[0], ev.iParams[1]));
		}
	}

	LinaGX::Window* GfxManager::GetApplicationWindow(StringID sid)
	{
		m_lgx->GetWindowManager().GetWindow(sid);
	}

} // namespace Lina
