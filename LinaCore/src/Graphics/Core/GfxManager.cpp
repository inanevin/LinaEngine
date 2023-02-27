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
#include "Graphics/Core/IGfxResource.hpp"
#include "Graphics/Resource/Material.hpp"
#include "System/ISystem.hpp"
#include "Profiling/Profiler.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Platform/RendererIncl.hpp"

// Debug
#include "Graphics/Core/WorldRenderer.hpp"
#include "World/Core/EntityWorld.hpp"

namespace Lina
{
	WorldRenderer* testWorldRenderer = nullptr;
	EntityWorld*   testWorld		 = nullptr;

	void GfxManager::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		Renderer::PreInitialize(initInfo, this);
	}

	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		Renderer::Initialize(initInfo);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data			  = m_dataPerFrame[i];
			data.globalDataBuffer = Renderer::CreateBufferResource(&m_globalData, sizeof(GPUGlobalData));
		}
	}

	void GfxManager::Shutdown()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_dataPerFrame[i];
			Renderer::DeleteBufferResource(data.globalDataBuffer);
		}

		Renderer::Shutdown();
	}

	void GfxManager::Join()
	{
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) { m_surfaceRenderers[i]->Join(); });
		m_system->GetMainExecutor()->RunAndWait(tf);

		testWorldRenderer->Join();
	}

	void GfxManager::Tick(float delta)
	{
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) { m_surfaceRenderers[i]->Tick(delta); });
		m_system->GetMainExecutor()->RunAndWait(tf);

		testWorldRenderer->Tick(delta);
	}

	void GfxManager::Render()
	{
		auto& dataPerFrame = m_dataPerFrame[m_frameIndex];

		dataPerFrame.globalDataBuffer->Update(&m_globalData);

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

	void GfxManager::OnSystemEvent(ESystemEvent type, const Event& ev)
	{
		auto rm = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		if (type & EVS_PostInit)
		{
			const uint32 engineShaderCount			= 1;
			const String shaders[engineShaderCount] = {"Resources/Core/Shaders/LitStandard.linashader"};
			auto		 rm							= m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

			for (uint32 i = 0; i < engineShaderCount; i++)
			{
				const StringID shaderSID	= TO_SID(shaders[i]);
				const String   materialPath = "Resources/Core/Materials/" + FileSystem::GetFilenameOnlyFromPath(shaders[i]) + ".linamat";
				Material*	   mat			= new Material(rm, true, materialPath, TO_SID(materialPath));
				mat->SetShader(shaderSID);
				m_engineMaterials.push_back(mat);
			}
			WorldRenderer::testSwapchain = m_surfaceRenderers[0]->GetSwapchain();
			testWorld					 = new EntityWorld(m_system);
			testWorldRenderer			 = new WorldRenderer(this, FRAMES_IN_FLIGHT, nullptr, 0, testWorld, Vector2(800, 800), 1);
		}
		else if (type & EVS_PreSystemShutdown)
		{
			delete testWorld;
			delete testWorldRenderer;
			for (auto m : m_engineMaterials)
				delete m;
		}
	}

	IGfxResource* GfxManager::GetCurrentGlobalDataResource()
	{
		return m_dataPerFrame[m_frameIndex].globalDataBuffer;
	}
} // namespace Lina
