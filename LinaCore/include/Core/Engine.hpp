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

#include "System/ISystem.hpp"
#include "Input/Core/Input.hpp"
#include "Audio/Core/AudioManager.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "World/Level/LevelManager.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "JobSystem/JobSystem.hpp"
#include "IEngineInterface.hpp"

namespace Lina
{
	class CoreResourcesRegistry;
	class Application;

	class Engine : public ISystem, public ISystemEventListener
	{
	public:
		Engine(Application* app) : ISystem(app), m_gfxManager(this), m_input(this), m_audioManager(this), m_levelManager(this), m_windowManager(this), m_resourceManager(this), m_engineInterface(this){};

		virtual ~Engine() = default;

		// Inherited via ISystem
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;
		virtual void Tick(float dt) override;
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev) override;

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_ResourceLoaded;
		}

		inline Input& GetInput()
		{
			return m_input;
		}

		inline GfxManager& GetGfxManager()
		{
			return m_gfxManager;
		}

		inline IEngineInterface* GetInterface()
		{
			return &m_engineInterface;
		}

		inline WindowManager& GetWindowManager()
		{
			return m_windowManager;
		}

	protected:
		ResourceManager		   m_resourceManager;
		CoreResourcesRegistry* m_coreResourceRegistry = nullptr;
		Executor			   m_executor;
		Input				   m_input;
		AudioManager		   m_audioManager;
		GfxManager			   m_gfxManager;
		WindowManager		   m_windowManager;
		LevelManager		   m_levelManager;
		IEngineInterface	   m_engineInterface;
	};
} // namespace Lina

#endif
