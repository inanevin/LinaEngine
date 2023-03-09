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

#include "Core/Common.hpp"
#include "System/ISubsystem.hpp"
#include "Core/StringID.hpp"
#include "Data/Vector.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "GfxMeshManager.hpp"
#include "Event/ISystemEventListener.hpp"

namespace Lina
{
	class Material;
	class SurfaceRenderer;
	class IGfxBufferResource;
	class Renderer;
	class ResourceManager;
	class Recti;

	class GfxManager : public ISubsystem, public ISystemEventListener
	{

	private:
		struct DataPerFrame
		{
			IGfxBufferResource* globalDataBuffer = nullptr;
		};

	public:
		GfxManager(ISystem* sys);
		virtual ~GfxManager() = default;

		virtual void		PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void		Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void		PostInit() override;
		virtual void		PreShutdown() override;
		virtual void		Shutdown() override;
		void				Join();
		void				Tick(float delta);
		void				Render();
		void				CreateSurfaceRenderer(StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask);
		void				DestroySurfaceRenderer(StringID sid);
		IGfxBufferResource* GetCurrentGlobalDataResource();
		virtual void		OnSystemEvent(SystemEvent eventType, const Event& ev) override;
		void				OnWindowMove(void* windowHandle, StringID sid, const Recti& rect);
		void				OnWindowResize(void* windowHandle, StringID sid, const Recti& rect);
		void				OnVsyncChanged(VsyncMode mode);

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_ResourceBatchLoaded;
		}

		inline const GfxMeshManager& GetMeshManager()
		{
			return m_meshManager;
		}

		inline Renderer* GetRenderer()
		{
			return m_renderer;
		}

	private:
		ResourceManager*		 m_resourceManager = nullptr;
		Renderer*				 m_renderer		   = nullptr;
		GfxMeshManager			 m_meshManager;
		DataPerFrame			 m_dataPerFrame[FRAMES_IN_FLIGHT];
		GPUGlobalData			 m_globalData;
		Vector<Material*>		 m_engineMaterials;
		Vector<SurfaceRenderer*> m_surfaceRenderers;
		uint32					 m_frameIndex = 0;
	};
} // namespace Lina
#endif
