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

#ifndef Application_HPP
#define Application_HPP

#include "System/ISystem.hpp"
#include "Core/Clock.hpp"
#include "Input/Core/Input.hpp"
#include "Audio/Core/AudioManager.hpp"
#include "Graphics/Platform/GfxManagerIncl.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "World/Level/LevelManager.hpp"
#include "Physics/Core/PhysicsEngine.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "JobSystem/JobSystem.hpp"
#include "IEngineInterface.hpp"

namespace Lina
{
	class CoreResourcesRegistry;

	class Application : public ISystem
	{
	public:
		Application() : m_input(this), m_audioManager(this), m_gfxManager(this), m_levelManager(this), m_physicsEngine(this), m_windowManager(this), m_resourceManager(this), m_engineInterface(this){};

		virtual ~Application() = default;

		// Inherited via ISystem
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void DispatchSystemEvent(ESystemEvent ev, const Event& data) override;
		virtual void Shutdown() override;
		virtual void PreTick() override;
		virtual void Tick() override;
		virtual void Quit() override;

		inline bool GetIsRunning()
		{
			return m_isRunning;
		}

	protected:
		virtual void LoadPlugins();
		virtual void PostInitialize();
		virtual void UnloadPlugins();
		void		 LoadPlugin(const char* name);
		void		 UnloadPlugin(IPlugin* plugin);

	protected:
		bool				   m_isRunning			  = false;
		CoreResourcesRegistry* m_coreResourceRegistry = nullptr;
		Executor			   m_executor;
		Input				   m_input;
		AudioManager		   m_audioManager;
		GfxManager			   m_gfxManager;
		WindowManager		   m_windowManager;
		LevelManager		   m_levelManager;
		PhysicsEngine		   m_physicsEngine;
		ResourceManager		   m_resourceManager;
		IEngineInterface	   m_engineInterface;
	};
} // namespace Lina

#endif
