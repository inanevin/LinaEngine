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
#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "CommonGraphics.hpp"
#include "ResourceUploadQueue.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	class ResourceManager;
	class GUIBackend;
	class Renderer;

	struct RendererPool
	{
		uint32			  order			= 0;
		StringID		  sid			= 0;
		bool			  submitInBatch = false;
		Vector<Renderer*> renderers;
	};

	class GfxManager : public Subsystem, public LinaGX::WindowListener
	{
	private:
		struct PerFrameData
		{
			uint16		  pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Max];
			uint16		  pipelineLayoutPersistentGlobal = 0;
			uint16		  descriptorSetPersistentGlobal	 = 0;
			Buffer		  globalDataBuffer;
			Buffer		  globalMaterialsBuffer;
			Atomic<bool>  bindlessDirty = true;
			SemaphoreData poolSubmissionSemaphore;
		};

	public:
		GfxManager(System* sys);
		~GfxManager() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void PreShutdown() override;
		virtual void Shutdown() override;
		virtual void PreTick() override;
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& sz) override;

		void CreateRendererPool(StringID sid, uint32 order, bool submitInBatch);
		void AddRenderer(Renderer* renderer, StringID pool);
		void RemoveRenderer(Renderer* renderer);

		void			WaitForSwapchains();
		void			Join();
		void			Poll();
		void			Tick(float delta);
		void			Render(StringID pool = 0);
		void			DestroyApplicationWindow(StringID sid);
		LinaGX::Window* CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		LinaGX::Window* GetApplicationWindow(StringID sid);

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

		inline const GUIBackend& GetGUIBackend() const
		{
			return m_guiBackend;
		}

		inline void MarkBindlessDirty()
		{
			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& pfd = m_pfd[i];
				pfd.bindlessDirty.store(true);
			}
		}

		inline TextureSampler* GetDefaultSampler(uint32 index) const
		{
			return m_defaultSamplers[index];
		}

		inline LinaVG::Text& GetLVGText()
		{
			return m_lvgText;
		}

	private:
		void UpdateBindlessResources(PerFrameData& pfd);

	private:
		ResourceUploadQueue		m_resourceUploadQueue;
		MeshManager				m_meshManager;
		GUIBackend				m_guiBackend;
		ResourceManager*		m_resourceManager = nullptr;
		Vector<TextureSampler*> m_defaultSamplers;
		Vector<Material*>		m_defaultMaterials;
		LinaGX::Instance*		m_lgx		   = nullptr;
		LinaGX::VSyncStyle		m_currentVsync = {};
		ApplicationDelegate*	m_appDelegate  = nullptr;
		LinaGX::Window*			m_mainWindow   = nullptr;
		Color					m_clearColor   = Color::Black;
		PerFrameData			m_pfd[FRAMES_IN_FLIGHT];
		Mutex					m_wrMtx;
		Mutex					m_bindlessMtx;
		Vector<RendererPool>	m_rendererPools;
		LinaVG::Text			m_lvgText;
	};
} // namespace Lina
#endif
