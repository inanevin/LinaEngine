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

#ifndef Engine_HPP
#define Engine_HPP

#include "Common/System/System.hpp"
#include "Core/Audio/AudioManager.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/WorldManager.hpp"
#include "Common/JobSystem/JobSystem.hpp"
#include "EngineInterface.hpp"
#include "Common/Event/SystemEventListener.hpp"

namespace Lina
{
	class Application;
	class GfxManager;

	class Engine : public System, public SystemEventListener
	{
	public:
		Engine(Application* app) : System(app), m_worldManager(this), m_audioManager(this), m_resourceManager(this), m_engineInterface(this), m_gfxManager(this){};

		virtual ~Engine() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void CoreResourcesLoaded() override;
		virtual void PreShutdown() override;
		virtual void Shutdown() override;
		virtual void PreTick() override;
		virtual void Poll() override;
		virtual void Tick() override;
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev) override;

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_ResourceLoaded | EVS_ResourceUnloaded;
		}

		inline GfxManager& GetGfxManager()
		{
			return m_gfxManager;
		}

		inline EngineInterface* GetInterface()
		{
			return &m_engineInterface;
		}

		inline ResourceManager& GetResourceManager()
		{
			return m_resourceManager;
		}

	private:
		void CalculateTime();

	protected:
		ResourceManager m_resourceManager;
		JobExecutor		m_executor;
		AudioManager	m_audioManager;
		GfxManager		m_gfxManager;
		EngineInterface m_engineInterface;
		WorldManager	m_worldManager;

		// Time
		int64 m_frameCapAccumulator		 = 0;
		int64 m_fixedTimestepAccumulator = 0;
	};
} // namespace Lina

#endif
