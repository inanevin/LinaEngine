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

#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Core/WorldRenderer.hpp"
#include "Graphics/Core/LGXWrapper.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/TextureSampler.hpp"
#include "Graphics/Resource/Font.hpp"
#include "System/System.hpp"
#include "Profiling/Profiler.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Core/SystemInfo.hpp"
#include "Graphics/Resource/TextureSampler.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina
{
	GfxManager::GfxManager(const SystemInitializationInfo& initInfo, System* sys) : Subsystem(sys, SubsystemType::GfxManager), m_meshManager(this), m_resourceUploadQueue(this)
	{
		m_resourceManager = sys->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_system->AddListener(this);
		m_lgxWrapper = sys->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);

		// GUIBackend
		{
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
	}

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_resourceUploadQueue.Initialize();
		m_meshManager.Initialize();
		m_currentVsync = initInfo.vsyncStyle;
		m_lgx		   = m_lgxWrapper->GetLGX();

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

			TextureSampler* defaultSampler		  = new TextureSampler(m_resourceManager, true, "Resources/Core/Samplers/DefaultSampler.linasampler", DEFAULT_SAMPLER_SID);
			TextureSampler* defaultGUISampler	  = new TextureSampler(m_resourceManager, true, "Resources/Core/Samplers/DefaultGUISampler.linasampler", DEFAULT_GUI_SAMPLER_SID);
			TextureSampler* defaultGUITextSampler = new TextureSampler(m_resourceManager, true, "Resources/Core/Samplers/DefaultGUITextSampler.linasampler", DEFAULT_GUI_TEXT_SAMPLER_SID);
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
			Material* defaultUnlitMaterial = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultUnlit.linamaterial", DEFAULT_UNLIT_MATERIAL);
			Material* defaultLitMaterial   = new Material(m_resourceManager, true, "Resources/Core/Materials/DefaultLit.linamaterial", DEFAULT_LIT_MATERIAL);
			defaultLitMaterial->SetShader("Resources/Core/Shaders/LitStandard.linashader"_hs);
			defaultUnlitMaterial->SetShader("Resources/Core/Shaders/UnlitStandard.linashader"_hs);
			m_defaultMaterials.push_back(defaultLitMaterial);
			m_defaultMaterials.push_back(defaultUnlitMaterial);
		}

		// pfd
		{
			for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& data = m_pfd[i];

				LinaGX::ResourceDesc globalDataDesc = {
					.size		   = sizeof(GPUGlobalData),
					.typeHintFlags = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
					.heapType	   = LinaGX::ResourceHeap::StagingHeap,
					.debugName	   = "GfxManager: Global Data Buffer",
				};
				data.globalDataResource = m_lgx->CreateResource(globalDataDesc);
				m_lgx->MapResource(data.globalDataResource, data.globalDataMapped);

				data.descriptorSet0GlobalData = m_lgx->CreateDescriptorSet({.bindings = {{
																				.descriptorCount = 1,
																				.type			 = LinaGX::DescriptorType::UBO,
																				.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
																			}}});

				LinaGX::DescriptorUpdateBufferDesc globalDataUpdateDesc = {
					.setHandle	   = data.descriptorSet0GlobalData,
					.binding	   = 0,
					.buffers	   = {data.globalDataResource},
					.isWriteAccess = false,
				};

				m_lgx->DescriptorUpdateBuffer(globalDataUpdateDesc);
			}
		}
	}

	void GfxManager::PreShutdown()
	{
		LinaVG::Terminate();

		for (auto m : m_defaultMaterials)
			delete m;

		for (auto s : m_defaultSamplers)
			delete s;

		for (auto sr : m_surfaceRenderers)
			delete sr;

		m_surfaceRenderers.clear();
	}

	void GfxManager::Shutdown()
	{
		m_system->RemoveListener(this);

		// pfd
		{
			for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& data = m_pfd[i];
				m_lgx->DestroyDescriptorSet(data.descriptorSet0GlobalData);
				m_lgx->DestroyResource(data.globalDataResource);
			}
		}

		m_resourceUploadQueue.Shutdown();
		m_meshManager.Shutdown();
	}

	void GfxManager::WaitForSwapchains()
	{
	}

	void GfxManager::Join()
	{
		return;
		m_lgxWrapper->GetLGX()->Join();
	}

	void GfxManager::Tick(float interpolationAlpha)
	{
		PROFILER_FUNCTION();
	}

	void GfxManager::Sync()
	{
		PROFILER_FUNCTION();
	}

	void GfxManager::Render()
	{
		PROFILER_FUNCTION();

		const uint32 currentFrameIndex = m_lgx->GetCurrentFrameIndex();
		auto&		 currentFrame	   = m_pfd[currentFrameIndex];

		auto id = PROFILER_STARTBLOCK("Flushing Resource Upload Queue");

		// Transfer.
		m_resourceUploadQueue.FlushAll();

		PROFILER_ENDBLOCK(id);

		id = PROFILER_STARTBLOCK("Surface Renderer Eligibility");

		// Determine eligible surface renderers.
		Vector<SurfaceRenderer*> validSurfaceRenderers;
		for (const auto& sr : m_surfaceRenderers)
		{
			if (sr->IsVisible())
				validSurfaceRenderers.push_back(sr);

			m_lgx->SetSwapchainActive(sr->GetSwapchain(), sr->IsVisible());
		}

		PROFILER_ENDBLOCK(id);

		const uint32 surfaceRenderersCount = static_cast<uint32>(validSurfaceRenderers.size());
		const uint32 worldRenderersCount   = static_cast<uint32>(m_worldRenderers.size());
		const uint32 guiThreads			   = worldRenderersCount + surfaceRenderersCount;

		id = PROFILER_STARTBLOCK("Dispatching System Event");

		// Notify
		{
			Event ev	  = {};
			ev.iParams[0] = guiThreads;
			m_system->DispatchEvent(EVS_StartRenderFrame, ev);
		}

		PROFILER_ENDBLOCK(id);

		id = PROFILER_STARTBLOCK("StartFrame");

		m_lgx->StartFrame();
		LinaVG::StartFrame(guiThreads);

		PROFILER_ENDBLOCK(id);

		// Update data.
		{
			// UpdateBindlessTextures();
			// UpdateBindlessSamplers();
			//
			// const auto&	  mp		 = m_lgx->GetInput().GetMousePositionAbs();
			// GPUGlobalData globalData = {
			// 	.mousePosition = Vector2(static_cast<float>(mp.x), static_cast<float>(mp.y)),
			// 	.deltaTime	   = SystemInfo::GetDeltaTimeF(),
			// 	.elapsedTime   = SystemInfo::GetAppTimeF(),
			// };
			// MEMCPY(currentFrame.globalDataMapped, &globalData, sizeof(GPUGlobalData));
		}

		// World Renderers
		{
		}

		id = PROFILER_STARTBLOCK("Render SF");

		// Record surface renderers.
		{
			if (surfaceRenderersCount == 1)
			{
				auto sf = validSurfaceRenderers[0];
				sf->Render(worldRenderersCount, currentFrameIndex);
			}
			else
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(validSurfaceRenderers.size()), 1, [&](int i) {
					auto	  sr	   = validSurfaceRenderers[i];
					const int threadID = worldRenderersCount + i;
					sr->Render(threadID, currentFrameIndex);
				});

				m_system->GetMainExecutor()->RunAndWait(tf);
			}
		}

		PROFILER_ENDBLOCK(id);

		id = PROFILER_STARTBLOCK("Submit");

		// Submit surface renderers.
		{
			Vector<LinaGX::CommandStream*> streams;
			Vector<uint16>				   waitSemaphores;
			Vector<uint64>				   waitValues;

			for (auto sf : validSurfaceRenderers)
			{
				const auto& ws = sf->GetCurrentWaitSemaphores();
				const auto& wv = sf->GetCurrentWaitValues();
				streams.push_back(sf->GetStream(currentFrameIndex));
				waitSemaphores.insert(waitSemaphores.begin(), ws.begin(), ws.end());
				waitValues.insert(waitValues.begin(), wv.begin(), wv.end());
			}

			LinaGX::SubmitDesc desc = {
				.targetQueue	= m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
				.streams		= streams.data(),
				.streamCount	= static_cast<uint32>(streams.size()),
				.useWait		= !waitSemaphores.empty(),
				.waitCount		= static_cast<uint32>(waitSemaphores.size()),
				.waitSemaphores = waitSemaphores.data(),
				.waitValues		= waitValues.data(),
			};

			m_lgx->SubmitCommandStreams(desc);
		}

		PROFILER_ENDBLOCK(id);

		LinaVG::EndFrame();

		id = PROFILER_STARTBLOCK("Present");

		// Present surface renderers.
		{
			// Present.
			Vector<uint8> swapchains;
			swapchains.resize(validSurfaceRenderers.size());

			uint32 i = 0;
			for (auto sf : validSurfaceRenderers)
			{
				swapchains[i] = sf->GetSwapchain();
				i++;
			}

			LinaGX::PresentDesc desc = {
				.swapchains		= swapchains.data(),
				.swapchainCount = static_cast<uint32>(swapchains.size()),
			};

			m_lgx->Present(desc);
		}

		PROFILER_ENDBLOCK(id);

		id = PROFILER_STARTBLOCK("End Frame");

		m_lgx->EndFrame();

		PROFILER_ENDBLOCK(id);
	}

	void GfxManager::CreateSurfaceRenderer(StringID sid, LinaGX::Window* window, const Vector2ui& initialSize)
	{
		SurfaceRenderer* renderer = new SurfaceRenderer(this, window, sid, initialSize);
		m_surfaceRenderers.push_back(renderer);
	}

	void GfxManager::DestroySurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });

		// Might already be deleted due to critical gfx error.
		if (it == m_surfaceRenderers.end())
			return;

		delete *it;
		m_surfaceRenderers.erase(it);
	}

	void GfxManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		bool updateBindlessTextures = false, updateBindlessSamplers = false;

		if (eventType & EVS_ResourceLoadTaskCompleted)
		{
			ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[0]);

			for (const auto& ident : task->identifiers)
			{
				if (ident.tid == GetTypeID<Texture>())
					updateBindlessTextures = true;

				if (ident.tid == GetTypeID<TextureSampler>())
					updateBindlessSamplers = true;
			}
		}
		else if (eventType & EVS_ResourceBatchUnloaded)
		{
			Vector<ResourceIdentifier>* idents = static_cast<Vector<ResourceIdentifier>*>(ev.pParams[0]);

			for (const auto& ident : *idents)
			{
				if (ident.tid == GetTypeID<Texture>())
					updateBindlessTextures = true;

				if (ident.tid == GetTypeID<TextureSampler>())
					updateBindlessSamplers = true;
			}
		}
		else if (eventType & EVS_LevelInstalled)
		{
		}
		else if (eventType & EVS_WindowResized)
		{
			Join();
			for (auto sr : m_surfaceRenderers)
			{
				if (sr->GetWindow() == ev.pParams[0])
					sr->Resize(Vector2ui(ev.iParams[0], ev.iParams[1]));
			}
		}

		if (updateBindlessTextures)
			MarkBindlessTexturesDirty();

		if (updateBindlessSamplers)
			MarkBindlessSamplersDirty();
	}

	SurfaceRenderer* GfxManager::GetSurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		return *it;
	}

	uint16 GfxManager::GetCurrentDescriptorSet0GlobalData()
	{
		return m_pfd[m_lgx->GetCurrentFrameIndex()].descriptorSet0GlobalData;
	}

	void GfxManager::UpdateBindlessTextures()
	{
		// auto& currentFrame = m_pfd[m_lgx->GetCurrentFrameIndex()];
		//
		// if (currentFrame.bindlessTexturesDirty)
		// {
		// 	currentFrame.bindlessTexturesDirty = false;
		//
		// 	Vector<Texture*> allTextures = m_resourceManager->GetAllResourcesRaw<Texture>(true);
		//
		// 	// TODO: ask world renderers for their bindless textures.
		//
		// 	Vector<uint32> textureHandles;
		// 	textureHandles.reserve(allTextures.size());
		//
		// 	uint32 i = 0;
		// 	for (auto txt : allTextures)
		// 	{
		// 		textureHandles.push_back(txt->GetGPUHandle());
		// 		txt->m_bindlessIndex = i;
		// 		i++;
		// 	}
		//
		// 	LinaGX::DescriptorUpdateImageDesc desc = {
		// 		.setHandle		 = currentFrame.descriptorSet0GlobalData,
		// 		.binding		 = 1,
		// 		.descriptorCount = static_cast<uint32>(textureHandles.size()),
		// 		.textures		 = textureHandles.data(),
		// 		.descriptorType	 = LinaGX::DescriptorType::SeparateImage,
		// 	};
		//
		// 	m_lgx->DescriptorUpdateImage(desc);
		// }
	}

	void GfxManager::UpdateBindlessSamplers()
	{
		// auto& currentFrame = m_pfd[m_lgx->GetCurrentFrameIndex()];
		//
		// if (currentFrame.bindlessSamplersDirty)
		// {
		// 	currentFrame.bindlessSamplersDirty = false;
		//
		// 	const auto&	   allSamplers = m_resourceManager->GetAllResourcesRaw<TextureSampler>(true);
		// 	Vector<uint32> samplerHandles;
		// 	samplerHandles.reserve(allSamplers.size());
		//
		// 	uint32 i = 0;
		// 	for (auto sampler : allSamplers)
		// 	{
		// 		samplerHandles.push_back(sampler->GetGPUHandle());
		// 		sampler->m_bindlessIndex = i;
		// 		i++;
		// 	}
		//
		// 	LinaGX::DescriptorUpdateImageDesc desc = {
		// 		.setHandle		 = currentFrame.descriptorSet0GlobalData,
		// 		.binding		 = 2,
		// 		.descriptorCount = static_cast<uint32>(samplerHandles.size()),
		// 		.samplers		 = samplerHandles.data(),
		// 		.descriptorType	 = LinaGX::DescriptorType::SeparateSampler,
		// 	};
		//
		// 	m_lgx->DescriptorUpdateImage(desc);
		// }
	}

} // namespace Lina
