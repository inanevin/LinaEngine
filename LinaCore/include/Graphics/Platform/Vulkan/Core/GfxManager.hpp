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

#ifndef GfxManager_HPP
#define GfxManager_HPP

#include "System/ISubsystem.hpp"
#include "GPUStorage.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "GfxBackend.hpp"
#include "Data/HashMap.hpp"
#include "Data/DelegateQueue.hpp"
#include "Data/ActionQueue.hpp"
#include "Graphics/Platform/Vulkan/Utility/VulkanStructures.hpp"
#include "Graphics/Platform/Vulkan/Objects/PipelineLayout.hpp"
#include "Graphics/Platform/Vulkan/Objects/Buffer.hpp"
#include "Graphics/Platform/Vulkan/Core/UploadContext.hpp"

namespace Lina
{
	class Material;
	class Renderer;
	class WindowManager;
	class EntityWorld;
	class WorldRenderer;
	class SurfaceRenderer;

	struct RenderFrame
	{
		Fence graphicsFence;
	};

	class GfxManager : public ISubsystem
	{
	public:
		GfxManager(ISystem* sys) : m_gpuStorage(this), ISubsystem(sys, SubsystemType::GfxManager), m_backend(this), m_gpuUploader(&m_backend){};
		virtual ~GfxManager() = default;

		// ****************** ENGINE API ****************** //
		virtual void	 Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void	 Shutdown() override;
		void			 Join();
		void			 Tick(float delta);
		void			 Render();
		void			 SyncData(float alpha);
		SurfaceRenderer* CreateSurfaceRenderer(Swapchain* swapchain, Bitmask16 mask);
		void			 DestroySurfaceRenderer(Swapchain* swapchain);
		virtual void	 OnSystemEvent(ESystemEvent type, const Event& ev);

		inline GPUStorage& GetGPUStorage()
		{
			return m_gpuStorage;
		}

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_PostSystemInit | EVS_PreSystemShutdown | EVS_ResourceBatchLoaded | EVG_LevelInstalled | EVS_WindowResize;
		}

		inline uint32 GetFrameIndex()
		{
			return m_frameNumber % FRAMES_IN_FLIGHT;
		}

		// ****************** VULKAN API ****************** //
		DescriptorSetLayout& GetLayout(DescriptorSetType type);

		inline GfxBackend* GetBackend()
		{
			return &m_backend;
		}

		inline DelegateQueue* GetDeletionQueue()
		{
			return &m_deletionQueue;
		}

		inline UploadContext& GetGPUUploader()
		{
			return m_gpuUploader;
		}

		inline ActionQueue& GetSyncQueue()
		{
			return m_syncQueue;
		}

		inline const HashMap<Mesh*, MergedBufferMeshEntry>& GetMeshEntries()
		{
			return m_meshEntries;
		}

		inline const Buffer& GetGPUVertexBuffer()
		{
			return m_gpuVtxBuffer;
		}

		inline const Buffer& GetGPUIndexBuffer()
		{
			return m_gpuIndexBuffer;
		}

		inline const PipelineLayout& GetGlobalAndPassLayouts()
		{
			return m_globalAndPassLayout;
		}

		inline const GPUGlobalData& GetGPUGlobalData()
		{
			return m_globalData;
		}

		inline const GPUDebugData& GetGPUDebugData()
		{
			return m_debugData;
		}

	private:
		WorldRenderer*									m_currentLevelRenderer		   = nullptr;
		SurfaceRenderer*								m_mainSwapchainSurfaceRenderer = nullptr;
		WindowManager*									m_windowManager				   = nullptr;
		RenderFrame										m_frames[FRAMES_IN_FLIGHT];
		uint32											m_frameNumber = 0;
		GfxBackend										m_backend;
		GPUStorage										m_gpuStorage;
		DelegateQueue									m_deletionQueue;
		ActionQueue										m_syncQueue;
		UploadContext									m_gpuUploader;
		HashMap<DescriptorSetType, DescriptorSetLayout> m_descriptorLayouts;
		PipelineLayout									m_globalAndPassLayout;
		Vector<Material*>								m_engineMaterials;
		Vector<SurfaceRenderer*>						m_surfaceRenderers;
		HashMap<Mesh*, MergedBufferMeshEntry>			m_meshEntries;
		Vector<StringID>								m_mergedModelIDs;
		Buffer											m_cpuVtxBuffer;
		Buffer											m_cpuIndexBuffer;
		Buffer											m_gpuVtxBuffer;
		Buffer											m_gpuIndexBuffer;
		GPUGlobalData									m_globalData;
		GPUDebugData									m_debugData;
	};
} // namespace Lina

#endif
