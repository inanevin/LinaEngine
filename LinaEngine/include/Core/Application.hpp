/*
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: Application

Central application class, responsible for managing all the engines like input, physics, rendering etc.
as well as defining the game loop.

Timestamp: 12/29/2018 10:43:46 PM
*/

#pragma once
#ifndef Lina_Application_HPP
#define Lina_Application_HPP

#include "Core/Common.hpp"
#include "Engine.hpp"
#include "EventSystem/Events.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/InputBackend.hpp"
#include "Core/AudioBackend.hpp"
#include "Core/RenderingBackend.hpp"
#include "Core/WindowBackend.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/ResourceManager.hpp"
#include "ECS/ECS.hpp"

namespace Lina
{

	class Application 
	{

	public:
	
		Application() {};
		~Application() {};

		void Startup(ApplicationInfo appInfo = ApplicationInfo());

	private:

		// Callbacks.
		void OnLog(Event::ELog dump);

	private:

		Event::EventSystem m_eventSystem;
		ECS::Registry m_ecs;
		Engine m_engine;
		Graphics::RenderEngineBackend m_renderEngine;
		Resources::ResourceManager m_resourceManager;
		Input::InputEngineBackend m_inputEngine;
		Physics::PhysicsEngineBackend m_physicsEngine;
		Audio::AudioEngineBackend m_audioEngine;
		ApplicationInfo m_appInfo;
	};
};


#endif