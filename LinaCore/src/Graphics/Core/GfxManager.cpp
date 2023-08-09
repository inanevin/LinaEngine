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
#include "Graphics/Resource/Font.hpp"
#include "System/ISystem.hpp"
#include "Profiling/Profiler.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Core/SystemInfo.hpp"
#include "Graphics/Resource/TextureSampler.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina
{
	GfxManager::GfxManager(const SystemInitializationInfo& initInfo, ISystem* sys) : ISubsystem(sys, SubsystemType::GfxManager), m_meshManager(this), m_resourceUploadQueue(this)
	{
		m_resourceManager = sys->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_resourceManager->AddListener(this);
		m_lgxWrapper = sys->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		m_lgx		 = m_lgxWrapper->GetLGX();

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
		m_currentVsync = initInfo.vsyncMode;

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
			samplerData.magFilter				 = LinaGX::Filter::Anisotropic;
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
		m_resourceUploadQueue.Shutdown();
		m_meshManager.Shutdown();
		m_resourceManager->RemoveListener(this);
	}

	void GfxManager::WaitForSwapchains()
	{
	}

	void GfxManager::Join()
	{
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
		m_resourceUploadQueue.FlushAll();

		Vector<SurfaceRenderer*> validSurfaceRenderers;
		for (const auto& sr : m_surfaceRenderers)
		{
			if (sr->IsVisible())
				validSurfaceRenderers.push_back(sr);
		}
		const uint32 surfaceRenderersCount = static_cast<uint32>(validSurfaceRenderers.size());
		const uint32 worldRenderersCount   = static_cast<uint32>(m_worldRenderers.size());

		m_lgx->StartFrame();
		LinaVG::StartFrame(worldRenderersCount + surfaceRenderersCount);

		if (surfaceRenderersCount == 1)
		{
			validSurfaceRenderers[0]->Render(worldRenderersCount, currentFrameIndex, hasTransferOperation);
			validSurfaceRenderers[0]->Present();
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(validSurfaceRenderers.size()), 1, [&](int i) {
				auto	  sr	   = validSurfaceRenderers[i];
				const int threadID = worldRenderersCount + i;
				sr->Render(threadID, currentFrameIndex);
				sr->Present();
			});

			m_system->GetMainExecutor()->RunAndWait(tf);
		}

		LinaVG::EndFrame();
		m_lgx->EndFrame();
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
		bool flushModels  = false;
		bool requireReset = false;

		if (eventType & EVS_ResourceLoadTaskCompleted)
		{
			ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[0]);

			for (const auto& ident : task->identifiers)
			{
				if (!flushModels && ident.tid == GetTypeID<Model>())
					flushModels = true;

				if (!requireReset && (ident.tid == GetTypeID<Texture>() || ident.tid == GetTypeID<Font>() || ident.tid == GetTypeID<TextureSampler>()))
					requireReset = true;
			}
		}
		else if (eventType & EVS_LevelInstalled)
		{
		}
	}

	SurfaceRenderer* GfxManager::GetSurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		return *it;
	}

} // namespace Lina
