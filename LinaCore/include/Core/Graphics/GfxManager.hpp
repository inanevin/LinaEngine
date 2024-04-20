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
*/

#pragma once

#ifndef GfxManager_HPP
#define GfxManager_HPP

#include "Common/Common.hpp"
#include "Common/System/Subsystem.hpp"
#include "Common/StringID.hpp"
#include "Common/Data/Vector.hpp"
#include "MeshManager.hpp"
#include "CommonGraphics.hpp"
#include "ResourceUploadQueue.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

class PoolAllocator;

namespace Lina
{
	class SurfaceRenderer;
	class WorldRenderer;
	class ResourceManager;
	class GUIBackend;
	class EntityWorld;

	class GfxManager : public Subsystem
	{

	private:
		struct PerFrameData
		{
			uint16		  pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Max];
			uint16		  pipelineLayoutPersistentGlobal = 0;
			uint16		  descriptorSetPersistentGlobal	 = 0;
			uint32		  globalDataResource			 = 0;
			uint8*		  globalDataMapped				 = nullptr;
			SemaphoreData worldSignalSemaphore;
		};

	public:
		GfxManager(System* sys);
		~GfxManager() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void PreShutdown() override;
		virtual void Shutdown() override;
		virtual void PreTick() override;

		void			 WaitForSwapchains();
		void			 Join();
		void			 Poll();
		void			 Tick(float delta);
		void			 Render();
		void			 DestroyApplicationWindow(StringID sid);
		LinaGX::Window*	 CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		LinaGX::Window*	 GetApplicationWindow(StringID sid);
		SurfaceRenderer* GetSurfaceRenderer(StringID sid);
		PoolAllocator*	 GetGUIAllocator(TypeID tid, size_t typeSize);
		WorldRenderer*	 CreateWorldRenderer(EntityWorld* world, const Vector2ui& size);
		void			 DestroyWorldRenderer(WorldRenderer* renderer);
		void			 DestroyWorldRenderer(EntityWorld* world);

		uint16 GetDescriptorSetPersistentGlobal(uint32 frameIndex) const
		{
			return m_pfd[frameIndex].descriptorSetPersistentGlobal;
		}

		uint16 GetPipelineLayoutPersistentGlobal(uint32 frameIndex) const
		{
			return m_pfd[frameIndex].pipelineLayoutPersistentGlobal;
		}

		uint16 GetPipelineLayoutPersistentRenderPass(uint32 frameIndex, RenderPassDescriptorType renderPassType) const
		{
			return m_pfd[frameIndex].pipelineLayoutPersistentRenderpass[renderPassType];
		}

		inline MeshManager& GetMeshManager()
		{
			return m_meshManager;
		}

		inline GUIBackend* GetGUIBackend()
		{
			return m_guiBackend;
		}

		inline ResourceUploadQueue& GetResourceUploadQueue()
		{
			return m_resourceUploadQueue;
		}

		inline const LinaGX::VSyncStyle& GetCurrentVsync() const
		{
			return m_currentVsync;
		}

		inline LinaGX::Instance* GetLGX()
		{
			return m_lgx;
		}

	private:
		ResourceUploadQueue				m_resourceUploadQueue;
		MeshManager						m_meshManager;
		Vector<WorldRenderer*>			m_worldRenderers;
		Vector<SurfaceRenderer*>		m_surfaceRenderers;
		GUIBackend*						m_guiBackend	  = nullptr;
		ResourceManager*				m_resourceManager = nullptr;
		Vector<TextureSampler*>			m_defaultSamplers;
		Vector<Material*>				m_defaultMaterials;
		LinaGX::Instance*				m_lgx		   = nullptr;
		LinaGX::VSyncStyle				m_currentVsync = {};
		ApplicationDelegate*			m_appDelegate  = nullptr;
		LinaGX::Window*					m_mainWindow   = nullptr;
		HashMap<TypeID, PoolAllocator*> m_guiAllocators;
		Mutex							m_guiAllocMutx;
		Color							m_clearColor = Color::Black;
		PerFrameData					m_pfd[FRAMES_IN_FLIGHT];
		Mutex							m_wrMtx;
	};
} // namespace Lina
#endif
