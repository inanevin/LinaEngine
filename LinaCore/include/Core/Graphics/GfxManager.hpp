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
#include "GfxMeshManager.hpp"
#include "Common/Event/SystemEventListener.hpp"
#include "CommonGraphics.hpp"
#include "ResourceUploadQueue.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	class SurfaceRenderer;
	class Renderer;
	class ResourceManager;
	class GUIBackend;
	class LGXWrapper;

	class GfxManager : public Subsystem, public SystemEventListener
	{

	private:
		struct PerFrameData
		{
			uint16 descriptorSet0GlobalData = 0;
			uint32 globalDataResource		= 0;
			uint8* globalDataMapped			= nullptr;
		};

	public:
		GfxManager(System* sys);
		~GfxManager() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void PreShutdown() override;
		virtual void Shutdown() override;
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev) override;

		void			 WaitForSwapchains();
		void			 Join();
		void			 Poll();
		void			 Tick(float interpolationAlpha);
		void			 Sync();
		void			 Render();
		void			 DestroyApplicationWindow(StringID sid);
		LinaGX::Window*	 CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		void			 CreateSurfaceRenderer(StringID sid, LinaGX::Window* window, const Vector2ui& initialSize);
		void			 DestroySurfaceRenderer(StringID sid);
		SurfaceRenderer* GetSurfaceRenderer(StringID sid);
		uint16			 GetDescriptorSet0();

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_WindowResized;
		}

		inline const GfxMeshManager& GetMeshManager()
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
		ResourceUploadQueue		 m_resourceUploadQueue;
		GfxMeshManager			 m_meshManager;
		Vector<SurfaceRenderer*> m_surfaceRenderers;
		GUIBackend*				 m_guiBackend	   = nullptr;
		ResourceManager*		 m_resourceManager = nullptr;
		Vector<TextureSampler*>	 m_defaultSamplers;
		Vector<Material*>		 m_defaultMaterials;
		LGXWrapper*				 m_lgxWrapper	= nullptr;
		LinaGX::Instance*		 m_lgx			= nullptr;
		LinaGX::VSyncStyle		 m_currentVsync = {};

		PerFrameData m_pfd[FRAMES_IN_FLIGHT];
	};
} // namespace Lina
#endif
