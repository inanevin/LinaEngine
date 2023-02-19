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

#include "Graphics/Platform/D3D12/Core/D3D12GfxManager.hpp"
#include "Graphics/Platform/D3D12/Core/D3D12SurfaceRenderer.hpp"
#include "Data/CommonData.hpp"
#include "System/ISystem.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina
{
	void D3D12GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_gpuStorage.Initilalize();
	}

	void D3D12GfxManager::InitializeEarly(const SystemInitializationInfo& initInfo)
	{
		m_backend.Initialize(initInfo);
	}

	void D3D12GfxManager::Shutdown()
	{
		m_backend.Shutdown();
		m_gpuStorage.Shutdown();
	}

	void D3D12GfxManager::Join()
	{
	}

	void D3D12GfxManager::Tick(float delta)
	{
	}

	void D3D12GfxManager::Render()
	{
		// Surface renderers.
		{
			PROFILER_SCOPE("All Surface Renderers", PROFILER_THREAD_RENDER);

			
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_surfaceRenderers.size()), 1, [&](int i) {
				m_surfaceRenderers[i]->SetThreadID(i);
				m_surfaceRenderers[i]->Render();
			});
			m_system->GetMainExecutor()->RunAndWait(tf);

			for (auto sr : m_surfaceRenderers)
				sr->Present();
		}
	}

	void D3D12GfxManager::OnSystemEvent(ESystemEvent type, const Event& ev)
	{
	}

	void D3D12GfxManager::CreateSurfaceRenderer(StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask)
	{
		D3D12SurfaceRenderer* renderer = new D3D12SurfaceRenderer(this, sid, windowHandle, initialSize, mask);
		m_surfaceRenderers.push_back(renderer);
	}

	void D3D12GfxManager::DestroySurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](D3D12SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		m_surfaceRenderers.erase(it);
		delete *it;
	}
} // namespace Lina
