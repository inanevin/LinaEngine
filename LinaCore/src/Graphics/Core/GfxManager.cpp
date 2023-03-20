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
#include "Graphics/Core/IGfxBufferResource.hpp"
#include "Graphics/Core/ISwapchain.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Font.hpp"
#include "System/ISystem.hpp"
#include "Profiling/Profiler.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Core/SystemInfo.hpp"
#include "Graphics/Resource/TextureSampler.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

// Debug
#include "Graphics/Core/WorldRenderer.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Math/Math.hpp"

namespace Lina
{
	WorldRenderer*	testWorldRenderer = nullptr;
	EntityWorld*	testWorld		  = nullptr;
	Entity*			camEntity		  = nullptr;
	Vector<Entity*> cubes;

	GfxManager::GfxManager(ISystem* sys) : ISubsystem(sys, SubsystemType::GfxManager), m_meshManager(this)
	{
		m_resourceManager = sys->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	void GfxManager::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		m_renderer = new Renderer();
		m_renderer->PreInitialize(initInfo, this);
		m_resourceManager->AddListener(this);

		// GUIBackend
		{
			m_guiBackend						  = new GUIBackend(this);
			LinaVG::Config.displayPosX			  = 0;
			LinaVG::Config.displayPosY			  = 0;
			LinaVG::Config.displayWidth			  = 0;
			LinaVG::Config.displayHeight		  = 0;
			LinaVG::Config.globalFramebufferScale = 1.0f;
			LinaVG::Config.globalAAMultiplier	  = 1.0f;
			LinaVG::Config.aaEnabled			  = true;
			LinaVG::Config.textCachingEnabled	  = true;
			LinaVG::Config.textCachingSDFEnabled  = true;
			LinaVG::Config.textCacheReserve		  = 10000;
			LinaVG::Config.errorCallback		  = [](const std::string& err) { LINA_ERR(err.c_str()); };
			LinaVG::Config.logCallback			  = [](const std::string& log) { LINA_TRACE(log.c_str()); };
			LinaVG::Backend::BaseBackend::SetBackend(m_guiBackend);
			LinaVG::Initialize();
		}
	}

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_renderer->Initialize(initInfo);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data			  = m_dataPerFrame[i];
			data.globalDataBuffer = m_renderer->CreateBufferResource(BufferResourceType::UniformBuffer, &m_globalData, sizeof(GPUGlobalData));
		}

		m_meshManager.Initialize();
	}

	void GfxManager::PostInitialize()
	{
		// Samplers
		{
			TextureSampler* defaultSampler = new TextureSampler(m_resourceManager, true, "Resource/Core/Samplers/DefaultSampler.linasampler", DEFAULT_SAMPLER_SID);
			SamplerData		samplerData;
			samplerData.minFilter	= Filter::Linear;
			samplerData.magFilter	= Filter::Linear;
			samplerData.mode		= SamplerAddressMode::ClampToEdge;
			samplerData.anisotropy	= 0.0f;
			samplerData.borderColor = Color::White;
			samplerData.mipLodBias	= 0.0f;
			samplerData.minLod		= 0.0f;
			samplerData.maxLod		= 0.0f;
			defaultSampler->SetSamplerData(samplerData);
			m_engineSamplers.push_back(defaultSampler);
		}

		// Shaders & materials
		{
			constexpr uint32 engineShaderCount			= 2;
			const String	 shaders[engineShaderCount] = {"Resources/Core/Shaders/LitStandard.linashader", "Resources/Core/Shaders/ScreenQuads/SQTexture.linashader"};

			for (uint32 i = 0; i < engineShaderCount; i++)
			{
				const StringID shaderSID	= TO_SID(shaders[i]);
				const String   materialPath = "Resources/Core/Materials/" + FileSystem::GetFilenameOnlyFromPath(shaders[i]) + ".linamat";
				Material*	   mat			= new Material(m_resourceManager, true, materialPath, TO_SID(materialPath));
				mat->SetShader(shaderSID);
				m_engineMaterials.push_back(mat);
			}
		}

		m_renderer->ResetResources();

		// Debug
		{
			testWorld = new EntityWorld();
			camEntity = testWorld->CreateEntity("Cam Entity");
			auto cam  = testWorld->AddComponent<CameraComponent>(camEntity);
			camEntity->SetPosition(Vector3(0, 0, -5));
			camEntity->SetRotationAngles(Vector3(0, 0, 0));

			testWorld->SetActiveCamera(cam);
			auto aq = m_resourceManager->GetResource<Model>("Resources/Core/Models/Cube.fbx"_hs)->AddToWorld(testWorld);
			/// auto aq2 = m_resourceManager->GetResource<Model>("Resources/Core/Models/Sphere.fbx"_hs)->AddToWorld(testWorld);
			/// auto aq3 = m_resourceManager->GetResource<Model>("Resources/Core/Models/Capsule.fbx"_hs)->AddToWorld(testWorld);

			aq->SetPosition(Vector3(-3.5f, 0, 0));
			// aq2->SetPosition(Vector3(-3, 0, 0));
			// aq3->SetPosition(Vector3(0, 0, 0));
			cubes.push_back(aq);
			// cubes.push_back(aq2);
			// cubes.push_back(aq3);
			testWorldRenderer = new WorldRenderer(this, BACK_BUFFER_COUNT, m_surfaceRenderers[0], 0, testWorld, Vector2(1440, 960), 1440.0f / 900.0f);
		}

		m_PostInitializeed = true;
	}

	void GfxManager::PreShutdown()
	{
		Join();
		LinaVG::Terminate();

		delete testWorldRenderer;
		delete testWorld;
		for (auto m : m_engineMaterials)
			delete m;
		for (auto s : m_engineSamplers)
			delete s;
		for (auto& sr : m_surfaceRenderers)
			delete sr;
		m_surfaceRenderers.clear();
	}

	void GfxManager::Shutdown()
	{
		Join();

		m_meshManager.Shutdown();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_dataPerFrame[i];
			m_renderer->DeleteBufferResource(data.globalDataBuffer);
		}

		m_renderer->Shutdown();
		delete m_renderer;
		m_resourceManager->RemoveListener(this);
	}

	void GfxManager::WaitForPresentation()
	{
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) { m_surfaceRenderers[i]->WaitForPresentation(); });
		m_system->GetMainExecutor()->RunAndWait(tf);
	}

	void GfxManager::Join()
	{
		m_renderer->Join();
	}

	void GfxManager::Tick(float delta)
	{
		PROFILER_FUNCTION();

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) { m_surfaceRenderers[i]->Tick(delta); });
		m_system->GetMainExecutor()->RunAndWait(tf);

		int i = 0;
		for (auto c : cubes)
		{
			c->AddPosition(Vector3(delta * 0.33f, 0, 0));

			if (c->GetPosition().x > 3.5f)
				c->SetPosition(Vector3(-3.5f, 0.0f, 0.0f));

			// c->SetPosition(Vector3(Math::Sin(SystemInfo::GetAppTimeF() * 0.2f) * 3, 0.0f, 0.0f));
			//  c->AddRotation(Vector3(0, 0, SystemInfo::GetDeltaTimeF() * 35));
			i++;
		}
		if (camEntity)
		{
			// camEntity->SetPosition(Vector3(0,0, Math::Sin(SystemInfo::GetAppTimeF() * 5) * 10));
			// camEntity->SetRotationAngles(Vector3(0, 180 + SystemInfo::GetAppTimeF(), 0));
		}
	}

	void GfxManager::Render()
	{
		PROFILER_FUNCTION();

		auto& frame = m_dataPerFrame[m_frameIndex];

		// Global data
		{
			m_globalData.screenSizeMousePos = Vector2::Zero;
			m_globalData.deltaElapsed		= Vector2(SystemInfo::GetDeltaTimeF(), SystemInfo::GetAppTimeF());
			frame.globalDataBuffer->Update(&m_globalData, sizeof(GPUGlobalData));
		}

		m_renderer->BeginFrame(m_frameIndex);
		LinaVG::StartFrame(static_cast<int>(m_surfaceRenderers.size()));

		// Surface renderers.
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) {
				m_surfaceRenderers[i]->Render(i, m_frameIndex);
				m_surfaceRenderers[i]->Present();
			});
			m_system->GetMainExecutor()->RunAndWait(tf);
		}

		LinaVG::EndFrame();
		m_renderer->EndFrame(m_frameIndex);

		m_frameIndex = (m_frameIndex + 1) % FRAMES_IN_FLIGHT;
	}

	void GfxManager::CreateSurfaceRenderer(StringID sid, IWindow* window, const Vector2i& initialSize, Bitmask16 mask)
	{
		SurfaceRenderer* renderer = new SurfaceRenderer(this, BACK_BUFFER_COUNT, sid, window, initialSize, mask);
		m_surfaceRenderers.push_back(renderer);
	}

	void GfxManager::DestroySurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSwapchain()->GetSID() == sid; });
		m_surfaceRenderers.erase(it);
		delete *it;
	}

	IGfxBufferResource* GfxManager::GetCurrentGlobalDataResource()
	{
		return m_dataPerFrame[m_frameIndex].globalDataBuffer;
	}

	void GfxManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		Bitmask16 flushMask		   = 0;
		bool	  checkForModels   = true;
		bool	  checkForTextures = true;
		bool	  checkForFonts	   = true;

		if (eventType & EVS_ResourceLoadTaskCompleted)
		{
			ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[0]);

			for (const auto& ident : task->identifiers)
			{
				if (checkForModels && ident.tid == GetTypeID<Model>())
				{
					flushMask.Set(UCF_FlushDataRequests);
					m_meshManager.MergeMeshes();
					checkForModels = false;
				}

				if (checkForTextures && ident.tid == GetTypeID<Texture>())
				{
					flushMask.Set(UCF_FlushTextureRequests);
					checkForTextures = false;
				}

				if (checkForFonts && ident.tid == GetTypeID<Font>())
				{
					checkForFonts = false;
					flushMask.Set(UCF_FlushTextureRequests);
				}
			}

			m_guiBackend->OnResourceBatchLoaded(ev);

			if (flushMask.GetValue() != 0)
				m_renderer->GetUploadContext()->Flush(flushMask);
		}
		else if (eventType & EVS_LevelInstalled)
		{
			m_renderer->ResetResources();
		}
	}

	void GfxManager::OnWindowMoved(IWindow* window, StringID sid, const Recti& rect)
	{
	}

	void GfxManager::OnWindowResized(IWindow* window, StringID sid, const Recti& rect)
	{
		if (!m_PostInitializeed)
			return;

		m_renderer->OnWindowResized(window, sid, rect);
	}

	void GfxManager::OnVsyncChanged(VsyncMode mode)
	{
		if (!m_PostInitializeed)
			return;
		m_renderer->OnVsyncChanged(mode);
	}

	SurfaceRenderer* GfxManager::GetSurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSwapchain()->GetSID() == sid; });
		return *it;
	}

} // namespace Lina
