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
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Model.hpp"
#include "System/ISystem.hpp"
#include "Profiling/Profiler.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Core/SystemInfo.hpp"

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

	void GfxManager::PostInit()
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
		WorldRenderer::testSwapchain = m_surfaceRenderers[0]->GetSwapchain();
		testWorld					 = new EntityWorld();
		camEntity					 = testWorld->CreateEntity("Cam Entity");
		auto cam					 = testWorld->AddComponent<CameraComponent>(camEntity);
		camEntity->SetPosition(Vector3(0, 0, -5));
		camEntity->SetRotationAngles(Vector3(0, 0, 0));

		testWorld->SetActiveCamera(cam);
		auto aq = m_resourceManager->GetResource<Model>("Resources/Core/Models/Cube.fbx"_hs)->AddToWorld(testWorld);
		auto aq2 = m_resourceManager->GetResource<Model>("Resources/Core/Models/Sphere.fbx"_hs)->AddToWorld(testWorld);
		auto aq3 = m_resourceManager->GetResource<Model>("Resources/Core/Models/Capsule.fbx"_hs)->AddToWorld(testWorld);

		aq->SetPosition(Vector3(3, 0, 0));
		aq2->SetPosition(Vector3(-3, 0, 0));
		aq3->SetPosition(Vector3(0, 0, 0));
		cubes.push_back(aq);
		cubes.push_back(aq2);
		cubes.push_back(aq3);
		testWorldRenderer = new WorldRenderer(this, BACK_BUFFER_COUNT, nullptr, 0, testWorld, Vector2(1440, 960), 1440.0f / 900.0f);
	}

	void GfxManager::PreShutdown()
	{
		Join();
		delete testWorld;
		delete testWorldRenderer;
		for (auto m : m_engineMaterials)
			delete m;
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

	void GfxManager::Join()
	{
		m_renderer->Join();
	}

	void GfxManager::Tick(float delta)
	{
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) { m_surfaceRenderers[i]->Tick(delta); });
		m_system->GetMainExecutor()->RunAndWait(tf);

		testWorldRenderer->Tick(delta);

		for (auto c : cubes)
		{
			c->AddRotation(Vector3(0, 0, SystemInfo::GetDeltaTimeF() * 35));
		}
		if (camEntity)
		{
			// camEntity->SetPosition(Vector3(0,0, Math::Sin(SystemInfo::GetAppTimeF() * 5) * 10));
			// camEntity->SetRotationAngles(Vector3(0, 180 + SystemInfo::GetAppTimeF(), 0));
		}
	}

	void GfxManager::Render()
	{
		auto& frame = m_dataPerFrame[m_frameIndex];

		m_globalData.screenSizeMousePos = Vector2::Zero;
		m_globalData.deltaElapsed		= Vector2(SystemInfo::GetDeltaTimeF(), SystemInfo::GetAppTimeF());
		frame.globalDataBuffer->Update(&m_globalData, sizeof(GPUGlobalData));

		m_renderer->BeginFrame(m_frameIndex);

		// Surface renderers.
		{
			PROFILER_SCOPE("All Surface Renderers", PROFILER_THREAD_RENDER);

			// Taskflow tf;
			// tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) {
			// 	m_surfaceRenderers[i]->SetThreadID(i);
			// 	m_surfaceRenderers[i]->Render();
			// });
			// m_system->GetMainExecutor()->RunAndWait(tf);
			//
			// for (auto sr : m_surfaceRenderers)
			// 	sr->Present();
		}

		testWorldRenderer->Render(m_frameIndex);

		m_renderer->EndFrame(m_frameIndex);

		m_frameIndex = (m_frameIndex + 1) % FRAMES_IN_FLIGHT;
	}

	void GfxManager::CreateSurfaceRenderer(StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask)
	{
		SurfaceRenderer* renderer = new SurfaceRenderer(this, FRAMES_IN_FLIGHT, sid, windowHandle, initialSize, mask);
		m_surfaceRenderers.push_back(renderer);
	}

	void GfxManager::DestroySurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		m_surfaceRenderers.erase(it);
		delete *it;
	}

	IGfxBufferResource* GfxManager::GetCurrentGlobalDataResource()
	{
		return m_dataPerFrame[m_frameIndex].globalDataBuffer;
	}

	void GfxManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_ResourceBatchLoaded)
		{
			Vector<ResourceIdentifier>* idents = static_cast<Vector<ResourceIdentifier>*>(ev.pParams[0]);
			const uint32				size   = static_cast<uint32>(idents->size());

			for (uint32 i = 0; i < size; i++)
			{
				const ResourceIdentifier& ident = idents->at(i);

				if (ident.tid == GetTypeID<Model>())
				{
					m_meshManager.MergeMeshes();
					break;
				}
			}
		}
	}

	void GfxManager::OnWindowMove(void* windowHandle, StringID sid, const Recti& rect)
	{
	}

	void GfxManager::OnWindowResize(void* windowHandle, StringID sid, const Recti& rect)
	{
	}

	void GfxManager::OnVsyncChanged(VsyncMode mode)
	{
	}

} // namespace Lina
