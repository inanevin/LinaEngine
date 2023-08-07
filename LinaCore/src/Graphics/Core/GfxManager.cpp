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
	GfxManager::GfxManager(const SystemInitializationInfo& initInfo, ISystem* sys) : ISubsystem(sys, SubsystemType::GfxManager), m_meshManager(this)
	{
		m_resourceManager = sys->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_resourceManager->AddListener(this);
		m_meshManager.Initialize();

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
	}

	void GfxManager::PreShutdown()
	{
		LinaVG::Terminate();

		m_surfaceRenderers.clear();
	}

	void GfxManager::Shutdown()
	{
		m_meshManager.Shutdown();
		m_resourceManager->RemoveListener(this);
	}

	void GfxManager::WaitForSwapchains()
	{
	}

	void GfxManager::Join()
	{
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
	}

	void GfxManager::CreateSurfaceRenderer(StringID sid, const Vector2ui& initialSize)
	{
		SurfaceRenderer* renderer = new SurfaceRenderer(this, sid, initialSize);
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
