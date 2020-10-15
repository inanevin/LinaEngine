/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: EditorApplication
Timestamp: 10/15/2020 10:44:39 PM

*/
#pragma once

#ifndef EditorApplication_HPP
#define EditorApplication_HPP

// Headers here.
#include "Core/GUILayer.hpp"
#include "Core/SplashScreen.hpp"

namespace LinaEditor
{
	class EditorApplication
	{
	public:

		EditorApplication() {}

		void Initialize(LinaEngine::Graphics::Window* appWindow, LinaEngine::Graphics::RenderEngine* renderEngine, LinaEngine::Application* app, LinaEngine::Physics::PhysicsEngine* physicsEngine, LinaEngine::ECS::ECSRegistry* ecs);

		~EditorApplication()
		{
			LINA_CLIENT_TRACE("[Destructor] -> Editor Application ({0})", typeid(*this).name());
		}

	private:

		GUILayer* m_guiLayer;
	};
}

#endif
