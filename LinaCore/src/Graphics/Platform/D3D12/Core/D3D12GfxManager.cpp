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
#include "Graphics/Platform/D3D12/WinHeaders/d3dx12.h"
#include "Graphics/Platform/D3D12/Utility/D3D12Helpers.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	void DX12GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_gpuStorage.Initilalize();

		// Create an empty root signature.
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
			ThrowIfFailed(m_backend.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		}
	}

	void DX12GfxManager::InitializeEarly(const SystemInitializationInfo& initInfo)
	{
		m_backend.Initialize(initInfo);
	}

	void DX12GfxManager::Shutdown()
	{
		m_backend.Shutdown();
		m_gpuStorage.Shutdown();
	}

	void DX12GfxManager::Join()
	{
	}

	void DX12GfxManager::Tick(float delta)
	{
	}

	void DX12GfxManager::Render()
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

	void DX12GfxManager::OnSystemEvent(ESystemEvent type, const Event& ev)
	{
	}

	void DX12GfxManager::CreateSurfaceRenderer(StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask)
	{
		DX12SurfaceRenderer* renderer = new DX12SurfaceRenderer(this, sid, windowHandle, initialSize, mask);
		m_surfaceRenderers.push_back(renderer);
	}

	void DX12GfxManager::DestroySurfaceRenderer(StringID sid)
	{
		auto it = linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sid](DX12SurfaceRenderer* renderer) { return renderer->GetSID() == sid; });
		m_surfaceRenderers.erase(it);
		delete *it;
	}
} // namespace Lina
