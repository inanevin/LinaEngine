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

#pragma once

#ifndef D3D12GfxManager_HPP
#define D3D12GfxManager_HPP

#include "System/ISubsystem.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Core/IGfxManager.hpp"
#include "Data/Vector.hpp"
#include "D3D12Backend.hpp"
#include "D3D12GpuStorage.hpp"

#include "Graphics/Platform/D3D12/WinHeaders/d3d12.h"
#include <wrl/client.h>

namespace Lina
{
	class DX12SurfaceRenderer;

	class DX12GfxManager : public IGfxManager
	{
	public:
		DX12GfxManager(ISystem* sys) : m_gpuStorage(this), IGfxManager(sys, &m_gpuStorage, &m_backend){};
		virtual ~DX12GfxManager() = default;

		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void InitializeEarly(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;
		virtual void Join() override;
		virtual void Tick(float delta) override;
		virtual void Render() override;
		virtual void OnSystemEvent(ESystemEvent type, const Event& ev) override;
		virtual void CreateSurfaceRenderer(StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask) override;
		virtual void DestroySurfaceRenderer(StringID sid) override;

		inline DX12Backend* GetD3D12Backend()
		{
			return &m_backend;
		}

		inline DX12GpuStorage* GetD3D12GpuStorage()
		{
			return &m_gpuStorage;
		}

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_PostSystemInit | EVS_PreSystemShutdown | EVS_ResourceBatchLoaded | EVG_LevelInstalled | EVS_WindowResize;
		}

		inline ID3D12RootSignature* GetRootSignature()
		{
			return m_rootSignature.Get();
		}

	private:
		Vector<DX12SurfaceRenderer*>				m_surfaceRenderers;
		DX12GpuStorage								m_gpuStorage;
		DX12Backend								m_backend;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		// Inherited via IGfxManager
	};
} // namespace Lina

#endif
